#include "crt.h"
#include "option.h"

#include <stdlib.h>
#include <ncurses.h>

/* mark process enter curses mode */
static bool CRT_INITIALIZED = false;

int getcolorpair(Color c) {
    switch (c) {
        case RED: return COLOR_PAIR(RED);
        case GREEN: return COLOR_PAIR(GREEN);
        case CYAN: return COLOR_PAIR(CYAN);
        default: break;
    }
    return COLOR_PAIR(CUSTOM);
}

void quit(char* msg) {
    if (msg) printf("%s\n", msg);
    if (CRT_INITIALIZED) endwin();
    exit(0);
}

static void initcolor() {
    if (!has_colors()) return;
    start_color();
    init_pair(CUSTOM, COLOR_WHITE,  COLOR_BLACK);
    init_pair(RED, COLOR_RED,  COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN,  COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN,  COLOR_BLACK);
}

void initcrt() {
    initscr();
    noecho();
    nodelay(stdscr, true);
    curs_set(0); /* hide cursor */
    keypad(stdscr, true);
    initcolor();

    CRT_INITIALIZED = true;
}
