#include "diary.h"



void help() {
    printf("Usage: "PROC" -%s\n\n", optstring);
    printf("  -d, --date=STRING     specify date time\n\n");
    printf("  -h, --help            show this help info and exit\n\n");
    printf("  -i, --editor          specify editor to open file\n");
    printf("                        cat will be used if no option\n");
    printf("                        vim is default editor if there is no arg follow -i\n\n");
    printf("  -o, --objectives      target file is monthly objectives\n\n");
    printf("  -v, --version         show version info and exit\n\n");
    printf("  -y, --yearly          target file is yearly objectives\n\n");
    exit(0);
}
