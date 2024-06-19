#include "yraid.h"


void help() {
    printf("Usage: %s -%s\n", basename(getenv("_")), optstring);
    printf("  -c, --config [key value] check/set config. ");
    printf("If no arguments, all config will be printed.\n");
    printf("                        All keys are listed as follows:\n");
    printf("                        - `repository` to specific datapath\n");
    printf("                        - `birthday` to specific your brithday\n");
    printf("                        - `editor` to specific high priority tool to open file\n");
    printf("  -d, --date=STRING     specify date time\n");
    printf("  -g, --generator       generate default file if not exist\n");
    printf("  -h, --help            show this help info and exit\n");
    printf("  -i, --editor          specify editor to open file\n");
    printf("                        cat will be used if no option\n");
    printf("                        vim is default editor if there is no arg follow -i\n");
    printf("  -l, --list            show diary update view\n");
    printf("  -o, --objectives      target file is monthly objectives\n");
    printf("  -v, --version         show version info and exit\n");
    printf("  -y, --yearly          target file is yearly objectives\n");
    exit(0);
}
