#include <ncurses.h>
#include <stdlib.h>
#include "enemy.h"
#include "cam.h"

void init_enemy(enemy *e, int y, int x) {
    e->x = x;
    e->y = y;
    e->hp = 5;
    e->max_hp = 5;
    e->symbol = 'E';
}

void draw_enemy(enemy *e) {
    int ey = e->y - startY;
    int ex = e->x - startX;

    if(ex >= 0 && ex < VIEW_WIDTH && ey >= 0 && ey < VIEW_HEIGHT) {
        attron(COLOR_PAIR(1) | A_BOLD);
        mvaddch(ey + 1, (ex) * 2, e->symbol);
        attroff(COLOR_PAIR(1) | A_BOLD);
    }

}