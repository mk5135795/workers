#include "Sensor.h"
#include<numeric>

Sensor::Sensor(int n)
	: _drop_sum(n)
{
	_drop_rate.resize(n, 1);
}

void Sensor::Report(Sensor::Info type, int id, int weight) {
	if (type == Info::more)
	{
		_drop_sum += weight;
		_drop_rate[id] += weight;
	}
	else
	{
		_drop_sum -= _drop_rate[id];
		_drop_rate[id] = 0;
	}
}

int Sensor::GetRaport(std::vector<int>& raport) {
    int tmp_n(_drop_sum);

    _drop_sum = 0;
	_drop_rate.swap(raport);
	for(int i(_drop_rate.size() - 1); i >= 0; i--)
    {
        if(raport[i] == 0)
            _drop_rate[i] = 0;
        else
        {
            _drop_rate[i] = 1;
            _drop_sum++;
        }
    }
	return tmp_n;
}
