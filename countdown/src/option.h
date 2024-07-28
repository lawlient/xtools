#ifndef OPTION_H
#define OPTION_H

#define OPT_COLOR_LEN 10

typedef struct Option_ {
    char color[OPT_COLOR_LEN];
    int pomodoro;
    int mode;
    long value;
} Option;

typedef enum OptCode_ {
    OPT_OK,
    OPT_ERR,
} OptCode;
extern OptCode parse_option(int argc, char *argv[], Option* opt);





#endif