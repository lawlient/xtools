#include "countdown.h"

int main(int argc, char* argv[]) {
    const option* opt = parse_option(argc, argv);
    Item* this = Item_new(opt);
    Item_run(this);
    return 0;
}