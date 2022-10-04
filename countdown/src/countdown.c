#include "countdown.h"
#include "option.h"
#include "util.h"
#include "crt.h"

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


Item* Item_new(const Option* opt) {
    Item* this = (Item *)malloc(sizeof(Item));
    if (this == NULL) quit("init count down object fail");
    this->color = opt->color;
    this->mode  = opt->mode;
    this->value = opt->sec * 1000 /* ms */;
    this->done  = 0;
    this->draw  = TEXT;

    if (0 == this->value) {
        this->value = 25 * 60 * 1000;
    }

    this->st = (State*)malloc(sizeof(State));
    this->st->item = this;
    this->actions = Actions_keys();
    return this;
}

void Item_delete(Item* this) {
    if (this == NULL) return;
    free(this->st);
    free(this);
}

static Formatime* Item_getmodetime(const Item* this) {
    switch (this->mode) {
    case TICKTOCK: return getformatime(this->value);
    case DEADLINE: return getformatime(this->value - time(NULL) * 1000);
    }
    return getformatime(this->value);
}

static void TextDraw(const Item* this) {
    const Formatime* fmt = Item_getmodetime(this);
    struct tm tm;
    tm.tm_hour = fmt->hour;
    tm.tm_min  = fmt->minute;
    tm.tm_sec  = fmt->second;
    char buf[20] = {0};
    strftime(buf, 20, "%H:%M:%S", &tm);

    clear();
    int color = getcolorpair(this->color);
    attron(color);

    if (fmt->day) {
        char daystr[12] = {0};
        snprintf(daystr, 12, "%d days", fmt->day);
        move(LINES/2 - 1, (COLS - strlen(daystr))/2);
        addstr(daystr);

        move(LINES/2 + 1, (COLS - strlen(buf))/2);
    } else {
        move(LINES/2, (COLS - strlen(buf))/2);
    }
    addstr(buf);
    attroff(color);

    refresh();
}

const char BIGTEXT[17][5][5] = {
	{ { 1,1,1,1,1 }, /* 0 */
	  { 1,0,0,0,1 },
	  { 1,0,0,0,1 },
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 } },
	{ { 0,0,0,0,1 }, /* 1 */
	  { 0,0,0,0,1 },
	  { 0,0,0,0,1 },
	  { 0,0,0,0,1 },
	  { 0,0,0,0,1 } },
	{ { 1,1,1,1,1 }, /* 2 */
	  { 0,0,0,0,1 },
	  { 1,1,1,1,1 },
	  { 1,0,0,0,0 },
	  { 1,1,1,1,1 } },
	{ { 1,1,1,1,1 }, /* 3 */
	  { 0,0,0,0,1 },
	  { 1,1,1,1,1 },
	  { 0,0,0,0,1 },
	  { 1,1,1,1,1 } },
	{ { 1,0,0,0,1 }, /* 4 */
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 },
	  { 0,0,0,0,1 },
	  { 0,0,0,0,1 } },
	{ { 1,1,1,1,1 }, /* 5 */
	  { 1,0,0,0,0 },
	  { 1,1,1,1,1 },
	  { 0,0,0,0,1 },
	  { 1,1,1,1,1 } },
	{ { 1,1,1,1,1 }, /* 6 */
	  { 1,0,0,0,0 },
	  { 1,1,1,1,1 },
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 } },
	{ { 1,1,1,1,1 }, /* 7 */
	  { 0,0,0,0,1 },
	  { 0,0,0,0,1 },
	  { 0,0,0,0,1 },
	  { 0,0,0,0,1 } },
	{ { 1,1,1,1,1 }, /* 8 */
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 },
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 } },
	{ { 1,1,1,1,1 }, /* 9 */
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 },
	  { 0,0,0,0,1 },
	  { 1,1,1,1,1 } },
	{ { 0,0,0,0,0 }, /* : */
	  { 0,0,1,0,0 },
	  { 0,0,0,0,0 },
	  { 0,0,1,0,0 },
	  { 0,0,0,0,0 } },
	{ { 0,1,1,1,0 }, /* A */
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 },
	  { 1,0,0,0,1 },
	  { 1,0,0,0,1 } },
	{ { 1,1,1,1,1 }, /* P */
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 },
	  { 1,0,0,0,0 },
	  { 1,0,0,0,0 } },
	{ { 1,0,0,0,1 }, /* M */
	  { 1,1,0,1,1 },
	  { 1,0,1,0,1 },
	  { 1,0,0,0,1 },
	  { 1,0,0,0,1 } },
	{ { 1,1,1,0,0 }, /* D */
	  { 1,0,0,1,0 },
	  { 1,0,0,0,1 },
	  { 1,0,0,1,0 },
	  { 1,1,1,0,0 } },
	{ { 0,1,1,1,1 }, /* S */
	  { 1,0,0,0,0 },
	  { 0,1,1,1,0 },
	  { 0,0,0,0,1 },
	  { 1,1,1,1,0 } },
	{ { 1,0,0,0,1 }, /* Y */
	  { 0,1,0,1,0 },
	  { 0,0,1,0,0 },
	  { 0,0,1,0,0 },
	  { 0,0,1,0,0 } },
};

