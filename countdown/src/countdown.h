#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include "action.h"
#include "option.h"

typedef enum DrawMode_ {
    TEXT = 0,
    BIGTXT,
    DRAWMODESIZE,
} DrawMode;

typedef struct Item_ {
    Color color;
    Mode mode;
    long value;
    int quit;
    DrawMode draw;

    State* st;
    Action* actions;
} Item;

/* create a countdown object from commandline option */
extern Item* Item_new(const Option* opt);

extern void Item_delete(Item* this);

extern void Item_run(Item* this);

#endif
