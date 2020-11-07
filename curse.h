#pragma once

#include <ncurses.h>

struct WindowPosition {
	int x;
	int y;
};

struct WindowSize {
	int width;
	int height;
};

class CursedWindow {
public:
	CursedWindow(const WindowPosition& position, const WindowSize& size) {
		// std::cout << "INFO: window_=" << window_ << '\n';
		window_ = newwin(size.height, size.width, position.y, position.x);
		wrefresh(window_);
	}
	CursedWindow(const CursedWindow&) = delete;
	CursedWindow(CursedWindow&& window) {
		destroy();
		std::swap(window_, window.window_);
	}

	~CursedWindow() {
		destroy();
	}

	void update(const std::string& content) {
		werase(window_);
		// box(window_, 0, 0);
		wprintw(window_, content.c_str());
		wrefresh(window_);
	}

	void destroy() {
		// wborder(window_, ' ', ' ', ' ',' ',' ',' ',' ',' ');
		// wrefresh(window_);
		delwin(window_);
		window_ = nullptr;
	}

private:
	WINDOW * window_ = nullptr;
};

class MainCurse {
public:

	MainCurse() {
		initscr();
		cbreak();
		// TODO: raw(); ?
		printw("Blabla, hi!");
		refresh();
		// getch();
	}

	MainCurse(const MainCurse&) = delete;
	MainCurse(MainCurse&&) = delete;

	~MainCurse() {
		endwin();
	}

	void update() {
		refresh();
	}

	// void add_window(CursedWindow&& window) {
		// windows_.emplace_back(std::move(window));
	// }
	void wait_for_any() {
		getch();
	}

private:
	// std::vector<CursedWindow> windows_;

};
