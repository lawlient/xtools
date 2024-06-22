#include "template.h"


static const char* targetname(const struct tm* date) {
    static char name[FILENAME_LENGTH];
    strftime(name, FILENAME_LENGTH, "./%Y/%m/%d.md", date);
    return name;
}

Template daily = {
    .classname  = "daily",
    .targetname = targetname,
    .generator  = generator,
    .generate   = generate,
};