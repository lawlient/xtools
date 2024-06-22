#ifndef TEMPLATE_HEADER
#define TEMPLATE_HEADER

#include "../yraid.h"

#define FILENAME_LENGTH 128


struct tm;

typedef struct Template_ {
    const char* classname;
    const char* (*targetname)();
    int (*generator)(const char* templname, const char* targetname);
    int (*generate)(struct Template_ *t, const struct tm*);
} Template;


extern int generator(const char* templname, const char* targetname);
extern int generate(Template *t, const struct tm* date);


extern void template(int argc, char *argv[]);

extern Template daily;
extern Template monthly;
extern Template yearly;
extern Template *t;

#endif