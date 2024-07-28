#include "option.h"
#include "time-parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_GETOPT_H
    #include <getopt.h>
#endif


static const struct option long_options[] = {
    { "color",      required_argument,  0, 'c'},
    { "help",       no_argument,        0, 'h'},
    { "version",    no_argument,        0, 'v'},
    { "time",       required_argument,  0, 't'},
    { "pomorodo",   no_argument,        0, 'p'},
    { 0, 0, 0, 0},
};

static void usage() {
    printf(" Usage: countdown -[hVd] [-c color] [-t sec]\n\n"
           "  -h    --help       Print usage and exit\n\n"
           "  -v    --version    Print version information and exit\n\n"
           "  -c    --color      color for text, you can change it with key 'c' \n"
           "                     {custom, red, green, cyan}\n\n"
           "  -t    --time       specific your time\n");
}

static void version() {
    printf(" countdown version %s (compiled %s, %s)\n", VERSION, __TIME__, __DATE__);
    printf("  Email: jiuanx2011@163.com\n");
    printf("  Web: https://github.com/lawlient/xtools/countdown\n");
}


static void init_option(Option* opt) {
    opt->color[0] = '\0';
    opt->pomodoro = 0;
    opt->mode = 0;
    opt->value = 0;
}

OptCode parse_option(int argc, char* argv[], Option* opt) {
    init_option(opt);
    int optchr, opti = 0;
    while ((optchr = getopt_long(argc, argv, "c:t:hpv", long_options, &opti)) != EOF) {
        switch(optchr) {
            case 'h': usage(); exit(0);
            case 'v': version(); exit(0);
            case 'c': {
                strncpy(opt->color, optarg, OPT_COLOR_LEN);
                continue;
            }
            case 't': {
                if (cmd_time(optarg)) return OPT_ERR;
                opt->mode = (int)tsmode;
                opt->value = (long)tscmd/* define in parser.y */;
                continue;
	        }
            case 'p': {
                opt->pomodoro = 1;
                continue;
            }
            default: usage(); exit(0);
        }
    }
    return OPT_OK;
}

