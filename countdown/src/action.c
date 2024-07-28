#include "action.h"
#include "countdown.h"

#include <curses.h>


static ActionResCode quit(State* s) { return ACT_QUIT; }

static ActionResCode pause(State* s) {
    erase();
    attron(A_BLINK);
    int y = LINES/2 - 2, x = ((COLS - 17)/2);
    mvaddstr(y++, x, "+++++++++++++++++");
    mvaddstr(y++, x, "+               +");
    mvaddstr(y++, x, "+     pause     +");
    mvaddstr(y++, x, "+               +");
    mvaddstr(y++, x, "+++++++++++++++++");
    attroff(A_BLINK);
    refresh();

    for (int i = 0; i != 'p'; ) {
        i = getch();  /* exit with key p */
        if (i == 'q') {
            return quit(s);
        }
    }
    erase();
    return OK;
}

static ActionResCode changeColorForward(State* s) {
    s->item->color = (s->item->color + 1) % COLORSIZE;
    return ACT_OK;
}

static ActionResCode changeColorBackward(State* s) {
    s->item->color = (s->item->color + COLORSIZE - 1) % COLORSIZE;
    return ACT_OK;
}

static ActionResCode changeDrawForward(State* s) {
    s->item->draw = (s->item->draw + 1) % DRAWMODESIZE;
    erase(); /* 清除当前屏幕 */
    return ACT_OK;
}
static ActionResCode changeDrawBackward(State* s) {
    s->item->draw = (s->item->draw + DRAWMODESIZE - 1) % DRAWMODESIZE;
    erase(); /* 清除当前屏幕 */
    return ACT_OK;
}

static ActionResCode ttyResize(State* s) {
    erase();
    return ACT_OK;
}

void BindDefaultKeys(Item* this) {
    this->actions['q'] = quit;
    this->actions['p'] = pause;
    this->actions['c'] = changeColorForward;
    this->actions['C'] = changeColorBackward;
    this->actions['m'] = changeDrawForward;
    this->actions['M'] = changeDrawBackward;
    this->actions[KEY_RESIZE] = ttyResize;
}
