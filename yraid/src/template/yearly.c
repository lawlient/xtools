#include "template.h"



static const char* targetname(const struct tm* date) {
    static char name[FILENAME_LENGTH];
    strftime(name, 50, "./%Y/%m/objectives.md", date);
    return name;
}


Template yearly = {
    .classname  = "yearly",
    .targetname = targetname,
    .generator  = generator,
    .generate   = generate,
};