#include "diary.h"


const char* optstring = "d:ghi::ovy";
static struct option log_options[] = {
    {"date",       required_argument, 0,    'd'},
    {"help",       no_argument,       0,    'h'},
    {"editor",     optional_argument, 0,    'i'},
    {"generator",  no_argument,       0,    'g'},
    {"objectives", no_argument,       0,    'o'},
    {"version",    no_argument,       0,    'v'},
    {"yearly",     no_argument,       0,    'y'},
    {0,            0,                 0,     0 },
};

const char *editor = "cat";
struct tm date;
const Module *d;

int main(int argc, char *argv[]) {
    int c;
    int err = E_OK;
    time_t now = time(0);
    int generate = 0;

    localtime_r(&now, &date); // now

    d = &daily;
    
    while (1) {
        int option_index = 0;
        c = getopt_long(argc, argv, optstring, log_options, &option_index);
        if (-1 == c) break;

        switch (c) {
            case 'd': {
                err = parse_date_string(optarg, &date);
                if (err) {
                    printf("parse %s fail\n", optarg);
                    return err;
                }
                break;
            }
            case 'g': generate = 1; break;
            case 'h': help();
            case 'i': editor = optarg ? optarg : "vim"; break;
            case 'o': d = &monthly; break;
            case 'v': version();
            case 'y': d = &yearly; break;
            default: break;
        }
    };

    
    const char* repository = getenv("DIARY");
    if (!repository) {
        printf("ENV DIARY doesn't exist\n");
        return E_NON_EXIST;
    }
    chdir(repository);

    struct stat fst;
    if (stat(d->name(), &fst)) {
        if (!generate) {
            printf("cannot found diary at %s %s\n", repository, d->name());
            return E_NON_EXIST;
        }

        d->generator();
    }

    int CMD_LEN = 100;
    char cmd[CMD_LEN];
    strncpy(cmd, editor, CMD_LEN);
    strncat(cmd, " ", CMD_LEN);
    strncat(cmd, d->name(), CMD_LEN);
    system(cmd);
    return err;
}





