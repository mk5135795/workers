#pragma once

#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>

#include "Object.h"
#include "Table.h"

class Worker
{
public:

	Worker(Table* tbl, std::weak_ptr<Object> obj, std::weak_ptr<Object> time, std::weak_ptr<Object> row, std::vector<int>&& pattern);

	bool Work(int seconds);
	bool Collect(int seconds);
	bool Rest(int seconds);
	void Fired();

	static void NewDelivery();
	static void EndAll();

private:
	bool Timer(int time, bool delivery_enable);
	void ConvertTime(int time);

	std::atomic<bool> _delivery;
	std::vector<int> _pattern;
	std::vector<int> _storage;

	std::mutex _mutex;

	std::weak_ptr<Object> _obj;
	std::weak_ptr<Object> _obj_time;
	std::weak_ptr<Object> _obj_row;

	Table* _table;

	static std::atomic<bool> _s_run;
	static std::condition_variable _s_cv;
	static std::vector<Worker*> _s_workers;
};

