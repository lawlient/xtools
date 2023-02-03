#include "diary.h"

#include <time.h>
#include <ncurses.h>
#include <locale.h>


#define COLNUM_MONTH     20
#define COLNUM_COL_GAP   2
#define COLNUM_SEASON   (3 * COLNUM_MONTH + 2 * COLNUM_COL_GAP)
#define YEAR_LINE       3
#define MY_BIRTH_YEAR   1992
#define AGE             (date.tm_year + 1900 - MY_BIRTH_YEAR)

typedef int(*action)(int key);
static action *actions;
static void load_actions();
static void free_actions();

enum Color_ { FOCUS, EXIST, NONE };
enum Season { Spring, Summer, Autumn, Winter };

static int CHINESE = 0; /* control show in english or chinese, default is english */
static const char *WEEK_en[] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa", NULL };
static const char *WEEK_cn[] = { "日", "一", "二", "三", "四", "五", "六", NULL };
static const char *ZODIAC[] = { "🐭", "🐮", "🐯", "🐰", "🐲", "🐍", "🐴", "🐏", "🐵", "🐔", "🐶", "🐷", NULL};

static void draw();
static void year();
static void season(int s);
static void month(int y, int x, int m);
static void day(int y, int x, const struct tm*);
static int daily_exist(const struct tm *date);
static int is_today(const struct tm *date);
static const char *zodiac();


static void initcrt();
static void exitcrt();


void view() {
    setlocale(LC_ALL, "");
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

        if (actions && actions[ch]) {
            actions[ch](ch);
            draw();
        }

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

    init_pair(FOCUS, COLOR_RED, COLOR_BLACK);
    init_pair(EXIST, COLOR_GREEN, COLOR_BLACK);
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
    int size = 0;
    size += strftime(line, COLNUM_SEASON, "%Y", &date);
    size += snprintf(line+size, COLNUM_SEASON-size, "%s", zodiac());
    size += snprintf(line+size, COLNUM_SEASON-size, " %d", AGE);
    attron(A_BOLD);
    int x = (COLS - COLNUM_SEASON) / 2;
    int xx = (COLNUM_SEASON - strlen(line)) / 2;
    mvprintw(YEAR_LINE, x + xx, line);
    attroff(A_BOLD);
}

void season(int s) {
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
    int xx = x + 1/* gap */;
    for (int d = 0; d < 7; d++) {
        const char* w = CHINESE ? WEEK_cn[d] : WEEK_en[d];
        mvprintw(y, xx, w);
        xx += strlen(w) + (CHINESE ? 0 : 1)/* gap */;
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
    mvaddstr(y, x++, " ");
    char mday[3];
    strftime(mday, 3, "%e", tm);

    if (is_today(tm)) attron(A_UNDERLINE);

    if (daily_exist(tm)) {
        attron(COLOR_PAIR(EXIST));
        mvaddstr(y, x, mday);
        attroff(COLOR_PAIR(EXIST));
    } else {
        attron(COLOR_PAIR(NONE));
        mvaddstr(y, x, mday);
        attroff(COLOR_PAIR(NONE));
    }
    if (is_today(tm)) attroff(A_UNDERLINE);
}


int daily_exist(const struct tm *d) {
    char filename[50];
    strftime(filename, 50, "./%Y/%m/%d.md", d);
    struct stat tmp;
    return stat(filename, &tmp) == 0;
}

int is_today(const struct tm *date) {
    time_t now = time(0);
    struct tm today;
    localtime_r(&now, &today);
    if (today.tm_year == date->tm_year &&
        today.tm_mon == date->tm_mon &&
        today.tm_mday == date->tm_mday) {
        return 1;
    }
    return 0;
}

const char *zodiac() {
    int idx = 4;
    int off = (date.tm_year + 1900 - 2012) % 12;
    idx = (idx + off) % 12;
    if (idx < 0) idx += 12;
    return ZODIAC[idx];
}

static int redraw(int key) { return 0; }

static int last_year(int key) { 
    date.tm_year--;
    return 0;
}

static int next_year(int key) { 
    date.tm_year++;
    return 0;
}

static int today(int key) {
    time_t now = time(0);
    localtime_r(&now, &date);
    return 0;
}

static int lang(int key) {
    CHINESE = ~CHINESE;
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
    actions['0'] = today;
    actions['l'] = lang;
    actions[KEY_RESIZE] = redraw;
}

void free_actions() { if (actions) free(actions); }
