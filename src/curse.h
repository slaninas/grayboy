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

// TODO: Hide CursedWindow inside MainCurse, add method to MainCurse for making CursedWindow
class CursedWindow {
public:
	CursedWindow(const WindowPosition& position, const WindowSize& size) {
		window_ = newwin(size.height - 2, size.width - 2, position.y + 1, position.x + 1);
		wrefresh(window_);

		border_window_ = newwin(size.height, size.width, position.y, position.x);
		box(border_window_, 0, 0);
		wrefresh(border_window_);
	}
	CursedWindow(const CursedWindow&) = delete;
	CursedWindow(CursedWindow&& window) noexcept {
		destroy();
		std::swap(window_, window.window_);
	}

	~CursedWindow() {
		destroy();
	}

	void update(const std::string& content) {
		box(border_window_, 0, 0);
		wrefresh(border_window_);

		werase(window_);
		wprintw(window_, content.c_str());
		wrefresh(window_);
	}

	void add(const std::string& str) {
		wprintw(window_, str.c_str());
		wrefresh(window_);
	}

	void destroy() {
		delwin(window_);
		window_ = nullptr;
	}

private:
	WINDOW* window_ = nullptr;
	WINDOW* border_window_ = nullptr;
};

class MainCurse {
public:
	MainCurse() {
		initscr();
		cbreak();
		// TODO: raw(); ?
	}

	MainCurse(const MainCurse&) = delete;
	MainCurse(MainCurse&&) = delete;

	~MainCurse() {
		endwin();
	}

	void update() {
		refresh();
	}

	void add(const std::string& str) {
		printw(str.c_str());
	}

	void wait_for_any() {
		getch();
	}

	auto get_char() {
		return getch();
	}

private:
	// std::vector<CursedWindow> windows_;
};