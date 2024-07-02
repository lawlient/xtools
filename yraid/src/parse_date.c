#include "yraid.h"

#define DATE_MAX_LEN 128

static char datestr[DATE_MAX_LEN + 1] = {0};
static char *d = datestr;

void moveday(struct tm *day, int off, char *step) {
    if (!strcmp(step, "day")) {
        time_t t = mktime(day);
        t += off * 86400;
        localtime_r(&t, day);
        return;
    }
    if (!strcmp(step, "week")) {
        moveday(day, 7*off, "day");
        return;
    }
    if (!strcmp(step, "month")) {
        day->tm_year += off / 12;
        // fix 
        day->tm_mon += off % 12;
        return;
    }
    if (!strcmp(step, "year")) {
        day->tm_year += off;
        return;
    }
}

typedef enum TokenType_ {
    DONE,
    NUMBER,
    MONDAY, THUSDAY, WEDNESDAY, THURSDAY, FIRDAY, SATURDAY, SUNDAY,
    DAY, WEEK, MONTH, YEAR,
    YESTERDAY, TOMORROW,
    LAST, NEXT,
} TokenType;
int num = 0;



static char* numeric[] = {"zone", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", NULL};
// static char* weekname[] = { "Monday", "Thusday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", NULL };

char* next() {
    char* token = strtok(d, " ");
    d = NULL;
    return token;
}

TokenType token() {
    char* token = next();
    if (!token) return DONE;
    if (!strcmp(token, "yesterday")) return YESTERDAY;
    if (!strcmp(token, "tomorrow")) return TOMORROW;
    if (!strcmp(token, "last")) return LAST;
    if (!strcmp(token, "next")) return NEXT;
    if (!strcmp(token, "day")) return DAY;
    if (!strcmp(token, "week")) return WEEK;
    if (!strcmp(token, "month")) return MONTH;
    if (!strcmp(token, "year")) return YEAR;

    for (int i = 0; numeric[i]; i++) {
        if (!strcmp(token, numeric[i])) {
            num = i;
            return NUMBER;
        }
    }

    num = atoi(token);
    return NUMBER;
}

int parser() {
    TokenType tt = token();
    while (tt) {
        switch (tt) {
        case LAST:
            tt = token();
            switch (tt) {
            case DAY: moveday(&date, -1, "day"); break;
            case WEEK: moveday(&date, -1, "week"); break;
            case MONTH: moveday(&date, -1, "month"); break;
            case YEAR: moveday(&date, -1, "year"); break;
            default:
                printf("invalid date: %s\n", datestr);
                return EINVAL;
            }
            break;
        case NEXT:
            tt = token();
            switch (tt) {
            case DAY: moveday(&date, 1, "day"); break;
            case WEEK: moveday(&date, 1, "week"); break;
            case MONTH: moveday(&date, 1, "month"); break;
            case YEAR: moveday(&date, 1, "year"); break;
            default:
                printf("invalid date: %s\n", datestr);
                return EINVAL;
            }
            break;
        case YESTERDAY: moveday(&date, -1, "day"); break;
        case TOMORROW: moveday(&date, 1, "day"); break;
        case NUMBER:
            tt = token();
            switch (tt) {
            case DAY: moveday(&date, num, "day"); break;
            case WEEK: moveday(&date, num, "week"); break;
            case MONTH: moveday(&date, num, "month"); break;
            case YEAR: moveday(&date, num, "year"); break;
            default:
                printf("invalid date: %s\n", datestr);
                exit(EINVAL);
            }
            break;

        default:
            printf("invalid token: %d\n", tt);
            exit(EINVAL);
        }

        tt = token();
    }
    return 0;
}



int parse_date_string(const char* datearg) {
    if (datearg == NULL) return E_OK;

    strncpy(datestr, datearg, DATE_MAX_LEN);

    parser();

    return E_OK;
}

