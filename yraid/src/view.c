#include "diary.h"

#include <time.h>
#include <ncurses.h>
#include <locale.h>


#define COLNUM_MONTH     20                                     // width of month
#define YEAR_TITLE_COL_GAP   2                                  // width of gap between year title and spring
#define MONTH_COL_GAP   2                                       // width of gap between month
#define COLNUM_SEASON   (3 * COLNUM_MONTH + 2 * MONTH_COL_GAP)  // width of season
#define YEAR_LINE       3                                       // start linenum of year title
#define HEIGTH_MONTH    10                                      // heigth of month

typedef struct Position_ Position;
struct Position_ {
    int y;
    int x;
} focus = {
    0, 0
};

typedef int(*action)(int key);
static action *actions;
static void load_actions();
static void free_actions();

enum Color_ { FOCUS = 1, EXIST, NONE };
enum Season { Spring, Summer, Autumn, Winter };

static int CHINESE = 0; /* control show in english or chinese, default is english */
static const char *WEEK_en[] = { "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa", NULL };
static const char *WEEK_cn[] = { "日", "一", "二", "三", "四", "五", "六", NULL };

static void draw();
static void year();
static void season(int s);
static void month(int y, int x, int m);
static void day(int y, int x, const struct tm*);
static int daily_exist(const struct tm *date);
static int is_today(const struct tm *date);
static void locate();
static void init_focus();
static int is_focus(int y, int x);
static int isLeapYear(int year);
static int get_max_dayofmonth(const struct tm *date);

static struct tm getDate(int year, int month, int week, int sort); 


static void initcrt();
static void exitcrt();


