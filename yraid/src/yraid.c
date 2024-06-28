#include "yraid.h"


const char* optstring = "c::d:e::ghlmt::vy";
static struct option log_options[] = {
    {"config",     optional_argument, 0,    'c'},
    {"date",       required_argument, 0,    'd'},
    {"editor",     optional_argument, 0,    'e'},
    {"help",       no_argument,       0,    'h'},
    {"generator",  no_argument,       0,    'g'},
    {"list",       no_argument,       0,    'l'},
    {"monthly",    no_argument,       0,    'm'},
    {"template",   optional_argument, 0,    't'},
    {"version",    no_argument,       0,    'v'},
    {"yearly",     no_argument,       0,    'y'},
    {0,            0,                 0,     0 },
};

const char *editor = "cat";
struct tm date;

int main(int argc, char *argv[]) {
    int c;
    int err = E_OK;
    int generate = 0;

    /* set default time : now */
    time_t now = time(0);
    localtime_r(&now, &date);

    /* parse command options */
    while (1) {
        int option_index = 0;
        c = getopt_long(argc, argv, optstring, log_options, &option_index);
        if (-1 == c) {
            printf("Try '%s -h' for help\n", PROC);
            return EPERM;
        }

        switch (c) {
            case 'c': config(argc-2, &argv[2]);
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
            case 'e': editor = optarg ? optarg : get_config(CFG_EDITOR); break;
            case 'l': view();
            case 'm': t = &monthly; break;
            case 't': template(argc-2, &argv[2]); break;
            case 'v': version();
            case 'y': t = &yearly; break;
            default: break;
        }
    };

    parse_config();
    workdir(1);

    if (generate) {
        if (t->generate(t, &date)) {
            return ENONET;
        }
    }

    if (editor == NULL) {
        printf("use `%s -c editor xxx` to config your default editor\n", PROC);
        printf("or try '%s -e xxx' to specific one off editor\n", PROC);
        return 0;
    }

    int CMD_LEN = 100;
    char cmd[CMD_LEN];
    strncpy(cmd, editor, CMD_LEN);
    strncat(cmd, " ", CMD_LEN);
    strncat(cmd, t->targetname(&date), CMD_LEN);
    system(cmd);
    return err;
}

void workdir(int exited) {
    /* change work directory under repository */
    const char* repository = get_config(CFG_REPOSITORY);
    if (!repository) {
        printf("you should set data path first\n");
        printf("try: %s -c set repository `your data path`\n", getenv("_"));
        if (exited) exit(ENONET);
    }
    if (chdir(repository)) {
        printf("data path: %s error\n", repository);
        printf("try: %s -c set repository `your data path`\n", getenv("_"));
        if (exited) exit(ENONET);
    }
}
