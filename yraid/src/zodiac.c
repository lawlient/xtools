#include "yraid.h"


static const char *ZODIAC[] = { "🐭", "🐮", "🐯", "🐰", "🐲", "🐍", "🐴", "🐏", "🐵", "🐔", "🐶", "🐷", NULL};

const char *zodiac() {
    int idx = 4;
    int off = (date.tm_year + 1900 - 2012) % 12;
    idx = (idx + off) % 12;
    if (idx < 0) idx += 12;
    return ZODIAC[idx];
}

