#include "template.h"
#include <string.h>


Template *t = &daily;

static int readline(int fd, char* line, int len);
static int parseline(char *dsc, int dscsize, char *src, int srcsize);
static int command(char *cmd, char *dsc, int dscsize);

int generator(const char* templname, const char* targetname) {
    if (!targetname) return -1;

    struct stat st;
    if (!stat(targetname, &st)) {
        return 0; /* already exist */
    }

    int dscfd = open(targetname, O_RDWR | O_CREAT, 0664);
    if (dscfd <= 0 && ENOENT == errno) {
        char *tgtcpy = strdup(targetname);
        const char* dir= dirname(tgtcpy);
        if (mkdir(dir, 0755)) {
            printf("create dir:%s fail\n", dir);
            exit(errno);
        }
        free(tgtcpy);
        dscfd = open(targetname, O_RDWR | O_CREAT, 0664);
    }
    if (dscfd <= 0) return -1;

    int srcfd = open(templname, O_RDONLY);
    if (srcfd > 0) {
        char rline[1024];
        char wline[1024];
        int len = 0;
        while ((len = readline(srcfd, rline, 1024)) > 0) {
            parseline(wline, 1024, rline, 1024);
            write(dscfd, wline, strlen(wline));
        }

        close(srcfd);
    }

    close(dscfd);
    return 0;
}

static const char* templatefilename(Template* t) {
    static char name[FILENAME_LENGTH];
    const char* configpath = configfilepath();
    snprintf(name, FILENAME_LENGTH, "%s/template/%s", configpath, t->classname);
    return name;
}


int generate(Template *t, const struct tm* date) {
    const char* temp = templatefilename(t);
    const char* target = t->targetname(date);
    return t->generator(temp, target);
}


void template(int argc, char *argv[]) {
    parse_config();

    const char* editor = get_config(CFG_EDITOR);
    if (!editor) editor = "vi";

    if (argc < 1) exit(0);

    Template* t = NULL;
    if (!strcmp(argv[0], "daily")) t = &daily;
    if (!strcmp(argv[0], "monthly")) t = &monthly;
    if (!strcmp(argv[0], "yearly")) t = &yearly;

    struct stat dir;
    char tempdir[FILENAME_LENGTH];
    snprintf(tempdir, FILENAME_LENGTH, "%s/template", configfilepath());
    if (stat(tempdir, &dir)) {
        if (errno == ENOENT) {
            if (mkdir(tempdir, 0755)) {
                printf("create template dir:%s fail\n", tempdir);
                exit(errno);
            }
        }
    }

    int CMD_LEN = 128;
    char cmd[CMD_LEN];
    snprintf(cmd, CMD_LEN, "%s %s", editor, templatefilename(t));
    exit(system(cmd));
}



int readline(int fd, char* line, int len) {
    memset(line, 0, len);

    char c = 0;
    int size = 0;
    char *p = line;
    while ((size = read(fd, &c, 1)) > 0) {
        *p++ = c;
        if (c == '\n') break;
        if ((p - line) >= len) break;
    }
    return p - line;
}

int parseline(char *dsc, int dscsize, char *src, int srcsize) {
    memset(dsc, 0, dscsize);
    char *mark = NULL;
    char *w = dsc;
    char *p = src;

    while (*p) {
        if (*p != '$') {
            *w++ = *p++;
            continue;
        }

        if (*(p+1) != '{') {
            *w++ = *p++;
            continue;
        }

        mark = p++;
        while (*p != '}') p++;
        *p++ = 0;
        w += command(mark+2, w, dscsize - (w - dsc));
    }
    return 0;
}

int command(char *cmd, char *dsc, int dscsize) {
    char args[256];
    strncpy(args, cmd, strlen(cmd));
    int argc = 0;
    char *argv[15] = {0};

    char *p = strtok(args, " ");
    while (p) {
        argv[argc++] = p;
        p = strtok(NULL, " ");
    }
    if (!argc) return 0;
    
    char buf[BUFSIZ];
    int size = 0;

    if (!strcmp(argv[0], "day")) {
        const char* format = argc > 1 ? argv[1] : "%F %A";
        size = strftime(buf, BUFSIZ, format, &date);
        if (size > 0) {
            strncpy(dsc, buf, size);
            return size;
        }
    }

    if (!strcmp(argv[0], "timestamp")) {
        struct tm now;
        time_t ts = time(0);
        localtime_r(&ts, &now);
        const char *format = argc > 1 ? argv[1] : "%F %T";
        size = strftime(dsc, dscsize, format, &now);
        return size;
    }

    return 0;
}
