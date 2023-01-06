#include "diary.h"

#define TYPE_DAILY        'd'
#define TYPE_OBJECTIVES   'o'
#define TYPE_YEARLY       'y'

const char* optstring = "d:hi::ovy";
static char type = TYPE_DAILY;
static struct option log_options[] = {
    {"date",       required_argument, 0,    'd'},
    {"help",       no_argument,       0,    'h'},
    {"editor",     optional_argument, 0,    'i'},
    {"objectives", no_argument,       0,    'o'},
    {"version",    no_argument,       0,    'v'},
    {"yearly",     no_argument,       0,    'y'},
    {0,            0,                 0,     0 },
};

const char *editor = "cat";

int main(int argc, char *argv[]) {
    int c;
    int err = E_OK;
    time_t now = time(0);

    struct tm filetm;
    localtime_r(&now, &filetm); // now
    
    while (1) {
        int option_index = 0;
        c = getopt_long(argc, argv, optstring, log_options, &option_index);
        if (-1 == c) break;

        switch (c) {
            case 'd': {
                err = parse_date_string(optarg, &filetm);
                if (err) {
                    printf("parse %s fail\n", optarg);
                    return err;
                }
                break;
            }

            case 'h': help();
            case 'i': editor = optarg ? optarg : "vim"; break;
            case 'o': type = TYPE_OBJECTIVES; break;
            case 'v': version();
            case 'y': type = TYPE_YEARLY; break;
            default: break;
        }
    };

    
    const char* repository = getenv("DIARY");
    if (!repository) {
        printf("ENV DIARY doesn't exist\n");
        return E_NON_EXIST;
    }
    chdir(repository);

    char filename[50];
    strftime(filename, 50, "./%Y/", &filetm);
    switch (type) {
        char buf[50];
        case TYPE_DAILY: {
            strftime(buf, 50, "%m/%d.md", &filetm);
            strncat(filename, buf, 50);
            break;
        }
        case TYPE_OBJECTIVES: {
            strftime(buf, 50, "%m/objectives.md", &filetm);
            strncat(filename, buf, 50);
            break;
        }
        case TYPE_YEARLY: {
            strftime(buf, 50, "/objectives.md", &filetm);
            strncat(filename, buf, 50);
            break;
        }
    }
    struct stat fst;
    if (stat(filename, &fst)) {
        perror("stat");
        char buf[20];
        strftime(buf, 20, "%F", &filetm);
        printf("cannot found diary at %s in %s\n", buf, repository);
        return E_NON_EXIST;
    }

    int CMD_LEN = 100;
    char cmd[CMD_LEN];
    strncpy(cmd, editor, CMD_LEN);
    strncat(cmd, " ", CMD_LEN);
    strncat(cmd, filename, CMD_LEN);
    system(cmd);
    return err;
}





