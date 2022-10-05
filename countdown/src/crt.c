#include "crt.h"
#include "option.h"

#include <stdlib.h>
#include <curses.h>

/* mark process enter curses mode */
static bool CRT_INITIALIZED = false;

#define colorpair_num(t, c) ( t * COLORSIZE + c )
const int COLOR_THEME[][COLORSIZE] = {
    {
        COLOR_PAIR(colorpair_num(T_TEXT, CUSTOM)),
        COLOR_PAIR(colorpair_num(T_TEXT, RED)),
        COLOR_PAIR(colorpair_num(T_TEXT, GREEN)),
        COLOR_PAIR(colorpair_num(T_TEXT, CYAN)),
        COLOR_PAIR(colorpair_num(T_TEXT, BLUE)),
    },
    {
        COLOR_PAIR(colorpair_num(T_BIG_TEXT, CUSTOM)),
        COLOR_PAIR(colorpair_num(T_BIG_TEXT, RED)),
        COLOR_PAIR(colorpair_num(T_BIG_TEXT, GREEN)),
        COLOR_PAIR(colorpair_num(T_BIG_TEXT, CYAN)),
        COLOR_PAIR(colorpair_num(T_BIG_TEXT, BLUE)),
    }
};

void quit(char* msg) {
    if (msg) printf("%s\n", msg);
    if (CRT_INITIALIZED) endwin();
    exit(0);
}

static void initcolor() {
    if (!has_colors()) return;
    start_color();

    init_pair(colorpair_num(T_TEXT, CUSTOM), COLOR_WHITE,  COLOR_BLACK);
    init_pair(colorpair_num(T_TEXT, RED), COLOR_RED,  COLOR_BLACK);
    init_pair(colorpair_num(T_TEXT, GREEN), COLOR_GREEN,  COLOR_BLACK);
    init_pair(colorpair_num(T_TEXT, CYAN), COLOR_CYAN,  COLOR_BLACK);
    init_pair(colorpair_num(T_TEXT, BLUE), COLOR_BLUE,  COLOR_BLACK);

    init_pair(colorpair_num(T_BIG_TEXT, CUSTOM), COLOR_WHITE, COLOR_WHITE);
    init_pair(colorpair_num(T_BIG_TEXT, RED), COLOR_RED,  COLOR_RED);
    init_pair(colorpair_num(T_BIG_TEXT, GREEN), COLOR_GREEN,  COLOR_GREEN);
    init_pair(colorpair_num(T_BIG_TEXT, CYAN), COLOR_CYAN,  COLOR_CYAN);
    init_pair(colorpair_num(T_BIG_TEXT, BLUE), COLOR_BLUE,  COLOR_BLUE);
}

void initcrt() {
    initscr();
    noecho();
    nonl();
    nodelay(stdscr, true);
    curs_set(0); /* hide cursor */
    keypad(stdscr, true);
    initcolor();

    CRT_INITIALIZED = true;
}
