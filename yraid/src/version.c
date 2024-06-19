#include "yraid.h"

#ifndef VERSION
#define VERSION "default"
#endif

#ifndef EMAIL
#define EMAIL ""
#endif

void version() {
    printf("%s %s \n  Compiled @ %s %s\n", basename(getenv("_")), VERSION, __DATE__, __TIME__);
    if (strlen(EMAIL) > 0) printf("  Contact me by email: %s\n", EMAIL);
    exit(0);
}
