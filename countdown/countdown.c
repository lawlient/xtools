#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>


#ifdef HAVE_GETOPT_H
    #include <getopt.h>
#endif

enum color_t {
    CUSTOM, 
    RED,
    GREEN,
    CYAN,
    COLORSIZE,
};

enum mode_t {
    TICKTOCK, // countdown for specific seconds, you can pause it and continue it
    DEADLINE, // countdown for deadline, you can not pause it
};

struct xformat {
    int day;
    int hour;
    int minute;
    int second;
};

struct context {
    enum color_t color;
    enum mode_t mode;
    long value;
    bool quit;
};
static struct context gctx = {
    .color = CUSTOM,
    .mode = TICKTOCK, /* default mode */
    .value = 25 * 60,
    .quit = false,
};

struct xformat* getformatime(long t) {
    int sec = t;
    static struct xformat fmt = {0};
    if (t <= 0) return &fmt;
    fmt.day = sec / 86400; sec = sec % 86400;
    fmt.hour = sec / 3600; sec = sec % 3600;
    fmt.minute = sec / 60; sec = sec % 60;
    fmt.second = sec;
    return &fmt;
}

struct xformat* getmodetime(const struct context* ctx) {
    switch (ctx->mode) {
    case TICKTOCK: return getformatime(ctx->value);
    case DEADLINE: return getformatime(ctx->value - time(NULL));
    }
    return getformatime(ctx->value);
}

int getcolorpair(const struct context* ctx) {
    switch (ctx->color) {
        case RED: return COLOR_PAIR(RED);
        case GREEN: return COLOR_PAIR(GREEN);
        case CYAN: return COLOR_PAIR(CYAN);
        default: break;
    }
    return COLOR_PAIR(CUSTOM);
}

void quit(char* msg) {
    if (msg) printf("%s\n", msg);
    endwin();
    exit(0);
}

void usage() {
    printf(" Usage: countdown -[hV] [-c color] [-m mode] [-t sec]\n\n"
           "  -h: Print usage and exit\n\n"
           "  -V: Print version information and exit\n\n"
           "  -c [color]: color for text, you can change it with key 'c' \n"
           "     {custom, red, green, cyan}\n\n"
           "  -m [mode]: countdown mode, you cannot change it after you start\n"
           "     ticks    : countdown for specific seconds, you can pause it and continue it again\n"
           "     deadline : countdown for deadline, you can not pause it\n\n"
           "  -t [sec|deadline]: specific your time\n");
}

void version(void) {
    printf(" countdown version %s (compiled %s, %s)\n", VERSION, __TIME__, __DATE__);
    printf("  Email: jiuanx2011@163.com\n");
    printf("  Web: https://github.com/lawlient/xtools/countdown\n");
}

void parse_option(int argc, char* argv[]) {
    opterr = 0;
    int optchr;
    while ((optchr = getopt(argc, argv, "c:m:t:hV")) != EOF) {
        switch(optchr) {
            case 'h': usage(); exit(0);
            case 'V': version(); exit(0);
            case 'c': {
                if (!strcasecmp(optarg, "red")) {
                    gctx.color = RED;
                } else if (!strcasecmp(optarg, "green")) {
                    gctx.color = GREEN;
                } else if (!strcasecmp(optarg, "cyan")) {
                    gctx.color = CYAN;
                } else {
                    gctx.color = CUSTOM;
                }
                continue;
            }
            case 'm': {
                if (!strcasecmp(optarg, "tick")) {
                    gctx.mode = TICKTOCK;
                } else if (!strcasecmp(optarg, "deadline")) {
                    gctx.mode = DEADLINE;
                }
                continue;
            }
            case 't': {
                if ((gctx.value = atol(optarg)) <= 0) {
                    quit("countdown time should be more than 0");
                };
                continue;
	        }
            default: usage(); exit(0);
        }
    }
    return;
}

void initcolor() {
    if (!has_colors()) return;
    start_color();
    init_pair(CUSTOM, COLOR_WHITE,  COLOR_BLACK);
    init_pair(RED, COLOR_RED,  COLOR_BLACK);
    init_pair(GREEN, COLOR_GREEN,  COLOR_BLACK);
    init_pair(CYAN, COLOR_CYAN,  COLOR_BLACK);
}


void init() {
    initscr();
    noecho();
    nodelay(stdscr, true);
    curs_set(0); /* hide cursor */
    keypad(stdscr, true);
    initcolor();
}

void draw_pause() {
    clear();
    static const char* box1  = "+++++++++++++++++";
    static const char* box2  = "+               +";
    static const char* text  = "+     pause     +";
    attron(COLOR_PAIR(GREEN)|A_BLINK);
    int x = ((COLS - strlen(box1))/2);
    int y = LINES/2 - 2;
    move(y, x); addstr(box1); y++; 
    move(y, x); addstr(box2); y++; 
    move(y, x); addstr(text); y++; 
    move(y, x); addstr(box2); y++; 
    move(y, x); addstr(box1);
    attroff(COLOR_PAIR(GREEN)|A_BLINK);
    refresh();
}

void pause_count() {
    int i;
    draw_pause();
    do {
        i = getch();
    } while (i != 'p');
}


void draw() {
    clear();

    time_t now = time(0);
    struct tm* tm = localtime(&now);
    const struct xformat* fmt = getmodetime(&gctx);
    tm->tm_hour = fmt->hour;
    tm->tm_min  = fmt->minute;
    tm->tm_sec  = fmt->second;
    char buf[20] = {0};
    strftime(buf, 20, "%H:%M:%S", tm);

    int color = getcolorpair(&gctx);
    attron(color);

    if (fmt->day) {
        char daystr[12] = {0};
        snprintf(daystr, 12, "%d days", fmt->day);
        move(LINES/2 - 1, (COLS - strlen(daystr))/2);
        addstr(daystr);

        move(LINES/2 + 1, (COLS - strlen(buf))/2);
        addstr(buf);
    } else {
        move(LINES/2, (COLS - strlen(buf))/2);
        addstr(buf);
    }
    attroff(color);

    refresh();
}

void recalc() {
    switch (gctx.mode) {
    case TICKTOCK: {
        if (--gctx.value < 0) gctx.quit = true;
        break;
    }
    case DEADLINE: {
        if (time(NULL) > gctx.value) gctx.quit = true;
        break;
    }
    default:
        gctx.quit = true;
    }
}

int countdown() {
    int c = 0;
    do {
        if ((c = getch()) != ERR) {
            switch (c) {
                case 'c': { gctx.color = (gctx.color + 1) % COLORSIZE; break; }
                case 'C': { gctx.color = (gctx.color + COLORSIZE - 1) % COLORSIZE; break; }
                case 'p': { if (gctx.mode == TICKTOCK) pause_count(); break; }
                case 'q': quit(0);
                default: break;
            }
        }

        draw();

        recalc();

        sleep(1);

    } while (!gctx.quit);
}

int main(int argc, char* argv[]) {

    parse_option(argc, argv);

    init();

    countdown();

    quit(0);

    return 0;
}
