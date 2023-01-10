#include "diary.h"

#include <ncurses.h>

enum Color_ { FOCUS, EXIST, NONE };

static void initcrt() {
    initscr();
    start_color();
    noecho();
    nonl();
    nodelay(stdscr, true);
    curs_set(0);
	cbreak();
    keypad(stdscr, true);

    init_pair(FOCUS, COLOR_GREEN, COLOR_BLACK);
    init_pair(EXIST, COLOR_BLACK, COLOR_WHITE);
    init_pair(NONE, COLOR_WHITE, COLOR_BLACK);
}



void view() {
    initcrt();

    clear();
    const int COLNUM = 62;
    char line[COLNUM];
    int l = 3;
    strftime(line, COLNUM, "%Y", &date);
    attron(A_BOLD); mvaddstr(l++, (COLS - COLNUM)/2, line); attroff(A_BOLD);

    refresh();

    int ch = 0;
    do {
        if ((ch = getch()) == 'q') {
            break;
        }

        usleep(50 * 1000);
    } while (1);

    endwin();
    exit(0);
}
