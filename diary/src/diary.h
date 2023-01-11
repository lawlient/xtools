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

extern void view();

struct tm;
extern int parse_date_string(const char* date, struct tm *tm);
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


extern struct tm date;
extern const char* repository;


#endif
