#include "draw.h"
#include "countdown.h"
#include "util.h"

#include <string.h>
#include <time.h>
#include <curses.h>

static void TextDraw(const Item* this) {
	if (!Item_GetimeFn(this)) return;
    const Formatime* fmt = Item_Getime(this);
    struct tm tm;
    tm.tm_hour = fmt->hour;
    tm.tm_min  = fmt->minute;
    tm.tm_sec  = fmt->second;
    char buf[20] = {0};
    strftime(buf, 20, "%H:%M:%S", &tm);

    int color = COLOR_THEME[T_DEFAULT][this->color];
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

static const char BIGTEXT[17][5][5] = {
	{ { 1,1,1,1,1 }, /* 0 */
	  { 1,0,0,0,1 },
	  { 1,0,0,0,1 },
	  { 1,0,0,0,1 },
	  { 1,1,1,1,1 } },
	{ { 0,0,1,0,0 }, /* 1 */
	  { 0,0,1,0,0 },
	  { 0,0,1,0,0 },
	  { 0,0,1,0,0 },
	  { 0,0,1,0,0 } },
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

static void BigTextDrawHelper(int y, int x, int t, int c) {
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 5; col++)  {
            if (BIGTEXT[t][row][col] == 1) {
                int color = COLOR_THEME[T_BG][c];
                attron(color);
                mvaddch(y + row, x + col, ' ');
                attroff(color);
            } else {
                mvaddch(y + row, x + col, ' ');
            }
        }
    }
}

static void BigTextDraw(const Item* this) {
	if (!Item_GetimeFn(this)) return;
    const Formatime* fmt = Item_Getime(this);
    static const int BIGCOL = 5;

    int x = 0, y = 0;
    if (fmt->day) {
        y = (LINES - 2 * BIGCOL - 2) / 2;
        int c = 1;
        int day = fmt->day;
        while (day / 10) { c++; day /= 10; }
        int x = (COLS - BIGCOL * (c + 4) - (c - 1) - 6) / 2;
        x = COLS - x - BIGCOL;
        BigTextDrawHelper(y, x, 15 /* S */, this->color); x -= BIGCOL + 1;
        BigTextDrawHelper(y, x, 16 /* Y */, this->color); x -= BIGCOL + 1;
        BigTextDrawHelper(y, x, 11 /* A */, this->color); x -= BIGCOL + 1;
        BigTextDrawHelper(y, x, 14 /* D */, this->color); x -= BIGCOL + 3;
        day = fmt->day;
        while (day) {
            int n = day % 10;
            BigTextDrawHelper(y, x, n, this->color);
            x -= BIGCOL + 1;
            day = day / 10;
        }

        y += BIGCOL + 2;
    } else {
        y = (LINES - 5) / 2;
    }
    x = (COLS - BIGCOL * 8 - 7) / 2;
    BigTextDrawHelper(y, x, fmt->hour / 10, this->color); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->hour % 10, this->color); x += BIGCOL+1;
    BigTextDrawHelper(y, x, 10, this->color); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->minute / 10, this->color); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->minute % 10, this->color); x += BIGCOL+1;
    BigTextDrawHelper(y, x, 10, this->color); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->second / 10, this->color); x += BIGCOL+1;
    BigTextDrawHelper(y, x, fmt->second % 10, this->color);

    refresh();
}

typedef void(*Draw)(const Item*);
static const Draw draws[DRAWMODESIZE] = {
    TextDraw,
    BigTextDraw,
};

void Item_draw(const Item* this) {
    if (draws[this->draw]) {
        draws[this->draw](this);
    }
}