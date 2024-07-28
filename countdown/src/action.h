#ifndef ACTION_H
#define ACTION_H

typedef enum {
    ACT_OK = 0x0,
    ACT_QUIT = 0x1,
} ActionResCode;

struct Item_;
typedef struct State_ {
    struct Item_* item;
} State;

typedef ActionResCode (*Action)(State* s);

struct Item_;
typedef struct  Item_ Item;
void BindDefaultKeys(Item *this);


#endif