void view() {
    setlocale(LC_ALL, "");
    initcrt();

    init_focus();

    draw();

    int ch = 0;
    do {
        ch = getch();
        if (ch == ERR) {
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

#if NCURSES_MOUSE_VERSION > 1
      mousemask(BUTTON1_RELEASED | BUTTON4_PRESSED | BUTTON5_PRESSED, NULL);
#else
      mousemask(BUTTON1_RELEASED, NULL);
#endif

    init_pair(FOCUS, COLOR_RED, COLOR_WHITE);
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

void init_focus() { locate(); }

void year() {
    char line[COLNUM_SEASON];
    int size = 0;
    size += strftime(line+size, COLNUM_SEASON-size, "%Y", &date);
    size += snprintf(line+size, COLNUM_SEASON-size, "%s", zodiac());
    size += snprintf(line+size, COLNUM_SEASON-size, " %d", AGE);
    attron(A_BOLD);
    int x = (COLS - COLNUM_SEASON) / 2;
    int xx = (COLNUM_SEASON - strlen(line)) / 2;
    mvprintw(YEAR_LINE, x + xx, line);
    attroff(A_BOLD);
}

void season(int s) {
    int y = YEAR_LINE + YEAR_TITLE_COL_GAP + s * HEIGTH_MONTH;
    int x = (COLS - COLNUM_SEASON) / 2;
    int m = s * 3;
    month(y, x, m); x += COLNUM_MONTH + MONTH_COL_GAP; m++;
    month(y, x, m); x += COLNUM_MONTH + MONTH_COL_GAP; m++;
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
        if (mtm.tm_wday == 6 /* end of week */) yy++;
    }
}

void day(int y, int x, const struct tm *tm) {
    mvaddstr(y, x++, " ");
    char mday[3];
    strftime(mday, 3, "%e", tm);

    if (is_today(tm)) attron(A_UNDERLINE);

    if (is_focus(y, x)) {
        attron(COLOR_PAIR(FOCUS));
        mvaddstr(y, x, mday);
        attroff(COLOR_PAIR(FOCUS));

    } else if (daily_exist(tm)) {
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

void locate() {
    int season = date.tm_mon / 3;
    focus.y = YEAR_LINE +  YEAR_TITLE_COL_GAP + season * HEIGTH_MONTH + 2/* month & weekname */;
    focus.x = (COLS - COLNUM_SEASON) / 2 + (date.tm_mon % 3) * (COLNUM_MONTH + MONTH_COL_GAP);
    focus.x += (date.tm_wday % 7) * 3;

    int mday = date.tm_mday;
    while (mday > 7) {
        focus.y++;
        mday = mday - 7;
    }
    if (mday > date.tm_wday+1) focus.y++;
}

int is_focus(int y, int x) {
    if (focus.y != y) return 0;
    return (x >= focus.x && x < focus.x + 3);
}

int get_max_dayofmonth(const struct tm *d) {
    static const int MAX_DAY_IN_LEAP_YEAR[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    static const int MAX_DAY_IN_YEAR[]      = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (isLeapYear(d->tm_year + 1900)) {
        return MAX_DAY_IN_LEAP_YEAR[d->tm_mon];
    }
    return MAX_DAY_IN_YEAR[d->tm_mon];
}

// 判断是否为闰年
int isLeapYear(int year) {
    if (year % 4 == 0 && year % 100 != 0) {
        return 1;
    } 
    return year % 400 == 0;
}

// 计算某年某月第一天是星期几
int getFirstDayOfWeek(int y, int m) {
    /* 基姆拉尔森计算公式
    W= (d+2m+3(m+1)/5+y+y/4-y/100+y/400+1) mod 7 */
    m++; /* start from one */
    if (m == 1 /* Jan */ || m == 2 /* Feb */) {
        y--;
        m += 12;
    }
    return (1 + 2 * m + 3 * (m+1) / 5 + y + y/4 - y/100 + y/400 +1) % 7;
}

struct tm getDate(int year, int month, int wday, int first) {
    int wdayOf1stDay = getFirstDayOfWeek(year, month);
    struct tm tmp = date;

    tmp.tm_year = year - 1900;
    tmp.tm_mon  = month;
    tmp.tm_mday = 1 + ((wday - wdayOf1stDay + 7) % 7);
    if (!first) {
        while (tmp.tm_mday + 7 <= get_max_dayofmonth(&tmp)) {
            tmp.tm_mday += 7;
        }
    }
    tmp.tm_wday = wday;
    return tmp;
} 



/*  ---------------------------  op related ---------------- */

static int redraw(int key) { return 0; }

static int last_year(int key) { 
    date.tm_year--;
    locate();
    return 0;
}

static int next_year(int key) { 
    date.tm_year++;
    locate();
    return 0;
}

static int today(int key) {
    time_t now = time(0);
    localtime_r(&now, &date);
    locate();
    return 0;
}

static int lang(int key) {
    CHINESE = ~CHINESE;
    return 0;
}

static int up(int key) {
    if (date.tm_mday > 7) {
        date.tm_mday -= 7;
    } else {
        int year = date.tm_year+1900;
        if (date.tm_mon < 3) year--;
        int month   = (date.tm_mon + 9) % 12;
        date = getDate(year, month, date.tm_wday, 0);
    }
    locate();
    return 0;
}

static int down(int key) {
    if (date.tm_mday + 7 <= get_max_dayofmonth(&date)) {
        date.tm_mday += 7;
    } else {
        int year = date.tm_year+1900;
        if (date.tm_mon >= 9) year += 1;
        int month = (date.tm_mon + 3) % 12;
        date = getDate(year, month, date.tm_wday, 1);
    }
    locate();
    return 0;
}

static int last_day(int key) {
    if (date.tm_mday > 1) {
        date.tm_mday--;
        date.tm_wday = (date.tm_wday + 6) % 7;
    } else {
        int year = date.tm_year+1900;
        if (date.tm_mon == 0) year--;
        int month = (date.tm_mon + 11) % 12;
        int wday  = (date.tm_wday + 6) % 7;
        date = getDate(year, month, wday, 0);
    }
    locate();
    return 0;
}

static int next_day(int key) {
     if (date.tm_mday < get_max_dayofmonth(&date)) {
        date.tm_mday++;
        date.tm_wday = (date.tm_wday + 1) % 7;
    } else {
        int year = date.tm_year+1900;
        if (date.tm_mon == 11) year++;
        int month = (date.tm_mon + 1) % 12;
        int wday  = (date.tm_wday + 1) % 7;
        date = getDate(year, month, wday, 1);
    }
    locate();
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
    actions[KEY_UP]     = up;
    actions[KEY_DOWN]   = down;
    actions[KEY_LEFT]   = last_day;
    actions[KEY_RIGHT]  = next_day;
    actions[KEY_RESIZE] = redraw;
}

void free_actions() { if (actions) free(actions); }
