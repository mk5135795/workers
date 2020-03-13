#pragma once

#include <mutex>
#include <memory>
#include <vector>

#include "Object.h"
#include "Sensor.h"

class Table
{
public:
	Table(std::weak_ptr<Object> row, int n);

	int Get(int id, int& quantity, int weight);
	void Put(int id, int quantity);
	Sensor* GetSensor();
	void Update();

private:
	std::vector<int> _storage;
	std::mutex* _mutex;
	std::weak_ptr<Object> _obj_row;
	std::unique_ptr<Sensor> _sen;
};

