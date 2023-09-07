#include "yraid.h"



static char filename[50];

static char* name() {
    strftime(filename, 50, "./%Y/%m/%d.md", &date);
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
    if (f == NULL) {
        printf("open %s fail\n", name());
        exit(E_CREAT_FAIL);
    }

    int n = 120;
    char line[n];
    strftime(line, n, "%F %A", &date);
    fprintf(f, "# 📅 %s\n\n", line);
    fprintf(f, "## 🎏 今日事件\n\n");
    fprintf(f, "### 🎯 计划\n\n");
    fprintf(f, "无\n\n");
    fprintf(f, "### 💀 变化\n\n");
    fprintf(f, "无\n\n");
    fprintf(f, "## 📕 日记\n\n");
    fprintf(f, "无\n\n");

    template_suffix(f);
    fclose(f);
}


const Module daily = {
    .name = name,
    .generator = template,
};
