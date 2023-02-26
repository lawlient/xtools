#include "diary.h"



static char filename[50];

static char* name() {
    strftime(filename, 50, "./%Y/%m/objectives.md", &date);
    return filename;
}

static void checkdir() {
    char dir[50];
    strftime(dir, 50, "./%Y/%m", &date);
    if (access(dir, 0700)) {
        mkdir(dir, 0700);
    }
}

static void template() {
    checkdir();
    FILE* f;
    f = fopen(name(), "w");
    if (f == NULL) exit(E_CREAT_FAIL);

    int n = 120;
    char line[n];
    strftime(line, n, "%Y年%m月目标", &date);
    fprintf(f, "# 🎯 %s\n\n", line);
    fprintf(f, "## 👷 工作\n\n");
    fprintf(f, "## 📖 阅读\n\n");
    fprintf(f, "## 🔨 技能\n\n");
    fprintf(f, "## 👪 生活\n\n");

    template_suffix(f);
    fclose(f);
}

const Module monthly = {
    .name = name,
    .generator = template,
};
