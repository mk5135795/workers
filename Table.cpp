#include "Table.h"
#include <string>

Table::Table(std::weak_ptr<Object> row, int n)
	: _obj_row(row)
{
	_storage.resize(n, 0);
	_mutex = new std::mutex[n];
	_sen = std::make_unique<Sensor>(n);
}

int Table::Get(int id, int& quantity, int weight)
{
	{
		std::unique_lock<std::mutex> lk(_mutex[id], std::try_to_lock);
		//if already taken
		if (!lk.owns_lock())
			return -1;

		//if enough
		if (_storage[id] >= quantity)
		{
			_storage[id] -= quantity;
			return 1;
		}

		//if not enough
		_sen->Report(Sensor::Info::more, id, weight);
		quantity = _storage[id];
		_storage[id] = 0;
	}
	return 0;
}

void Table::Put(int id, int quantity)
{
	int tmp;
	{
		std::lock_guard<std::mutex> lk(_mutex[id]);
		_storage[id] += quantity;
		tmp = _storage[id];
	}

	//if overflow
	if(tmp >= 250)
		_sen->Report(Sensor::Info::less, id);
}

Sensor* Table::GetSensor() {
	return _sen.get();
}

void Table::Update() {
	int tmp;
	std::wstring wstr;

	for (int i(_storage.size() - 1); i >= 0; i--)
	{
	    //copy & unlock
		{
			std::lock_guard<std::mutex> lk(_mutex[i]);
			tmp = _storage[i];
		}

		//convert
		if (tmp == 0)
			wstr = L"  0";
		else if (tmp < 10)
			wstr = L"  " + std::to_wstring(tmp);
		else if (tmp < 100)
			wstr = L" " + std::to_wstring(tmp);
		else
			wstr = std::to_wstring(tmp);

        //set
        if(auto ptr_obj_row = _obj_row.lock())
            ptr_obj_row->SetString(wstr, Object::Color::normal, i);
	}
}
