#include "util.h"

Formatime* getformatime(long ms) {
    static Formatime fmt = {0};

    int msec = ms % 1000;
    int sec = ms / 1000 + (msec >= 500 ? 1 : 0);
    if (sec <= 0) return &fmt;
    fmt.day = sec / 86400; sec = sec % 86400;
    fmt.hour = sec / 3600; sec = sec % 3600;
    fmt.minute = sec / 60; sec = sec % 60;
    fmt.second = sec;
    return &fmt;
}
