#include "countdown.h"
#include "option.h"
#include "action.h"

#include <locale.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    Option opt;
    OptCode code = parse_option(argc, argv, &opt);

    /* initialize locale */
    const char* lc_ctype = getenv("LC_CTYPE");
    if (lc_ctype == NULL) {
        lc_ctype = getenv("LC_ALL");
    }
    setlocale(LC_CTYPE, lc_ctype ? lc_ctype : "");

    Item* this = Item_new(&opt);
    Item_run(this);
    Item_delete(this);
    return 0;
}
