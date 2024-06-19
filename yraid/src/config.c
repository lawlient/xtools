#include "yraid.h"

#define TOKENLEN 128

static const char* CONFIG_KEYS[] = { CFG_REPOSITORY, CFG_BIRTHDAY, CFG_EDITOR, NULL };
static char CONFIG_VALS[sizeof(CONFIG_KEYS)/ sizeof(CONFIG_KEYS[0])][TOKENLEN] = {0};


typedef struct Token_ {
    char *s;
    char *e;
} Token;
#define IS_INVALID_TOKEN(t) (!(t).s)

static char *readline(int fd);
static Token getToken(char* line, int idx);
static const char* pathname();
static const char* filename();
static int open_config_file();
static void print_config();
static void save_config();
static int set_config(const char* key , const char* val);


/* read config file and parse to struct */
void parse_config() {
    int fd = open_config_file();
    if (fd <= 0) exit(errno);

    char *line = 0;
    while ((line = readline(fd)) != NULL) {
        Token key, eq, val;

        key = getToken(line, 0);
        if (IS_INVALID_TOKEN(key)) continue;

        eq  = getToken(line, key.e - line);
        if (eq.s + 1 != eq.e || *eq.s != '=') continue;

        val = getToken(line, eq.e - line);
        if (IS_INVALID_TOKEN(val)) continue;

        *key.e = 0;
        *val.e = 0;
        set_config(key.s, val.s);
    }

    close(fd);
}

const char* get_config(const char* key) {
    for (int i = 0; CONFIG_KEYS[i]; i++) {
        if (!strncmp(CONFIG_KEYS[i], key, strlen(key))) {
            return CONFIG_VALS[i];
        }
    }
    return NULL;
}

void config(int argc, char *argv[]) {
    parse_config();

    if (argc == 0) print_config();
    if (argc < 2) exit(0);

    const char* key = argv[0];
    const char* val = argv[1];

    if (set_config(key, val)) {
        printf("set key:%s fail\n", key);
        printf("try `%s -c` to check all keys\n", PROC);
        exit(0);
    }

    save_config();
    exit(0);
}

const char* age() {
    static char AGE[5] = {0};
    const char *birth = get_config(CFG_BIRTHDAY);
    if (!birth) return "";
    struct tm birthday;
    memset(&birthday, 0, sizeof(birthday));
    if (!strptime(birth, "%Y-%m-%d", &birthday)) {
        return "";
    }
    if (birthday.tm_year > date.tm_year) return "";
    int age = date.tm_year - birthday.tm_year;
    if (date.tm_mon > birthday.tm_mon ||
        (date.tm_mon == birthday.tm_mon && date.tm_mday >= birthday.tm_mday)) {
        age++;
    }
    if (!age) return "";
    memset(AGE, 0, sizeof(AGE)/sizeof(AGE[0]));
    snprintf(AGE, sizeof(AGE)/sizeof(AGE[0]), "%d", age);
    return AGE;
}


///  -----------------------

/* 
    This file is responsible for read & write config file.
    The rule of config:
      1. construct of comments and a list of key/value;
      2. character after '#' (include '#') at the same line will be ignored;
      3. each pair of key/value is exclusive line;
      4. line format is `key=value`
*/

const char* pathname() {
    static char path[128];
    memset(path, 0, strlen(path));
    snprintf(path, 128, "%s/.config/%s", ENV_HOME, PROC);
    return path;
}

const char* filename() {
    static char name[128];
    memset(name, 0, strlen(name));
    snprintf(name, 128, "%s/config", pathname());
    return name;
}

int open_config_file() {
    int fd = open(filename(), O_RDWR | O_CREAT, 0644);
    if (fd > 0) return fd;

    if (ENOENT == errno) {
        if (mkdir(pathname(), 0755)) {
            printf("create config path: %s fail", pathname());
            return fd;
        }
        fd = open(filename(), O_RDWR | O_CREAT, 0644);
    }
    if (fd <= 0) {
        printf("open config file:%s fail. fd: %d\n", filename(), fd);
        printf("use `%s -c key value` to set config first\n", PROC);
    }
    return fd;
}

int set_config(const char* key , const char* val) {
    for (int i = 0; CONFIG_KEYS[i]; i++) {
        if (!strncmp(CONFIG_KEYS[i], key, strlen(key))) {
            strncpy(CONFIG_VALS[i], val, strlen(val));
            return 0;
        }
    }
    return -1;
}

void print_config() {
    for (int i = 0; CONFIG_KEYS[i]; i++) {
        printf("%s=%s\n", CONFIG_KEYS[i], CONFIG_VALS[i]);
    }
}

void save_config() {
    int fd = open(filename(), O_WRONLY | O_TRUNC);
    if (fd <= 0) {
        printf("%d: %s\n", errno, strerror(errno));
        exit(errno);
    }
    dprintf(fd, "# This file is generated, you would better not to edit it.\n");
    for (int i = 0; CONFIG_KEYS[i]; i++) {
        dprintf(fd, "%s=%s\n", CONFIG_KEYS[i], CONFIG_VALS[i]);
    }
    fsync(fd);
    close(fd);
}

char *readline(int fd) {
    static char line[1024];
    memset(line, 0, 1024);
    char *p = line;
    char c;
    int size = 0;
    while ((size = read(fd, &c, 1)) > 0) {
        if (c == '\n') {
            *p = 0;
            break;
        }
        *p++ = c;
    }
    return p == line ? NULL : line;
}

Token getToken(char* line, int idx) {
    // printf("idx:%d line:%s\n", idx, line);
    Token token = {0};
    char* p = line + idx;
    while (*p) {
        if (*p == ' ') {
            if (token.s) {
                token.e = p;
                return token;
            }
            p++;
            continue;
        }
        if (*p == '=') {
            if (token.s) {
                token.e = p;
            } else {
                token.s = p++;
                token.e = p;
            }
            return token;
        }
        if (*p == '#') {
            if (token.s) token.e = p;
            return token;
        }
        if (!token.s) token.s = p;
        p++;
    }
    if (token.s) {
        token.e = p;
    }
    return token;
}
