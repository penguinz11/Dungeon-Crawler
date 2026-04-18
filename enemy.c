#include <ncurses.h>
#include <stdlib.h>
#include "enemy.h"
#include "cam.h"

void init_enemy(enemy *e, int y, int x) {
    e->x = x;
    e->y = y;
    e->max_hp = 5;
    e->hp = e->max_hp;
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

int damage_enemy(enemy *e, int damage) {
    e->hp -= damage;
    if(e->hp <= 0) {
        e->x = -1;
        e->y = -1;
        return 1;
    }
    return 0;
}

int is_enemy_at(enemy *enemies, int count, int x, int y) {
    for(int i=0; i<count; i++)
    {
        if(enemies[i].x == x && enemies[i].y == y)
            return 1;
    }
    return 0;
}