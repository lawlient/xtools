#include "yraid.h"



static char filename[50];

static char* name() {
    strftime(filename, 50, "./%Y/objectives.md", &date);
    return filename;
}

static void checkdir() {
    char dir[50];
    strftime(dir, 50, "./%Y", &date);
    if (access(dir, 0700)) {
        mkdir(dir, 0700);
    }
}

static void template() {
    checkdir();
    FILE* f;
    f = fopen(name(), "w");
    if (f == NULL) exit(E_CREAT_FAIL);

    int n = 120, size = 0;
    char line[n];
    size += strftime(line+size, n-size, "💯 %Y年度计划", &date);
    size += snprintf(line+size, n-size, " %s", zodiac());
    size += snprintf(line+size, n-size, " %d", AGE);
    fprintf(f, "# %s\n\n", line);
    fprintf(f, "🎯 核心目标\n\n");
    fprintf(f, "## 👷 工作 %s\n\n", "40%");
    fprintf(f, "## 📖 阅读 %s\n\n", "10%");
    fprintf(f, "## 🔨 技能 %s\n\n", "40%");
    fprintf(f, "## 👪 生活 %s\n\n", "10%");

    template_suffix(f);
    fclose(f);
}

const Module yearly = {
    .name = name,
    .generator = template,
};
