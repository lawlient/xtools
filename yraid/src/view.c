#include "yraid.h"

#include <time.h>
#include <ncurses.h>
#include <locale.h>

#define ENTER   13
#define ESC     27

#define COLNUM_MONTH     20                                     // width of month
#define YEAR_TITLE_COL_GAP   2                                  // width of gap between year title
#define MONTH_COL_GAP   2                                       // width of gap between month
#define COLNUM_QUARTER (3 * COLNUM_MONTH + 2 * MONTH_COL_GAP)   // width of quarter
#define YEAR_LINE 3                                             // start linenum of year title
#define HEIGTH_MONTH 10                                         // heigth of month

#define PAGE_MOVE_LINES 15
#define PAGE_FOOT_GAP 5

typedef struct Position_ Position;
struct Position_ {
    int y;
    int x;
} focus = {
    0, 0
};

typedef int (*action)(int key);
static action *actions;
static void load_actions();
static void free_actions();

enum Color_ {
    FOCUS = 1,
    EXIST,
    NONE
};

static int CHINESE = 0; /* control show in english or chinese, default is english */
static const char *WEEK_en[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa", NULL};
static const char *WEEK_cn[] = {"日", "一", "二", "三", "四", "五", "六", NULL};

static void draw();
static void year();
static void quarter(int s);
static void month(int y, int x, int m);
static void day(int y, int x, const struct tm *);
static int daily_exist(const struct tm *date);
static int is_today(const struct tm *date);
static void locate();
static int is_focus(int y, int x);


static void initcrt();
static void exitcrt();

void view() {
    parse_config();
    /* change work directory if repository is setted */
    const char* repository = get_config(CFG_REPOSITORY);
    if (repository) chdir(repository);

    setlocale(LC_ALL, "");
    initcrt();

    locate();

    draw();

    int ch = 0;
    do {
        ch = getch();
        if (ch == ERR) {
            usleep(200 * 1000); // low cpu
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
    erase(); // screen blink when I use `clear()`
    year();
    quarter(0);
    quarter(1);
    quarter(2);
    quarter(3);
    refresh();
}


void year() {
    char line[COLNUM_QUARTER];
    int size = 0;
    size += snprintf(line+size, COLNUM_QUARTER-size, "%s ", zodiac());
    size += strftime(line+size, COLNUM_QUARTER-size, " %Y ", &date);
    size += snprintf(line+size, COLNUM_QUARTER-size, "%s", age());
    attron(A_BOLD);
    int x = (COLS - COLNUM_QUARTER) / 2;
    int xx = (COLNUM_QUARTER - strlen(line)) / 2;
    mvprintw(YEAR_LINE, x + xx, "%s", line);
    attroff(A_BOLD);
}

void quarter(int s) {
    int y = YEAR_LINE + YEAR_TITLE_COL_GAP + s * HEIGTH_MONTH;
    int x = (COLS - COLNUM_QUARTER) / 2;
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
        mvprintw(y, xx, "%s", w);
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
    int quarter = date.tm_mon / 3;
    focus.y = YEAR_LINE +  YEAR_TITLE_COL_GAP + quarter * HEIGTH_MONTH + 2/* month & weekname */;
    focus.x = (COLS - COLNUM_QUARTER) / 2 + (date.tm_mon % 3) * (COLNUM_MONTH + MONTH_COL_GAP);
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
        getDate(&date, year, month, date.tm_wday, 0);
    }
    locate();
    return 0;
}

static int down(int key) {
    if (date.tm_mday + 7 <= ndayofmonth(&date)) {
        date.tm_mday += 7;
    } else {
        int year = date.tm_year+1900;
        if (date.tm_mon >= 9) year += 1;
        int month = (date.tm_mon + 3) % 12;
        getDate(&date, year, month, date.tm_wday, 1);
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
        getDate(&date, year, month, wday, 0);
    }
    locate();
    return 0;
}

static int next_day(int key) {
     if (date.tm_mday < ndayofmonth(&date)) {
        date.tm_mday++;
        date.tm_wday = (date.tm_wday + 1) % 7;
    } else {
        int year = date.tm_year+1900;
        if (date.tm_mon == 11) year++;
        int month = (date.tm_mon + 1) % 12;
        int wday  = (date.tm_wday + 1) % 7;
        getDate(&date, year, month, wday, 1);
    }
    locate();
    return 0;
}

static int preview(int key) {
    if (!daily_exist(&date)) return 0;

    char filename[BUFSIZ];
    int size = strftime(filename, BUFSIZ, "./%Y/%m/%d.md", &date);
    if (0 == size) return 0;

    int fd = open(filename, O_RDONLY);
    if (fd <= 0) return 0;

    /* FIX: file size is large than 2 pages, content will be overflow */
    size_t len = 2 * getpagesize();
    char* map = mmap(0, len, PROT_READ, MAP_PRIVATE, fd, 0);
    if (!map) return 0;

    int nline = 0;
    for (int i = 0; map[i]; i++) {
        if ('\n' == map[i]) {
            nline++;
        }
    }


    int x = 0;
    int y = 0;
    int ln = 0;
    char line[BUFSIZ];

    while (1) {
        erase();
        y = 0;
        for (int i = 0; i < len && map[i]; i++) {
            if (map[i] == '\n') {
                mvprintw(y + ln, 0, "%s", line);
                x = 0;
                memset(line, 0, BUFSIZ);
                if (++y == LINES - 1 - ln) break;
                continue;
            }
            line[x++] = map[i];
        }
        mvprintw(LINES - 1, 0, "Escape with 'q'");
        refresh();


        int k;
        int quit = 0;
        int refresh = 0;
        do {
            k = getch();
            switch (k) {
                case ERR: usleep(200 * 1000); break;
                case 'q':
                case ESC: quit = 1; break;
                case KEY_DOWN:
                case 'j': {
                    if (ln + nline > LINES - 1 - PAGE_FOOT_GAP) {
                        ln--;
                        refresh = 1;
                    }
                    break;
                }
                case KEY_UP:
                case 'k': {
                    if (ln < 0) {
                        ln++;
                        refresh = 1;
                    }
                    break;
                }
                case KEY_PPAGE:
                case 'u': {
                    if (ln < 0) {
                        ln += PAGE_MOVE_LINES;
                        if (ln > 0) ln = 0;
                        refresh = 1;
                    }
                    break;
                }
                case KEY_NPAGE:
                case 'd': {
                    if (ln + nline > LINES - 1 - PAGE_FOOT_GAP) {
                        ln -= PAGE_MOVE_LINES;
                        if (ln + nline < LINES - 1 - PAGE_FOOT_GAP)
                            ln = LINES - 1 - PAGE_FOOT_GAP - nline;
                        refresh = 1;
                    }
                    break;
                }
                case KEY_RESIZE: refresh = 1; break;
                default: break;
            } 
            if (refresh || quit) break;
        } while (1);

        if (quit) break;
    }
    
    munmap(map, len);
    close(fd);
    return 0;
}



void load_actions() {
    free_actions();
    actions = (action *)malloc(KEY_MAX * sizeof(action));
    if (actions == NULL) {
        return;
    }

    actions['n']        = next_year;
    actions['p']        = last_year;
    actions['0']        = today;
    actions['l']        = lang;
    actions[KEY_UP]     = up;
    actions[KEY_DOWN]   = down;
    actions[KEY_LEFT]   = last_day;
    actions[KEY_RIGHT]  = next_day;
    actions[ENTER]      = preview;
    actions[KEY_RESIZE] = redraw;
}

void free_actions() { if (actions) free(actions); }
