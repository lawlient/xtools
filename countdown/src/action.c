#include "action.h"
#include "countdown.h"

#include <curses.h>


static ActionResCode quit(State* s) { 
    s->item->done = 1;
    return OK;
}

static ActionResCode pause(State* s) {
    if (s->item->mode == DEADLINE) return OK; /* 不支持暂停 */
    /* pause mode */
    clear();
    attron(COLOR_PAIR(GREEN)|A_BLINK);
    int y = LINES/2 - 2, x = ((COLS - 17)/2);
    mvaddstr(y++, x, "+++++++++++++++++");
    mvaddstr(y++, x, "+               +");
    mvaddstr(y++, x, "+     pause     +");
    mvaddstr(y++, x, "+               +");
    mvaddstr(y++, x, "+++++++++++++++++");
    attroff(COLOR_PAIR(GREEN)|A_BLINK);
    refresh();

    for (int i = 0; i != 'p'; ) {
        i = getch();  /* exit with key p */
        if (i == 'q') {
            quit(s);
            break;
        }
    }
    return OK;
}

static ActionResCode changeColorForward(State* s) {
    s->item->color = (s->item->color + 1) % COLORSIZE;
    return OK | REFRESH;
}

static ActionResCode changeColorBackward(State* s) {
    s->item->color = (s->item->color + COLORSIZE - 1) % COLORSIZE;
    return OK | REFRESH;
}

static ActionResCode changeDrawForward(State* s) {
    s->item->draw = (s->item->draw + 1) % DRAWMODESIZE;
    return OK | REFRESH;
}
static ActionResCode changeDrawBackward(State* s) {
    s->item->draw = (s->item->draw + DRAWMODESIZE - 1) % DRAWMODESIZE;
    return OK | REFRESH;
}

Action* Actions_keys() {
    static Action actions[KEY_MAX];
    static bool once_init = false;
    if (once_init) return actions;
    actions['q'] = quit;
    actions['p'] = pause;
    actions['c'] = changeColorForward;
    actions['C'] = changeColorBackward;
    actions['m'] = changeDrawForward;
    actions['M'] = changeDrawBackward;
    once_init = true;
    return actions;
}

