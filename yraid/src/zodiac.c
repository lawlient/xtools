#include "yraid.h"


static const char *ZODIAC[] = { "🐭", "🐮", "🐯", "🐰", "🐲", "🐍", "🐴", "🐏", "🐵", "🐔", "🐶", "🐷", NULL};

const char *zodiac() {
    return ZODIAC[date.tm_year % 12];
}

