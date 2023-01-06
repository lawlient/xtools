#ifndef DIARY_HEADER__
#define DIARY_HEADER__


#include "config.h"
#include "errcode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <time.h>
#include <getopt.h>

#include <sys/stat.h>

extern const char* optstring;

extern void version();

extern void help();

struct tm;
extern int parse_date_string(const char* date, struct tm *tm);






#endif
