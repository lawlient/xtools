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

/* command line opetions string */
extern const char* optstring;


/* printf version information then exit */
extern void version();

/* printf help information then exit */
extern void help();

/* enter interactive mode, check in one year */
extern void view();

struct tm;
/* parse date string
 * eg: "yesterday, tomorrow, +2 day, -2 days, and so on */
extern int parse_date_string(const char* date, struct tm *tm);

/* append diary common suffix while generating from template */
extern void template_suffix(FILE *f);



typedef char *(*Name)();
typedef void (*Template)();

typedef struct Module_ {
    Name name;
    Template generator;
} Module;


extern const Module daily;
extern const Module monthly;
extern const Module yearly;

extern const Module *d;


/* global variable, the date & time parse from command line option -d
 * the default value is now if there is no option -d */
extern struct tm date;


/* absolute directory of diary repository, rely on ENV: DIARY */
extern const char* repository;


#endif
