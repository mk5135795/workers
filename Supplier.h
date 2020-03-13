#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Table.h"
#include "Object.h"

class Supplier
{
public:
	Supplier(Table* tbl, std::weak_ptr<Object> obj, std::weak_ptr<Object> time, int refresh_rate, int max_items, int resources_n);
	void Fill();
	void Start();
	void Stop();

private:
    void Timer();

	std::atomic<bool> _run;
	int _ref_rate;
	int _max_amount;
	int _res_n;

	std::mutex _mutex;
	std::condition_variable _cv;
	std::thread* _refill;
	Table* _table;

	std::weak_ptr<Object> _obj;
	std::weak_ptr<Object> _obj_time;
};

