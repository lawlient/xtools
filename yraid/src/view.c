#include "yraid.h"

#include <stdlib.h>
#include <time.h>
#include <ncurses.h>
#include <locale.h>

#define ENTER   13
#define ESC     27

#define COLNUM_MONTH     20                                     // width of month (= 2 * 7 + 1 * 6)
#define YEAR_TITLE_COL_GAP   2                                  // width of gap between year title
#define MONTH_COL_GAP   2                                       // width of gap between month
#define COLNUM_QUARTER (3 * COLNUM_MONTH + 2 * MONTH_COL_GAP)   // width of quarter
#define YEAR_LINE 3                                             // start linenum of year title
#define HEIGTH_MONTH 10                                         // heigth of month

#define PAGE_MOVE_LINES 15
#define PAGE_FOOT_GAP 5

typedef enum {
    AC_OK       = 0,
    AC_REFRESH  = 0x1,
    AC_QUIT     = 0x2,
} ActionResult;
typedef ActionResult (*action)(int key);
static action *actions;
static void load_actions();
static void free_actions();

enum Color_ {
    DEFAULT,
    CURSOR,
    EXIST,
    HIGHLIGHT,
};

static int CHINESE = 0; /* control show in english or chinese, default is english */
static const char *WEEK_en = " Su Mo Tu We Th Fr Sa";
static const char *WEEK_cn = " 日 一 二 三 四 五 六";

static struct tm TODAY;
static void draw();
static void draw_year();
static void draw_month(int m);
static void draw_day(const struct tm *, int y, int x);
static int daily_exist(const struct tm *date);
static int is_today(const struct tm *date);
static int is_cursor_day(const struct tm* day);


static void initcrt();
static void exitcrt();

void view() {
    parse_config();
    /* change work directory if repository is setted */
    const char* repository = get_config(CFG_REPOSITORY);
    if (repository) chdir(repository);

    setlocale(LC_ALL, "");
    initcrt();

    draw();

    int ch = 0;
    do {
        ch = getch();
        if (ch == ERR) {
            usleep(200 * 1000); // low cpu
            continue;
        }

        if (actions && actions[ch]) {
            ActionResult ac = actions[ch](ch);
            if (ac & AC_QUIT) break;
            if (ac & AC_REFRESH) {
                draw();
            }
        }

    } while (1);

    exitcrt();
    exit(0);
}

void initcrt() {
    initscr();
    start_color();
    use_default_colors();
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

    init_pair(DEFAULT, -1, -1);
    init_pair(CURSOR, COLOR_WHITE, COLOR_CYAN);
    init_pair(EXIST, COLOR_GREEN, -1);
    init_pair(HIGHLIGHT, COLOR_CYAN, -1);

    load_actions();
}

void exitcrt() {
    free_actions();
    endwin();
}

void draw() {
    time_t now = time(0);
    localtime_r(&now, &TODAY);

    erase();

    draw_year();
    for (int m = 0; m < 12; m++)
      draw_month(m);

    refresh();
}


void draw_year() {
    char line[COLNUM_QUARTER];
    int size = 0;
    size += snprintf(line+size, COLNUM_QUARTER-size, "%s ", zodiac());
    size += strftime(line+size, COLNUM_QUARTER-size, " %Y ", &date);
    size += snprintf(line+size, COLNUM_QUARTER-size, "%s", age());
    attrset(COLOR_PAIR(HIGHLIGHT) | A_BOLD);
    int x = (COLS - COLNUM_QUARTER) / 2;
    int xx = (COLNUM_QUARTER - strlen(line)) / 2;
    mvprintw(YEAR_LINE, x + xx, "%s", line);
}

void draw_month(int m) {
    int y = YEAR_LINE + YEAR_TITLE_COL_GAP + (m / 3) * HEIGTH_MONTH;
    int x = (COLS - COLNUM_QUARTER) / 2 + (m % 3) * (COLNUM_MONTH + MONTH_COL_GAP);

    char line[COLNUM_MONTH];
    struct tm tmp = date;
    tmp.tm_mon = m;
    strftime(line, COLNUM_MONTH, "%B", &tmp);

    attrset(A_BOLD);
    mvaddstr(y++, x + (COLNUM_MONTH - strlen(line)) / 2 + 1, line);
    attrset(0);
    mvprintw(y++, x, "%s", CHINESE ? WEEK_cn : WEEK_en);

    int nday = ndayofmonth(tmp.tm_year + 1900, m);
    int wday = firstdayofmonth(tmp.tm_year + 1900, m);
    for (int d = 1; d <= nday; d++) {
        tmp.tm_mday = d;
        tmp.tm_wday = (d + wday - 1) % 7;
        if (tmp.tm_wday == 0) {
            y++;
        }

        draw_day(&tmp, y, x + 3 * tmp.tm_wday);
    }
}

