#ifndef UTIL_H
#define UTIL_H

typedef struct Formatime_ {
    int day;
    int hour;
    int minute;
    int second;
} Formatime;

extern Formatime* getformatime(long ms);

#endif