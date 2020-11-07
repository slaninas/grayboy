#pragma once

#include <ncurses.h>

class MainCurse {
public:

	MainCurse() {
		initscr();
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
