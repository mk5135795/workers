#include "Worker.h"

#include <string>

std::atomic<bool> Worker::_s_run(true);
std::condition_variable Worker::_s_cv;
std::vector<Worker*> Worker::_s_workers;

Worker::Worker(Table* tbl, std::weak_ptr<Object> obj, std::weak_ptr<Object> time, std::weak_ptr<Object> row, std::vector<int>&& pattern)
	: _delivery(false), _pattern(pattern), _obj(obj), _obj_time(time), _obj_row(row), _table(tbl)
{
	_storage.resize(_pattern.size(), 0);
	_s_workers.emplace_back(this);
}

bool Worker::Work(int seconds) {
    seconds *= (1. - (rand() % 5 - 2)/10.);

    if(auto ptr_obj = _obj.lock())
        ptr_obj->SetString(L"(•‸•)", Object::Color::work);

	for(int i(_storage.size() - 1); i >= 0; i--)
    {
		_storage[i] -= _pattern[i];
		if(auto ptr_obj_row = _obj_row.lock())
            ptr_obj_row->SetString(std::to_wstring(0), Object::Color::normal, i);
    }

	return Timer(seconds*10, false);
}

bool Worker::Collect(int seconds) {
	auto time = std::chrono::high_resolution_clock::now();
	bool complete;
	int amount;
	int time_old;
	std::vector<int> resource_state(_storage.size(), 0);
	std::chrono::duration<float> elapsed;

	if(auto ptr_obj = _obj.lock())
        ptr_obj->SetString(L"(˃д˂)", Object::Color::collect);

    seconds *= (1. - (rand() % 5 - 2)/10.);
    time_old = seconds/2;

	do {
        _delivery = false;
		if (!_s_run)
			return false;

		//try to get every item
		do {
			complete = true;
			for (int i(_storage.size() - 1); i >= 0; i--)
			{
				//if checked already
				if (resource_state[i] > 0)
					continue;
                else
					complete = false;

				//if not sufficient
				if ((amount = _pattern[i] - _storage[i]) > 0)
				{
					//2  enough
					//1  less than needed
					//0 try again(res in use)
                    resource_state[i] = _table->Get(i, amount, 10*((float)time_old/seconds)) + 1;
                    if(resource_state[i] > 0)
                    {
                        _storage[i] += amount;
                        if(auto ptr_obj_row = _obj_row.lock())
                            ptr_obj_row->SetString(std::to_wstring(_pattern[i]-_storage[i]), resource_state[i], i);
                    }
				}
				else
					resource_state[i] = 2;
			}

            //if timeout
            elapsed = std::chrono::high_resolution_clock::now() - time;
            if (elapsed.count() >= seconds)
            {
                Fired();
                return false;
            }
            //time left
            elapsed = std::chrono::duration<float>(seconds) - elapsed;
            if(time_old != (int)(elapsed.count()*10))
            {
                time_old = elapsed.count()*10;
                ConvertTime(time_old);
            }

        //while not checked all
		} while (!complete);


		//check if done
		complete = true;
		for (int& res : resource_state)
		{
			if (res < 2)
			{
				res = 0;
				complete = false;
			}
		}
		if (complete)
			return true;

	//if delivery or end -> repeat
	} while (!Timer(elapsed.count()*10, true));

    Fired();
	return false;
}

bool Worker::Rest(int seconds) {
    seconds *= (1. - (rand() % 5 - 2)/10.);

	if(auto ptr_obj = _obj.lock())
        ptr_obj->SetString(L"(╴▵╶)", Object::Color::rest);

	return Timer(seconds*10, false);
}

void Worker::Fired() {
	if(auto p_obj = _obj.lock())
        p_obj->SetString(L"(ó‸ò)", Object::Color::fired);
	if(auto ptr_obj_time = _obj_time.lock())
        ptr_obj_time->SetString(L"--.-", Object::Color::hidden);
    if(auto ptr_obj_row = _obj_row.lock())
        for(int i(_pattern.size() - 1); i >= 0; i--)
            ptr_obj_row->SetString(std::to_wstring(_pattern[i]-_storage[i]), Object::Color::fired, i);
}

void Worker::NewDelivery() {
	for (Worker* w : _s_workers)
		w->_delivery = true;
	_s_cv.notify_all();
}

void Worker::EndAll() {
	_s_run = false;
	_s_cv.notify_all();
}

bool Worker::Timer(int time, bool delivery_check) {
	std::unique_lock<std::mutex> lk(_mutex);
	for (int i(time - 1); i >= 0; i--)
	{
	    ConvertTime(i);
        //if end or delivery
		if (_s_cv.wait_for(lk, std::chrono::milliseconds(100), [this, &delivery_check] {
			if(delivery_check)
				return _delivery || !_s_run;
			else
				return !_s_run;
			}))
			return false;
	}
	return true;
}

void Worker::ConvertTime(int time) {
    std::wstring wstr;
    if(time < 10)
        wstr = L" 0." + std::to_wstring(time);
    else
    {
        if(time < 100)
            wstr = L" " + std::to_wstring(time) + L".";
        else
            wstr = std::to_wstring(time) + L".";
        std::swap(wstr[2], wstr[3]);
    }
    if(auto ptr_obj_time = _obj_time.lock())
        ptr_obj_time->SetString(wstr, Object::Color::normal);
}
