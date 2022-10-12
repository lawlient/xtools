#ifndef DRAW_H
#define DRAW_H

typedef enum DrawMode_ {
    TEXT = 0,
    BIGTXT,
    DRAWMODESIZE,
} DrawMode;

#define DEFALUT_DRAW_MODE BIGTXT

typedef enum Color_ {
    CUSTOM, 
    RED,
    GREEN,
    CYAN,
    BLUE,
    COLORSIZE,
} Color;

typedef enum CTheme_ {
    T_DEFAULT,
    T_BG,
} CTheme;
extern const int COLOR_THEME[][COLORSIZE];

#endif