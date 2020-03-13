#include "Supplier.h"
#include <string>
#include "Worker.h"

Supplier::Supplier(Table* tbl, std::weak_ptr<Object> obj, std::weak_ptr<Object> time, int refresh_rate, int max_items, int resources_n)
	: _run(false), _ref_rate(refresh_rate), _max_amount(max_items), _res_n(resources_n), _refill(nullptr), _table(tbl), _obj(obj), _obj_time(time)
{}

void Supplier::Fill() {
    std::wstring wstr;
	std::vector<int> drop_chance(_res_n, 1);
	std::vector<int> res(_res_n, 0);
	int i;
	int rnd;
	int prob;
	int sum;

	sum = _table->GetSensor()->GetRaport(drop_chance);
	if(sum == 0)
    {
        for (int i(_res_n - 1); i >= 0; i--)
            if(auto ptr_obj = _obj.lock())
                ptr_obj->SetString(L"  0", Object::Color::normal, i);
        return;
    }

	for(int j(_max_amount - 1); j >= 0; j--)
	{
		prob = 0;
		rnd = rand() % sum;
		for (i = (_res_n - 1); i >= 0; i--)
		{
			prob += drop_chance[i];
			if (rnd < prob)
			{
				res[i]++;
				i = -1;
				break;
			}
		}
		if (i == 0)
			res[0]++;
	}

	for (int i(_res_n - 1); i >= 0; i--)
    {
		_table->Put(i, res[i]);

		//convert
		if (res[i] == 0)
			wstr = L"  0";
		else if (res[i] < 10)
			wstr = L"  " + std::to_wstring(res[i]);
		else if (res[i] < 100)
			wstr = L" " + std::to_wstring(res[i]);
		else
			wstr = std::to_wstring(res[i]);
        if(auto ptr_obj = _obj.lock())
            ptr_obj->SetString(wstr, Object::Color::info, i);
    }
}

void Supplier::Start() {
	_run = true;
	_refill = new std::thread([this]{ Timer(); });
}

void Supplier::Stop() {
	_run = false;
	_cv.notify_one();
	if (_refill->joinable())
		_refill->join();
}

void Supplier::Timer() {
    std::wstring wstr;
	std::unique_lock<std::mutex> lk(_mutex);
	while (true)
    {
        for (int i(_ref_rate*10 - 1); i >= 0; i--)
        {
            //convert
            if(i < 10)
                wstr = L" 0." + std::to_wstring(i);
            else
            {
                if(i < 100)
                    wstr = L" " + std::to_wstring(i) + L".";
                else
                    wstr = std::to_wstring(i) + L".";
                std::swap(wstr[2], wstr[3]);
            }
            if(auto p_time = _obj_time.lock())
                p_time->SetString(wstr, Object::Color::normal);

            //if end
            if (_cv.wait_for(lk, std::chrono::milliseconds(100), [this] { return !_run; }))
                return;
        }

        //deliver
        this->Fill();
        Worker::NewDelivery();
    }
}
