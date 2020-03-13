#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <functional>
#include <memory>
#include <vector>

#include <ncursesw/curses.h>

#include "Object.h"
#include "Table.h"

class Graphic
{
public:
	Graphic();
	~Graphic();
	std::shared_ptr<Object> CreateObject(int y, int x);
	std::shared_ptr<Object> CreateObject(std::vector<Object::text>&& vec);
	void Start(int threads_n, int items_n, std::function<void()> update);
	void Stop();
private:
	void Gen(int threads_n, int items_n);
	void Refresh();

	WINDOW* _win;
	std::atomic<bool> _run;
	std::mutex _mutex;
	std::condition_variable _cv;
	std::thread* _refresh;
	std::vector<std::shared_ptr<Object>> _obj;
};

