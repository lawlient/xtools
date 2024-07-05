#include "yraid.h"

#define DATE_MAX_LEN 128

static char datestr[DATE_MAX_LEN + 1] = {0};
static char *d = datestr;

typedef enum TokenType_ {
    DONE,
    NUMBER,
    SUNDAY, MONDAY, TUESDAY, WEDNESDAY, THURSDAY, FIRDAY, SATURDAY,
    DAY, WEEK, MONTH, YEAR,
    YESTERDAY, TOMORROW, LAST, NEXT,
    THIS, FIRST, SECOND, THIRD, FOURTH, FIFTH, SIXTH, 
} TokenType;
int num = 0;



static char* numeric[] = {"zone", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", NULL};
static char* weekname[] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", NULL };
static char* order[] = {"this", "first", "second", "third", "fourth", "fifth", "sixth", NULL };

static char* next() {
    char* token = strtok(d, " ");
    d = NULL;
    return token;
}

static TokenType token() {
    char* token = next();
    if (!token) return DONE;
    if (!strcmp(token, "yesterday")) return YESTERDAY;
    if (!strcmp(token, "tomorrow")) return TOMORROW;
    if (!strcmp(token, "last")) {
        num = -1;
        return LAST;
    }
    if (!strcmp(token, "next")) {
        num = 1;
        return NEXT;
    }
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

    for (int i = 0; weekname[i]; i++) {
        if (!strcmp(weekname[i], token)) {
            return SUNDAY + i;
        }
    }

    for (int i = 0; order[i]; i++) {
        if (!strcmp(order[i], token)) {
            num = i;
            return THIS + i;
        }
    }

    num = atoi(token);
    return NUMBER;
}

static int parser() {
    TokenType tt = token();
    while (tt) {
        switch (tt) {
        case LAST:
        case NEXT:
        case NUMBER:
            tt = token();
            switch (tt) {
            case DAY: moveday(&date, num, "day"); break;
            case WEEK: moveday(&date, num, "week"); break;
            case MONTH: moveday(&date, num, "month"); break;
            case YEAR: moveday(&date, num, "year"); break;
            case SUNDAY:
            case MONDAY:
            case TUESDAY:
            case WEDNESDAY:
            case THURSDAY:
            case FIRDAY:
            case SATURDAY: whatday(&date, num, tt - SUNDAY); break;
            default:
                printf("invalid date: %s\n", datestr);
                return EINVAL;
            }
            break;

        case YESTERDAY: moveday(&date, -1, "day"); break;
        case TOMORROW: moveday(&date, 1, "day"); break;

        case SUNDAY:
        case MONDAY:
        case TUESDAY:
        case WEDNESDAY:
        case THURSDAY:
        case FIRDAY:
        case SATURDAY: whatday(&date, 0, tt - SUNDAY); break;

        case FIRST:
        case SECOND:
        case THIRD:
        case FOURTH:
        case FIFTH: 
        case SIXTH:
            tt = token();
            switch (tt) {
            case SUNDAY:
            case MONDAY:
            case TUESDAY:
            case WEDNESDAY:
            case THURSDAY:
            case FIRDAY:
            case SATURDAY: orderweekday(&date, num, tt - SUNDAY); break;
            default:
                printf("invalid date: %s\n", datestr);
                return EINVAL;
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



int cmd_date(const char* datearg) {
    if (datearg == NULL) return 0;
    if (strlen(datearg) > DATE_MAX_LEN) return E2BIG;

    strncpy(datestr, datearg, DATE_MAX_LEN);

    parser();

    return 0;
}