static const cchar_t BLOCK = { .chars = {1222, '\0'}};
static const cchar_t SYMBOL = { .chars = {'*', '\0'}};
static const cchar_t DUMMY = { .chars = {' ', '\0'}};
static void BigTextDrawHelper(int y, int x, int t) {
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++)  {
            const cchar_t* n = (BIGTEXT[t][row][col] == 1 ? (t > 9 ? &SYMBOL : &BLOCK) : &DUMMY);
            mvadd_wch(y + row, x + col, n);
        }
    }
}

static void BigTextDraw(const Item* this) {
    const Formatime* fmt = Item_getmodetime(this);
    static const int BIGCOL = 5;

    clear();
    int color = getcolorpair(this->color);
    attron(color);
    int x = 0, y = 0;
    if (fmt->day) {
        y = (LINES - 2 * BIGCOL - 2) / 2;
        int c = 1;
        int day = fmt->day;
        while (day / 10) { c++; day /= 10; }
        int x = (COLS - BIGCOL * (c + 4) - (c - 1) - 6) / 2;
        x = COLS - x - BIGCOL;
        attron(GREEN);
        BigTextDrawHelper(y, x, 15 /* S */); x -= BIGCOL + 1;
        BigTextDrawHelper(y, x, 16 /* Y */); x -= BIGCOL + 1;
        BigTextDrawHelper(y, x, 11 /* A */); x -= BIGCOL + 1;
        BigTextDrawHelper(y, x, 14 /* D */); x -= BIGCOL + 3;
        attroff(GREEN);
        day = fmt->day;
        while (day) {
            int n = day % 10;
            BigTextDrawHelper(y, x, n);
            x -= BIGCOL + 1;
            day = day / 10;
        }

        y += BIGCOL + 2;
    } else {
        y = (LINES - 5) / 2;
    }
    x = (COLS - BIGCOL * 8 - 7) / 2;
    BigTextDrawHelper(y, x, fmt->hour / 10); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->hour % 10); x += BIGCOL+1;
    BigTextDrawHelper(y, x, 10); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->minute / 10); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->minute % 10); x += BIGCOL+1;
    BigTextDrawHelper(y, x, 10); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->second / 10); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->second % 10);
    attroff(color);

    refresh();
}

typedef void(*Draw)(const Item*);
static const Draw draws[DRAWMODESIZE] = {
    TextDraw,
    BigTextDraw,
};
static void Item_draw(const Item* this) {
    if (draws[this->draw]) {
        draws[this->draw](this);
    } else {
        draws[TEXT](this);
    }
}

static void Item_count(Item* this) {
    switch (this->mode) {
    case TICKTOCK: {
        usleep(200 * 1000);
        this->value -= 200/*ms*/;
        if (this->value < 0) this->done = 1;
        break;
    }
    case DEADLINE: {
        usleep(500 * 1000); 
        if (time(NULL) * 1000 > this->value) this->done = 1;
        break;
    }
    default: this->done = 1;
    }
}

void Item_run(Item* this) {
    initcrt();
    
    ActionResCode res = OK;
    do {
        int c = 0;
        if ((c = getch()) != ERR) {
            if (this->actions[c]) res = this->actions[c](this->st);
        }

        if (this->done) break;

        Item_draw(this);

        Item_count(this);

    } while (!this->done);

    Item_delete(this);

    quit(0);
}
