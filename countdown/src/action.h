#ifndef ACTION_H
#define ACTION_H

typedef enum {
    OK = 0x0,
    REFRESH = 0x1,
} ActionResCode;

struct Item_;
typedef struct State_ {
    struct Item_* item;
} State;

typedef ActionResCode (*Action)(State* s);

Action* Actions_keys();


#endif
