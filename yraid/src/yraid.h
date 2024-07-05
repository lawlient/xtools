#ifndef DIARY_HEADER__
#define DIARY_HEADER__

#include "template/template.h"

#include "day.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libgen.h>
#include <unistd.h>
#define __USE_XOPEN
#include <time.h>
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#define ENV_HOME (getenv("HOME"))
#define PROC basename(getenv("_"))

/* config keys */
#define CFG_REPOSITORY "repository"
#define CFG_BIRTHDAY "birthday"
#define CFG_EDITOR "editor"
const char* configfilepath();
const char* get_config(const char* key);
void parse_config();

/* command line opetions string */
extern const char* optstring;

/* config */
extern void config(int argc, char* argv[]) __attribute__ ((__noreturn__));

/* printf version information then exit */
extern void version() __attribute__ ((__noreturn__));

/* printf help information then exit */
extern void help() __attribute__ ((__noreturn__));

/* enter interactive mode, check in one year */
extern void view();

// struct tm;
/* parse date string
 * eg: "yesterday, tomorrow, +2 day, -2 days, and so on */
extern int parse_date_string(const char* date);

extern void workdir(int exit);

/* global variable, the date & time parse from command line option -d
 * the default value is now if there is no option -d */
extern struct tm date;

/* get user's age if `birthday` is setted */
extern const char* age();

/* get chinese zodiac of date */
extern const char *zodiac();

#endif
