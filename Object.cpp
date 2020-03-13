#include "Object.h"

#include <string>

Object::Object(int y, int x, std::wstring wstr) {
    _vec.emplace_back(text{x, y, Color::normal, wstr, true});
}

Object::Object(std::vector<text>&& vec)
    : _vec(vec)
{}

void Object::SetString(std::wstring wstr, int color, int i)
{
	std::lock_guard<std::mutex> lk(_mut);
	_vec[i].wstr = wstr;
	_vec[i].color = color;
	_vec[i].modyfied = true;
}

void Object::Draw(WINDOW* win)
{
	std::lock_guard<std::mutex> lk(_mut);
	for(text t : _vec)
    {
        if(t.modyfied)
        {
            wattron(win, COLOR_PAIR(t.color));
            mvwaddwstr(win, t.y, t.x, t.wstr.c_str());
            wattroff(win, COLOR_PAIR(t.color));
            t.modyfied = false;
        }
    }
}
