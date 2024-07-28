#include "time-parser.h"

#include "parser.h"

int cmd_time(const char* opt) {
    init_scan_buffer(opt);

    int ret = yyparse();
    return ret;
}