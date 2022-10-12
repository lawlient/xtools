#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include "draw.h"
#include "action.h"

struct Item_;
typedef struct Item_ Item;


typedef struct Formatime_ *(*Item_Time)(const Item*);
typedef void (*Item_Count)(Item*);

typedef struct Vtable_ {
    Item_Time getime;
    Item_Count count;
} Vtable;

#define Item_GetimeFn(item) (item->vt.getime)
#define Item_Getime(item)  (item->vt.getime(item))

struct Item_ {
    Color color;
    long value;
    int quit;
    DrawMode draw;
    Vtable vt;

    struct State_* st;
    Action *actions;
};

struct Option_;
/* create a countdown object from commandline option */
extern Item* Item_new(const struct Option_* opt);

extern void Item_delete(Item* this);

extern void Item_run(Item* this);

extern void Item_draw(const Item* this);

#endif
