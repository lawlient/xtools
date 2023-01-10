#include "diary.h"

#include <time.h>
#include <ncurses.h>

static const int COLNUM_MONTH = 20;
static const int COLNUM_COL_GAP = 2;
static const int COLNUM_SEASON = 3 * COLNUM_MONTH + 2 * COLNUM_COL_GAP;
static const int YEAR_LINE = 3;

enum Color_ { FOCUS, EXIST, NONE };

static const char * WEEK_en[] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa", NULL };
static const char * WEEK_cn[] = { "日", "一", "二", "三", "四", "五", "六", NULL };



static void year();
static void season(int s);
static void month(int y, int x, int m);
static void initcrt();
static int access_daily(const struct tm *date);






void view() {
    initcrt();

    clear();
    year();
    for (int s = 0; s < 4; s++) season(s);


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




void initcrt() {
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

void year() {
    char line[COLNUM_SEASON];
    strftime(line, COLNUM_SEASON, "%Y", &date);
    attron(A_BOLD);
    mvaddstr(YEAR_LINE, (COLS - COLNUM_SEASON)/2, line);
    attroff(A_BOLD);
}

static void season(int s) {
    int y = YEAR_LINE + 1 + s * 10;
    int x = 0;
    int m = s * 3;
    month(y, x, m); x += COLNUM_MONTH + COLNUM_COL_GAP; m++;
    month(y, x, m); x += COLNUM_MONTH + COLNUM_COL_GAP; m++;
    month(y, x, m);
}

void month(int y, int x, int m) {
    char line[COLNUM_MONTH];
    struct tm tmp = date;
    tmp.tm_mon = m;
    strftime(line, COLNUM_MONTH, "%B", &tmp);
    mvaddstr(y++, x + (COLNUM_MONTH - strlen(line)) / 2, line);
    int xx = x;
    for (int d = 0; d < 7; d++) {
        mvaddstr(y, xx, WEEK_en[d]);
        xx += 3;
    }


}
