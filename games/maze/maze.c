#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>

typedef struct x_s x_t;
typedef struct round_s round_t;

#define MX 60 // Width of the map
#define MY 15 // Heigth of the map


#define MAP_PATH "./map/"

static char* kLevel[] = { "00", "01", "02", NULL };

enum Color_t { CHuman, CRoad, CWall, CExit, };


FILE* glog;
static void log_init();
static void log_done();


struct x_s {
    int x;
    int y;
};

struct round_s {
    int level;      // current round of game
    char m[MY][MX];       // store the map
    x_t x;          // represent the human
};

static int map_loader(round_t* r);
static void map_running(round_t* r);
static void initcrt();


static void map_draw(round_t *r);
static int human_move(round_t *r, int key);
static void winner_draw();

int main(int argc, char *argv[]) {
    round_t r;
    log_init();

    initcrt();

    r.level = 0;
    if (argc > 1) {
        r.level = atoi(argv[1]);
    }
    
    while (0 == map_loader(&r)) {
        map_running(&r);
    }

    log_done();
    endwin();

    return 0;
}




int map_loader(round_t* r) {
    FILE* f;
#define LEN 20
    char mapname[LEN+1] = {0};
    strncat(mapname, MAP_PATH, LEN);
    strncat(mapname, kLevel[r->level], LEN);
    strncat(mapname, ".dat", LEN);
    f = fopen(mapname, "r");
    if (f == NULL) {
        fprintf(glog, "open %s fail\n", mapname);
        return 1;
    }
    fprintf(glog, "open %s success.\n", mapname);

    char obj;
    r->x.x = 0;
    r->x.y = 0;
    for (int y = 0; y < MY; y++) {
        for (int x = 0; x <= MX; x++) {
            fscanf(f, "%c", &obj);
            switch (obj) {
                case '0': r->m[y][x] = '0'; break;
                case '1': r->m[y][x] = '1'; break;
                case '2': r->m[y][x] = '2'; break;
                case 'X': {
                    r->m[y][x] = 'X'; 
                    if (r->x.x || r->x.y) {
                        return 2;
                    } 
                    r->x.x = x;
                    r->x.y = y;
                    break;
                }
            }
        }
    }

    fclose(f);

    return 0;
}

void map_running(round_t* r) {
    int key = 0;
    do {
        map_draw(r);

        if ((key = getch()) != ERR) {
            if (human_move(r, key) < 0) {
                winner_draw();
                break;
            }
        }

        usleep(1 * 1000);
    } while(true);
}

void initcrt() {
    initscr();
    start_color();
    noecho();
    nonl();
    nodelay(stdscr, true);
    curs_set(0);
	cbreak();
    keypad(stdscr, true);

    init_pair(CHuman, COLOR_GREEN, COLOR_BLACK);
    init_pair(CRoad, COLOR_WHITE, COLOR_BLACK);
    init_pair(CWall, COLOR_BLACK, COLOR_WHITE);
    init_pair(CExit, COLOR_RED, COLOR_RED);
}

void map_draw(round_t *r) {
    int by = (LINES - MY) / 2;
    int bx = (COLS - MX) / 2;
    for (int y = 0; y < MY; y++) {
        for (int x = 0; x < MX; x++) {
            switch (r->m[y][x]) {
            case '0': {
                attron(COLOR_PAIR(CRoad));
                mvaddch(y + by, x + bx, ' ');
                attroff(COLOR_PAIR(CRoad));
                break;
            }
            case '1': {
                attron(COLOR_PAIR(CWall));
                mvaddch(y + by, x + bx, ' ');
                attroff(COLOR_PAIR(CWall));
                break;
            }
            case '2': {
                attron(COLOR_PAIR(CExit) | A_BLINK);
                mvaddch(y + by, x + bx, ' ');
                attroff(COLOR_PAIR(CExit) | A_BLINK);
                break;
            }
            case 'x': 
            case 'X': {
                attron(COLOR_PAIR(CHuman));
                mvaddch(y + by, x + bx, 'X');
                attroff(COLOR_PAIR(CHuman));
                break;
            }
            default:
                break;
            }
        }
    }
    refresh();
}

#define swap(x, y) {  char t = (x); (x) = (y); (y) = t; }

int human_move(round_t *r, int key) {
    x_t *h = &(r->x);
    int x = -1, y = -1;
    switch (key) {
    case 'w':
    case 'k':
    case KEY_UP: {
        y = h->y - 1;
        x = h->x;
        break;
    }
    
    case 's':
    case 'j':
    case KEY_DOWN: {
        y = h->y + 1;
        x = h->x;
        break;
    }

    case 'a':
    case 'l':
    case KEY_LEFT: {
        y = h->y;
        x = h->x - 1;
        break;
    }

    case 'd':
    case 'h':
    case KEY_RIGHT: {
        y = h->y;
        x = h->x + 1;
        break;
    }
    default:
        return 0;
    }

    if (x == -1 || y == -1) return 0;

    switch (r->m[y][x]) {
    case '0': {
        swap(r->m[y][x], r->m[h->y][h->x]);
        h->y = y;
        h->x = x;
        return 1;
    }
    case '1': {
        return 0;
    }
    case '2': {
        r->m[h->y][h->x] = '0';
        r->m[y][x] = 'X';
        r->level++; // next round
        return -1;
    }
    default:
        break;
    }
    return 0;
}


void winner_draw() {
    clear();
    attron(A_BLINK);
    mvaddstr(LINES/2, COLS/2 - 3, "WINNER");
    attroff(A_BLINK);
    while (ERR == getch()) ;
    refresh();
}

void log_init() {
    glog = fopen("./log", "w");
    if (glog == NULL) {
        glog = stdout;
    }
}


void log_done() {
    fflush(glog);
    fclose(glog);
}