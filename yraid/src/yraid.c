#include "yraid.h"


const char* optstring = "d:ghi::lovy";
static struct option log_options[] = {
    {"date",       required_argument, 0,    'd'},
    {"help",       no_argument,       0,    'h'},
    {"editor",     optional_argument, 0,    'i'},
    {"generator",  no_argument,       0,    'g'},
    {"list",       no_argument,       0,    'l'},
    {"objectives", no_argument,       0,    'o'},
    {"version",    no_argument,       0,    'v'},
    {"yearly",     no_argument,       0,    'y'},
    {0,            0,                 0,     0 },
};

const char *editor = "cat";
struct tm date;
const char* repository = NULL;
const Module *d;

int main(int argc, char *argv[]) {
    int c;
    int err = E_OK;
    time_t now = time(0);
    int generate = 0;

    /* set default time : now */
    localtime_r(&now, &date);
    d = &daily;

    /* change work directory under diary repository */
    repository = getenv("DIARY");
    if (!repository) {
        printf("ENV DIARY doesn't exist\n");
        return E_NON_EXIST;
    }
    chdir(repository);
    
    /* parse command options */
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
            case 'l': view();
            case 'o': d = &monthly; break;
            case 'v': version();
            case 'y': d = &yearly; break;
            default: break;
        }
    };

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


void template_suffix(FILE *f) {
    struct tm nowtm;
    time_t now = time(0);
    localtime_r(&now, &nowtm);

    const int n = 30;
    char line[n];
    fprintf(f, "\n-------------------\n\n");
    strftime(line, n, "%F %T", &nowtm);
    fprintf(f, "Date: *`%s`*\n", line);
}



