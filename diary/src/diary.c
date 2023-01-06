#include "diary.h"



#include <string.h>
#include <stdint.h>
#include <time.h>
#include <getopt.h>



#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static int parse_date_string(const char* date, struct tm * file);
static int cdiary();

static struct option log_options[] = {
    {"date", required_argument, 0, 0}
};


int main(int argc, char *argv[]) {
    int c;
    struct tm filetm;
    
    while (1) {
        int option_index = 0;
        c = getopt_long(argc, argv, "d", log_options, &option_index);
        if (-1 == c) break;

        switch (c) {
            case 'd': parse_date_string(optarg, &filetm); break;
            default: break;
        }

    };


    struct stat fst;
    char filename[50];
    strftime(filename, 50, "./%Y/%m/%d.md", &filetm);
    if (stat(filename, &fst)) {
        perror("stat");
        exit(E_NON_EXIST);
    }

    char cmd[100];
    strncpy(cmd, "cat", 100);
    strncat(cmd, filename, 100);
    system(cmd);
}





int parse_date_string(const char* date, struct tm *tm) {
    localtime_r(time(0), tm); // now
    if (date == NULL) return 0;

    

    return 0;
}

int cdiary() {
    const char* repository = getenv("DIARY");
    if (repository) {
        return chdir(repository);
    }

    return E_NON_EXIST;
}