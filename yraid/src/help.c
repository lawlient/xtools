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
    printf("  -e, --editor          specify editor to open file\n");
    printf("  -g, --generator       generate default file if not exist\n");
    printf("  -h, --help            show this help info and exit\n");
    printf("  -l, --list            calendar view\n");
    printf("  -m, --monthly         target file is monthly objectives\n");
    printf("  -t, --template        \n");
    printf("  -v, --version         show version info and exit\n");
    printf("  -y, --yearly          target file is yearly objectives\n");
    exit(0);
}
