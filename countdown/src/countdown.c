#include "countdown.h"
#include "option.h"
#include "util.h"
#include "crt.h"
#include "action.h"

#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef enum Mode_ {
    TICKTOCK, // countdown for specific seconds, you can pause it and continue it
    DEADLINE, // countdown for deadline, you can not pause it
    STOPWATCH, // stopwatch 
    POMODORO,
} Mode;


static const long kInterval = 200/*ms*/;
static void ticktock_count(Item* this) {
    usleep(kInterval * 1000);
    this->value -= kInterval/*ms*/;
    if (this->value <= 0) this->quit = 1;
}
static void deadline_count(Item* this) {
    usleep(kInterval * 1000); 
    if (time(NULL) * 1000 > this->value) {
        this->quit = 1;
    }
}
static void stopwatch_count(Item* this) {
    usleep(kInterval * 1000); 
    this->value += kInterval/*ms*/;
}

static Formatime* ticktock_getime(const Item* this) {
    return getformatime(this->value);
}
static Formatime* deadline_getime(const Item* this) {
    return getformatime(this->value - time(NULL) * 1000);
}
static Formatime* stopwatch_getime(const Item* this) {
    return getformatime(this->value);
}


static Color parse_color(const char* color) {
    if (!strcasecmp(color, "red")) return RED;
    if (!strcasecmp(color, "green")) return GREEN;
    if (!strcasecmp(color, "cyan")) return CYAN;
    if (!strcasecmp(color, "blue")) return BLUE;
    return CUSTOM;
}

Item* Item_new(const Option* opt) {
    Item* this = (Item *)malloc(sizeof(Item));
    if (this == NULL) quit("init count down object fail");
    this->color = parse_color(opt->color);
    this->draw  = DEFALUT_DRAW_MODE;
    this->actions = calloc(KEY_MAX, sizeof(Action));

    this->value = opt->sec * 1000 /* ms */;
    long nowms = (long)time(0) * 1000;
    if (0 == this->value) {
        this->vt.count = stopwatch_count;
        this->vt.getime = stopwatch_getime;
        BindDefaultKeys(this);
    } else if (nowms >= this->value) {
        this->vt.count = ticktock_count;
        this->vt.getime = ticktock_getime;
        BindDefaultKeys(this);
    } else if (nowms < this->value) {
        this->vt.count = deadline_count;
        this->vt.getime = deadline_getime;
        BindDeadlineKeys(this);
    } else {
        BindDefaultKeys(this);
        // todo
    }
    this->st = (State*)malloc(sizeof(State));
    this->st->item = this;
    return this;
}

void Item_delete(Item* this) {
    if (this == NULL) return;
    free(this->st);
    free(this->actions);
    free(this);
}

static ActionResCode Actions(int key, State* s) { 
    Item* item = s->item;
    return item->actions[key] ? item->actions[key](s) : ACT_OK;
}

#define Item_Count(item) do { if (item->vt.count) item->vt.count(item); } while(0);

void Item_run(Item* this) {
    initcrt();
    
    int key = 0;
    do {
        if ((key = getch()) != ERR) {
            if (Actions(key, this->st) & ACT_QUIT) break;
        }

        if (this->quit) break;

        Item_draw(this);

        Item_Count(this);

    } while (1);

    Item_delete(this);

    quit(0);
}
