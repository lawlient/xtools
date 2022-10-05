#ifndef OPTION_H
#define OPTION_H

typedef enum Color_ {
    CUSTOM, 
    RED,
    GREEN,
    CYAN,
    BLUE,
    COLORSIZE,
} Color;

typedef enum CTheme_ {
    T_TEXT,
    T_BIG_TEXT,
} CTheme;
extern const int COLOR_THEME[][COLORSIZE];

typedef enum Mode_ {
    TICKTOCK, // countdown for specific seconds, you can pause it and continue it
    DEADLINE, // countdown for deadline, you can not pause it
} Mode;

typedef struct Option_ {
    Color color;
    Mode mode;
    long sec;
} Option;

/* */
extern Option* parse_option(int argc, char *argv[]);





#endif