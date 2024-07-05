#ifndef DAY_HEADER__
#define DAY_HEADER__


struct tm;

int ndayofmonth(const struct tm *d);
int isLeapYear(int year);
int firstdayofmonth(int y, int m);
void getDate(struct tm *tm, int year, int month, int wday, int first);

void moveday(struct tm *day, int off, char *step);
void whatday(struct tm *day, int off, int what);
void orderweekday(struct tm *day, int order, int what);








#endif