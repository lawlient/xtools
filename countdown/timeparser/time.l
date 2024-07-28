%option noyywrap
%{
#define __USE_XOPEN
#include "time.h"

#include <stdio.h>
#include "parser.h"

static int strtotime(const char* format);
%}


%%

[0-9]+[dDhHmMsS] {
    if (strchr("dD", yytext[strlen(yytext)-1])) {
        yylval = atoi(yytext) * 86400;
        return RELATIVE;
    } else if (strchr("hH", yytext[strlen(yytext)-1])) {
        yylval = atoi(yytext) * 3600;
        return RELATIVE;
    } else if (strchr("mM", yytext[strlen(yytext)-1])) {
        yylval = atoi(yytext) * 60;
        return RELATIVE;
    } else if (strchr("sS", yytext[strlen(yytext)-1])) {
        yylval = atoi(yytext);
        return RELATIVE;
    }
}

[0-9]{4}[-][0-9]{2}[-][0-9]{2}[ ]+[0-9]{2}[:][0-9]{2}[:][0-9]{2} {
    strtotime("%4Y-%2m-%2d %T");
    return ABSOLUTE;
}

[0-9]{4}[/][0-9]{2}[/][0-9]{2}[ ]+[0-9]{2}[:][0-9]{2}[:][0-9]{2} {
    strtotime("%4Y/%2m/%2d %T");
    return ABSOLUTE;
}

[0-9]{4}[-][0-9]{2}[-][0-9]{2} {
    strtotime("%4Y-%2m-%2d");
    return ABSOLUTE;
}

[0-9]{4}[/][0-9]{2}[/][0-9]{2} {
    strtotime("%4Y/%2m/%2d");
    return ABSOLUTE;
}

[0-9]{2}[:][0-9]{2}[:][0-9]{2} {
    strtotime("%T");
    return ABSOLUTE;
}
%%


int strtotime(const char* format) {
    struct tm tm;
    time_t t = time(0);
    // set default today
    localtime_r(&t, &tm);
    tm.tm_hour = tm.tm_min = tm.tm_sec = 0;

    char *ret = strptime(yytext, format, &tm);
    if (!ret) {
        printf("parse `%s` error\n", yytext);
        return 1;
    }

    t = mktime(&tm);
    if (-1 == t) {
        // sometimes OVERFLOW error if we didn't set value to tm
        printf("%s\n", strerror(errno));
        return 2;
    }
    yylval = t;
    return 0;
}


void init_scan_buffer(const char* in) {
    YY_BUFFER_STATE bp;
    bp = yy_scan_string(in);
    if (!bp) {
        printf("new buffer fail\n");
        exit(0);
    }
    yy_switch_to_buffer(bp);
}