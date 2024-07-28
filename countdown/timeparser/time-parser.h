#ifndef TIME_HEADER__
#define TIME_HEADER__

#include <time.h>

typedef enum TSMode_ {
    TICK = 0,
    REL  = 1,
    ABS  = 2,
} TSMode;
extern TSMode tsmode;
extern time_t tscmd;

extern void init_scan_buffer(const char* in);

extern int cmd_time(const char* opt);

#endif