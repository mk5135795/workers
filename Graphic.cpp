#include "Graphic.h"

Graphic::Graphic()
	: _run(false), _refresh(nullptr)
{
	setlocale(LC_ALL, "");
	initscr();
	cbreak();
	start_color();
	keypad(stdscr, true);
	init_pair(Object::Color::error,   COLOR_RED,     COLOR_BLACK);
	init_pair(Object::Color::normal,  COLOR_WHITE,   COLOR_BLACK);
	init_pair(Object::Color::work,    COLOR_CYAN,    COLOR_BLACK);
	init_pair(Object::Color::rest,    COLOR_GREEN,   COLOR_BLACK);
	init_pair(Object::Color::collect, COLOR_YELLOW,  COLOR_BLACK);
	init_pair(Object::Color::fired,   COLOR_MAGENTA, COLOR_BLACK);
	init_pair(Object::Color::info,    COLOR_BLUE,    COLOR_BLACK);
	init_pair(Object::Color::hidden,  COLOR_BLACK,   COLOR_BLACK);
	refresh();
    _win = newwin(24, 80, 1, 0);
}

Graphic::~Graphic() {
	Stop();
}

std::shared_ptr<Object> Graphic::CreateObject(int y, int x) {
	_obj.emplace_back(std::make_shared<Object>(y, x, L""));
	return _obj.back();
}

std::shared_ptr<Object> Graphic::CreateObject(std::vector<Object::text>&& vec) {
	_obj.emplace_back(std::make_shared<Object>(std::move(vec)));
	return _obj.back();
}

void Graphic::Start(int threads_n, int items_n, std::function<void()> update) {
	clear();
	noecho();
	curs_set(0);
	Gen(threads_n, items_n);

	_run = true;
	_refresh = new std::thread([this, &update] {
			std::unique_lock<std::mutex> lk(_mutex);
			while (!_cv.wait_for(lk, std::chrono::milliseconds(30), [this] { return !_run; })) {
				update();
				this->Refresh();
			}
		});
}

void Graphic::Stop() {
	_run = false;
	_cv.notify_one();
	if (_refresh->joinable())
		_refresh->join();
    delete _refresh;

    delwin(_win);
    endwin();
}

void Graphic::Gen(int threads_n, int items_n) {
    mvwaddwstr(_win, 0, 28, L"|| \\      ##.#      / ||");
    mvwaddwstr(_win, 1, 28, L"\\/  '--------------'  \\/");
    mvwaddwstr(_win, 2, 27, L"┌────────────────────────┐");
    mvwaddwstr(_win, 3, 27, L"│                        │");
    mvwaddwstr(_win, 4, 27, L"│                        │");
    mvwaddwstr(_win, 5, 27, L"│                        │");
    mvwaddwstr(_win, 6, 27, L"│                        │");
    mvwaddwstr(_win, 7, 27, L"└────────────────────────┘");

    for(int i(0), w(0), h(0); i < items_n; i++, w++)
    {
        if(w == 5)
        {
            w = 0;
            h++;
        }
        wattron(_win, COLOR_PAIR(Object::Color::info));
        mvwaddwstr(_win, 4 + h*2, 28 + w*5, L"+");
        wattroff(_win, COLOR_PAIR(Object::Color::info));
    }
    for(int i(0), w(0), h(0); i < threads_n; i++, w++)
    {
        if(h < 2 && w == 2)
            w = 4;
        else if(w == 6)
        {
            w = 0;
            h++;
        }
        if(items_n > 5)
        {
            mvwaddwstr(_win, 1 + h*4, 2 + w*13, L"⎡         ⎤");
            mvwaddwstr(_win, 2 + h*4, 2 + w*13, L"⎣         ⎦");
        }
        else
            mvwaddwstr(_win, 1 + h*4, 2 + w*13, L"[         ]");
    }
}

void Graphic::Refresh() {
	for (auto obj : _obj)
        obj->Draw(_win);
    wrefresh(_win);
}
