#include "countdown.h"
#include <locale.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    const Option* opt = parse_option(argc, argv);

    /* initialize locale */
    const char* lc_ctype = getenv("LC_CTYPE");
    if (lc_ctype == NULL) {
        lc_ctype = getenv("LC_ALL");
    }
    setlocale(LC_CTYPE, lc_ctype ? lc_ctype : "");

    Item* this = Item_new(opt);
    Item_run(this);
    return 0;
}