#include "diary.h"

#include <time.h>
#include <ncurses.h>


#define COLNUM_MONTH     20
#define COLNUM_COL_GAP   2
#define COLNUM_SEASON   (3 * COLNUM_MONTH + 2 * COLNUM_COL_GAP)
#define YEAR_LINE       3

typedef int(*action)(int key);
static action *actions;
static void load_actions();
static void free_actions();

enum Color_ { FOCUS, EXIST, NONE };
enum Season { Spring, Summer, Autumn, Winter };

static const char * WEEK_en[] = { " Su", " Mo", " Tu", " We", " Th", " Fr", " Sa", NULL };


__attribute__((unused)) static const char * WEEK_cn[] = { "日", "一", "二", "三", "四", "五", "六", NULL };



static void draw();
static void year();
static void season(int s);
static void month(int y, int x, int m);
static void day(int y, int x, const struct tm*);
static int daily_exist(const struct tm *date);


static void initcrt();
static void exitcrt();


void view() {
    initcrt();

    draw();

    int ch = 0;
    do {
        ch = getch();
        if (ch == ERR) {
            usleep(50 * 1000);
            continue;
        }

        if (ch == 'q') {
            break;
        }

        if (actions && actions[ch]) actions[ch](ch);

    } while (1);

    exitcrt();
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
    init_pair(EXIST, COLOR_BLACK, COLOR_GREEN);
    init_pair(NONE, COLOR_WHITE, COLOR_BLACK);

    load_actions();
}

void exitcrt() {
    free_actions();
    endwin();
}

void draw() {
    clear();
    year();
    season(Spring);
    season(Summer);
    season(Autumn);
    season(Winter);
    refresh();
}

void year() {
    char line[COLNUM_SEASON];
    strftime(line, COLNUM_SEASON, "%Y", &date);
    attron(A_BOLD);
    int x = (COLS - COLNUM_SEASON) / 2;
    int xx = (COLNUM_SEASON - strlen(line)) / 2;
    mvaddstr(YEAR_LINE, x + xx, line);
    attroff(A_BOLD);
}

static void season(int s) {
    int y = YEAR_LINE + 2 + s * 10;
    int x = (COLS - COLNUM_SEASON) / 2;
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
    y++;

    tmp.tm_mday = 1;
    time_t ttime = mktime(&tmp);

    struct tm mtm;
    int yy = 0;
    for (int d = 0; d < 31; d++, ttime += 86400) {
        gmtime_r(&ttime, &mtm);
        if (mtm.tm_mon != m) break;
        day(y + yy, x + 3 * (mtm.tm_wday), &mtm);
        if (mtm.tm_wday == 6 /* Sunday */) yy++;
    }
}

void day(int y, int x, const struct tm *tm) {
    char mday[4];
    strftime(mday, 4, " %e", tm);

    if (daily_exist(tm)) {
        attron(COLOR_PAIR(EXIST));
        mvaddstr(y, x, mday);
        attroff(COLOR_PAIR(EXIST));
    } else {
        attron(COLOR_PAIR(NONE));
        mvaddstr(y, x, mday);
        attroff(COLOR_PAIR(NONE));
    }
}


int daily_exist(const struct tm *d) {
    char filename[50];
    strftime(filename, 50, "./%Y/%m/%d.md", d);
    struct stat tmp;
    return stat(filename, &tmp) == 0;
}


static int redraw(int key) { draw(); return 0; }

static int last_year(int key) { 
    date.tm_year--;
    draw(); 
    return 0;
}

static int next_year(int key) { 
    date.tm_year++;
    draw();
    return 0;
}

void load_actions() {
    free_actions();
    actions = (action *)malloc(KEY_MAX * sizeof(action));
    if (actions == NULL) {
        return;
    }

    actions['n'] = next_year;
    actions['p'] = last_year;
    actions[KEY_RESIZE] = redraw;
}

void free_actions() { if (actions) free(actions); }
