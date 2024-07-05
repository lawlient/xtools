#ifndef DAY_HEADER__
#define DAY_HEADER__


struct tm;

/* 
 * get number of days in year/month 
 *   y: year, eg: 2024
 *   m: month, [0 - 11] 
 */
int ndayofmonth(int y, int m);

int isleapyear(int year);

/* 
 * get weekday of the first day of month
 *   y: year number, eg: 2024
 *   m: month [0 - 11]
 */
int firstdayofmonth(int y, int m);

void getDate(struct tm *tm, int year, int month, int wday, int first);

/* 
 * move day by unit
 *   off > 0: forward move
 *   off < 0: backward move
 *   unit: one of [ day, week, month, year ]
 */
void moveday(struct tm *day, int off, char *unit);

/* 
 * move day to weekday
 *   off = 0 : the same week
 *   off > 0 : forward off th wday
 *   off < 0 : backward off th wday
 *   wday: [0 - 6] 0(sunday), 1(monday), and so on
 */
void whatday(struct tm *day, int off, int wday);

/*
 * move day to the order th weekday in current month, eg: third monday, second sunday, ...
 *   wday: [0 - 6] 0(sunday), 1(monday), and so on
 */
void orderweekday(struct tm *day, int order, int wday);








#endif