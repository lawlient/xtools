#include "option.h"
#include "crt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef HAVE_GETOPT_H
    #include <getopt.h>
#endif


static Option opt = {
    .color = CUSTOM,
    .mode  = STOPWATCH,
    .sec = 0,
};

void usage() {
    printf(" Usage: countdown -[hVd] [-c color] [-t sec]\n\n"
           "  -h: Print usage and exit\n\n"
           "  -V: Print version information and exit\n\n"
           "  -c [color]: color for text, you can change it with key 'c' \n"
           "     {custom, red, green, cyan}\n\n"
           "  -t [sec|deadline]: specific your time\n");
}

void version() {
    printf(" countdown version %s (compiled %s, %s)\n", VERSION, __TIME__, __DATE__);
    printf("  Email: jiuanx2011@163.com\n");
    printf("  Web: https://github.com/lawlient/xtools/countdown\n");
}

Option* parse_option(int argc, char* argv[]) {
    opterr = 0;
    int optchr;
    while ((optchr = getopt(argc, argv, "c:t:hV")) != EOF) {
        switch(optchr) {
            case 'h': usage(); exit(0);
            case 'V': version(); exit(0);
            case 'c': {
                if (!strcasecmp(optarg, "red")) {
                    opt.color = RED;
                } else if (!strcasecmp(optarg, "green")) {
                    opt.color = GREEN;
                } else if (!strcasecmp(optarg, "cyan")) {
                    opt.color = CYAN;
                } else {
                    opt.color = CUSTOM;
                }
                continue;
            }
            case 't': {
                if ((opt.sec = atol(optarg)) <= 0) {
                    quit("countdown time should be more than 0");
                };
                long now = time(0);
                opt.mode = (now < opt.sec ? DEADLINE : TICKTOCK);
                continue;
	        }
            default: usage(); exit(0);
        }
    }
    return &opt;
}

