#pragma once

#include <vector>

class Sensor
{
public:
	enum class Info : bool
	{
		less,
		more
	};

	Sensor(int n);
	void Report(Sensor::Info type, int id, int weight = 0);
	int GetRaport(std::vector<int>& raport);
private:
	std::vector<int> _drop_rate;
	int _drop_sum;
};

