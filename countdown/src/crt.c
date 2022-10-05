#include "crt.h"
#include "option.h"

#include <stdlib.h>
#include <curses.h>

/* mark process enter curses mode */
static bool CRT_INITIALIZED = false;

#define colorpair_num(t, c) ( t * COLORSIZE + c )
const int COLOR_THEME[][COLORSIZE] = {
    {
        COLOR_PAIR(colorpair_num(T_DEFAULT, CUSTOM)),
        COLOR_PAIR(colorpair_num(T_DEFAULT, RED)),
        COLOR_PAIR(colorpair_num(T_DEFAULT, GREEN)),
        COLOR_PAIR(colorpair_num(T_DEFAULT, CYAN)),
        COLOR_PAIR(colorpair_num(T_DEFAULT, BLUE)),
    },
    {
        COLOR_PAIR(colorpair_num(T_BG, CUSTOM)),
        COLOR_PAIR(colorpair_num(T_BG, RED)),
        COLOR_PAIR(colorpair_num(T_BG, GREEN)),
        COLOR_PAIR(colorpair_num(T_BG, CYAN)),
        COLOR_PAIR(colorpair_num(T_BG, BLUE)),
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

    init_pair(colorpair_num(T_DEFAULT, CUSTOM), COLOR_WHITE,  COLOR_BLACK);
    init_pair(colorpair_num(T_DEFAULT, RED), COLOR_RED,  COLOR_BLACK);
    init_pair(colorpair_num(T_DEFAULT, GREEN), COLOR_GREEN,  COLOR_BLACK);
    init_pair(colorpair_num(T_DEFAULT, CYAN), COLOR_CYAN,  COLOR_BLACK);
    init_pair(colorpair_num(T_DEFAULT, BLUE), COLOR_BLUE,  COLOR_BLACK);

    init_pair(colorpair_num(T_BG, CUSTOM), COLOR_WHITE, COLOR_WHITE);
    init_pair(colorpair_num(T_BG, RED), COLOR_RED,  COLOR_RED);
    init_pair(colorpair_num(T_BG, GREEN), COLOR_GREEN,  COLOR_GREEN);
    init_pair(colorpair_num(T_BG, CYAN), COLOR_CYAN,  COLOR_CYAN);
    init_pair(colorpair_num(T_BG, BLUE), COLOR_BLUE,  COLOR_BLUE);
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