void draw_day(const struct tm *tm, int y, int x) {
    mvaddstr(y, x++, " ");
    char mday[3];
    strftime(mday, 3, "%e", tm);

    if (is_today(tm)) attron(A_UNDERLINE);

    if (is_cursor_day(tm)) {
        attron(COLOR_PAIR(CURSOR));
        mvaddstr(y, x, mday);
        attroff(COLOR_PAIR(CURSOR));

    } else if (daily_exist(tm)) {
        attron(COLOR_PAIR(EXIST));
        mvaddstr(y, x, mday);
        attroff(COLOR_PAIR(EXIST));

    } else {
        attron(COLOR_PAIR(DEFAULT));
        mvaddstr(y, x, mday);
        attroff(COLOR_PAIR(DEFAULT));
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
    return (
        TODAY.tm_mday == date->tm_mday &&
        TODAY.tm_mon == date->tm_mon &&
        TODAY.tm_year == date->tm_year
    );
}

int is_cursor_day(const struct tm* day) {
    return (
        date.tm_mday == day->tm_mday &&
        date.tm_mon == day->tm_mon &&
        date.tm_year == day->tm_year
    );
}


/*  ---------------------------  op related ---------------- */

static ActionResult redraw(int key) { return AC_REFRESH; }

/*
 *  move to last year, keep month and day
 */
static ActionResult last_year(int key) {
    moveday(&date, -1, "year");
    return AC_REFRESH;
}

static ActionResult next_year(int key) { 
    moveday(&date, 1, "year");
    return AC_REFRESH;
}

static ActionResult today(int key) {
    date = TODAY;
    return AC_REFRESH;
}

static ActionResult i18n(int key) {
    CHINESE = ~CHINESE;
    return AC_REFRESH;
}

static ActionResult up(int key) {
    if (date.tm_mday > 7) {
        date.tm_mday -= 7;
    } else {
        int year = date.tm_year+1900;
        if (date.tm_mon < 3) year--;
        int month   = (date.tm_mon + 9) % 12;
        getDate(&date, year, month, date.tm_wday, 0);
    }
    return AC_REFRESH;
}

static ActionResult down(int key) {
    if (date.tm_mday + 7 <= ndayofmonth(date.tm_year + 1900, date.tm_mon)) {
        date.tm_mday += 7;
    } else {
        int year = date.tm_year+1900;
        if (date.tm_mon >= 9) year += 1;
        int month = (date.tm_mon + 3) % 12;
        getDate(&date, year, month, date.tm_wday, 1);
    }
    return AC_REFRESH;
}

static ActionResult yesterday(int key) {
    moveday(&date, -1, "day");
    return AC_REFRESH;
}

static ActionResult tomorrow(int key) {
     moveday(&date, 1, "day");
    return AC_REFRESH;
}


static const struct {
    const char* key;
    const char* val;
} lhint[] = {
    { .key = "       n: ", .val = "go to next year, keep month & day" },
    { .key = "       p: ", .val = "go to last year, keep month & day" },
    { .key = "       H: ", .val = "show this hint info              " },
    { .key = "       i: ", .val = "international, english or chinese" },
    { .key = "   ENTER: ", .val = "try to open diary if exist       " },
    { .key = "       q: ", .val = "quit                             " },
    { .key = NULL, .val = NULL }
}, rhint[] = {
    { .key = "       h: ", .val = "go to yesterday                  " },
    { .key = "       l: ", .val = "go to tomorrow                   " },
    { .key = "       k: ", .val = "move cursor up                   " },
    { .key = "       j: ", .val = "move cursor down                 " },
    { .key = "       0: ", .val = "move cursor to today             " },
    { .key = NULL, .val = NULL }
};
static ActionResult hint(int key) {
    int y = 0;
    erase();

    attrset(COLOR_PAIR(HIGHLIGHT) | A_BOLD);
    mvprintw(y++, 0, "%s", "yraid " VERSION " - (C) 2020-2024 Jovan.");
    y++;
    int li = 0;
    for (li = 0; lhint[li].key; li++) {
        attrset(COLOR_PAIR(HIGHLIGHT) | A_BOLD);
        mvaddstr(y + li, 0, lhint[li].key);
        attrset(COLOR_PAIR(DEFAULT));
        mvaddstr(y + li, 10, lhint[li].val);
    }
    int ri = 0;
    for (ri = 0; rhint[ri].key; ri++) {
        attrset(COLOR_PAIR(HIGHLIGHT) | A_BOLD);
        mvaddstr(y + ri, 50, rhint[ri].key);
        attrset(COLOR_PAIR(DEFAULT));
        mvaddstr(y + ri, 60, rhint[ri].val);
    }
    y += li > ri ? li : ri;
    attrset(COLOR_PAIR(HIGHLIGHT) | A_BOLD);
    mvprintw(++y, 0, "Press any key to return.");
    refresh();

    int k = ERR;
    do {
        k = getch();
        if (k == ERR) {
            usleep(200 * 1000);
            continue;
        }
        break;
    } while(1);
    return AC_REFRESH;
}

static ActionResult preview(int key) {
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
    return AC_REFRESH;
}

static ActionResult quit() {
    return AC_QUIT;
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
    actions['i']        = i18n;
    actions[KEY_UP]     = up;
    actions[KEY_DOWN]   = down;
    actions[KEY_LEFT]   = yesterday;
    actions[KEY_RIGHT]  = tomorrow;
    actions['k']        = up;
    actions['j']        = down;
    actions['h']        = yesterday;
    actions['l']        = tomorrow;
    actions['H']        = hint;
    actions[ENTER]      = preview;
    actions['q']        = quit;
    actions[KEY_RESIZE] = redraw;
}

void free_actions() { if (actions) free(actions); }

