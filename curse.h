#pragma once

#include <ncurses.h>

class MainCurse {
public:

	MainCurse() {
		initscr();
		// TODO: raw(); ?
		printw("Blabla, hi!");
		refresh();
		getch();
	}

	MainCurse(const MainCurse&) = delete;
	MainCurse(MainCurse&&) = delete;

	~MainCurse() {
		endwin();
	}

private:

};
