#pragma once

#include <mutex>
#include <vector>

#include <ncursesw/curses.h>

class Object
{
public:
	enum Color
	{
		error = 1,
		normal,
		work,
		rest,
		collect,
		fired,
		info,
		hidden
	};

	struct text {
		int x;
		int y;
		int color;
		std::wstring wstr;
		bool modyfied;
	};

	Object(int y, int x, std::wstring wstr);
	Object(std::vector<text>&& vec);
	void SetString(std::wstring wstr, int color, int i = 0);
	void Draw(WINDOW* win);

private:
	bool _modyfied;
	std::mutex _mut;
	std::vector<text> _vec;
};

