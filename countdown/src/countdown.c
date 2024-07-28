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


static const long kInterval = 200/*ms*/;
static void ticktock_tick(Item* this) {
    usleep(kInterval * 1000);
    this->value -= kInterval/*ms*/;
    if (this->value <= 0) this->quit = 1;
}
static void deadline_tick(Item* this) {
    usleep(kInterval * 1000); 
    if (time(0) > this->value) this->quit = 1;
}
static void stopwatch_tick(Item* this) {
    usleep(kInterval * 1000); 
    this->value += kInterval/*ms*/;
}

static Formatime* ticktock_getime(const Item* this) {
    return getformatime(this->value);
}
static Formatime* deadline_getime(const Item* this) {
    return getformatime((this->value - time(NULL)) * 1000);
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
    this->quit = 0;
    this->draw  = DEFALUT_DRAW_MODE;

    if (0 == opt->mode) {
        this->value = 0;
        this->vt.tick = stopwatch_tick;
        this->vt.getime = stopwatch_getime;
    } else if (1 == opt->mode) {
        this->value = opt->value * 1000 /* ms */;
        this->vt.tick = ticktock_tick;
        this->vt.getime = ticktock_getime;
    } else if (2 == opt->mode) {
        this->value = opt->value;
        this->vt.tick = deadline_tick;
        this->vt.getime = deadline_getime;
    }
    this->actions = calloc(KEY_MAX, sizeof(Action));
    BindDefaultKeys(this);
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

#define TICK(item) do { if (item->vt.tick) item->vt.tick(item); } while(0);

void Item_run(Item* this) {
    initcrt();
    
    int key = 0;
    do {
        if ((key = getch()) != ERR) {
            if (Actions(key, this->st) & ACT_QUIT) break;
        }

        if (this->quit) break;

        Item_draw(this);

        TICK(this);

    } while (1);

    quit(0);
}
