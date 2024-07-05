#include "day.h"
#include "time.h"
#include "string.h"

int ndayofmonth(const struct tm *d) {
    static const int MAX_DAY_IN_LEAP_YEAR[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    static const int MAX_DAY_IN_YEAR[]      = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (isLeapYear(d->tm_year + 1900)) {
        return MAX_DAY_IN_LEAP_YEAR[d->tm_mon];
    }
    return MAX_DAY_IN_YEAR[d->tm_mon];
}

int isLeapYear(int year) {
    if (year % 4 == 0 && year % 100 != 0) {
        return 1;
    } 
    return year % 400 == 0;
}

/* get wday of the first day of month
  y: year number, eg: 2001
  m: month [0 - 11]
*/
int firstdayofmonth(int y, int m) {
    /* 基姆拉尔森计算公式
    W= (d+2m+3(m+1)/5+y+y/4-y/100+y/400+1) mod 7 */
    m++; /* start from one */
    if (m == 1 /* Jan */ || m == 2 /* Feb */) {
        y--;
        m += 12;
    }
    return (1 + 2 * m + 3 * (m+1) / 5 + y + y/4 - y/100 + y/400 +1) % 7;
}

void getDate(struct tm *tm, int year, int month, int wday, int first) {
    int wdayOf1stDay = firstdayofmonth(year, month);

    tm->tm_year = year - 1900;
    tm->tm_mon  = month;
    tm->tm_mday = 1 + ((wday - wdayOf1stDay + 7) % 7);
    if (!first) {
        while (tm->tm_mday + 7 <= ndayofmonth(tm)) {
            tm->tm_mday += 7;
        }
    }
    tm->tm_wday = wday;
} 


/* move date by day unit */
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
        day->tm_mon += off % 12;
        if (day->tm_mon < 0) {
            day->tm_year--;
            day->tm_mon += 12;
        } else if (day->tm_mon > 11) {
            day->tm_year++;
            day->tm_mon -= 12;
        }
        return;
    }
    if (!strcmp(step, "year")) {
        day->tm_year += off;
        return;
    }
}

/* offset to week day */
void whatday(struct tm *day, int off, int what) {
    if (what < 0 || what > 7) return;

    int fix = what - day->tm_wday;
    moveday(day, fix, "day");
    if (0 == off) {
        return;
    }

    if (off < 0 && fix < 0) off++;
    else if (off > 0 && fix > 0) off--;

    moveday(day, off, "week");
    return;
}

void orderweekday(struct tm *day, int order, int what) {
    if (what < 0 || what > 7) return;

    day->tm_mday = 1;
    day->tm_wday = firstdayofmonth(day->tm_year + 1900, day->tm_mon);
    if (day->tm_wday == what) order--;
    whatday(day, order, what);
}