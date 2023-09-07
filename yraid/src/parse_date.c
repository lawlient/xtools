#include "yraid.h"


/// static char* numeric[] = {"zone", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten", NULL};



int parse_date_string(const char* date, struct tm *tm) {
    if (date == NULL) return E_OK;
    time_t now = time(0);

    if (!strcmp(date, "yesterday")) {
        now -= 86400;
        localtime_r(&now, tm);
        return E_OK;
    }

    if (!strcmp(date, "tomorrow")) {
        now += 86400;
        localtime_r(&now, tm);
        return E_OK;
    }


    struct args {
        const char *p;
        int l;
        int used;
    } datestr[30];
    int argx = 0;

    const char *p = date;
    while (*p == ' ') p++;
    for (int i = 0; i < 30 && *p != '\0'; i++) {
        datestr[i].p = p;
        while (*p != '\0' && *p != ' ') p++;
        datestr[i].l = p - datestr[i].p;
        while (*p != '\0' && *p == ' ') p++;
        datestr[i].used = 0;
        argx++;
    }

    /// for (int i = 0; i < argx; i++) {
    ///     char buf[100];
    ///     strncpy(buf, datestr[i].p, datestr[i].l);
    ///     buf[datestr[i].l] = '\0';
    ///     printf("%d : %s\n", i, buf);
    /// }
    /// exit(0);

    int used  = 0;
    int day   = 0;
    int month = 0;
    int year  = 0;
    while (used < argx) {
        for (int i = 0; i < argx; i++) {
            if (datestr[i].used) continue;
            if (!strcmp(datestr[i].p, "day") || !strcmp(datestr[i].p, "days")) {
                datestr[i].used = 1; used++;
                if (i - 1 >= 0) day = atoi(datestr[i-1].p);
                if (0 == day) exit(E_DATESTR_INVALID);
                datestr[i-1].used = 1; used++;
                continue;
            }

            if (!strcmp(datestr[i].p, "month") || !strcmp(datestr[i].p, "monthes")) {
                datestr[i].used = 1; used++;
                if (i - 1 >= 0) month = atoi(datestr[i-1].p);
                if (0 == month) exit(E_DATESTR_INVALID);
                datestr[i-1].used = 1; used++;
                continue;
            }

            if (!strcmp(datestr[i].p, "year") || !strcmp(datestr[i].p, "years")) {
                datestr[i].used = 1; used++;
                if (i - 1 >= 0) year = atoi(datestr[i-1].p);
                if (0 == year) exit(E_DATESTR_INVALID);
                datestr[i-1].used = 1; used++;
                continue;
            }
        }
    }

    now += day * 86400;
    localtime_r(&now, tm);
    return E_OK;
}

