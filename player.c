#include <ncurses.h>
#include "enemy.h"
#include "player.h"
#include "map.h"
#include "cam.h"

void init_player(Player *p, int y, int x) {
    p->y = y;
    p->x = x;
    p->max_hp = 100;
    p->hp = p->max_hp;
    p->melee_damage = 1;
    p->symbol = '@';
    p->xp = 0;
    p->xp_cap = 10;
    p->level = 1;
    p->has_key = 0;
    p->score = 0;
}

void move_player(Player *p, int input, Map *m) {
    int nextY = p->y;
    int nextX = p->x;

    switch (input) {
        case 'w': case 'W': nextY--; break;
        case 's': case 'S': nextY++; break;
        case 'a': case 'A': nextX--; break;
        case 'd': case 'D': nextX++; break;
    }

    if (nextY >= 0 && nextY < WORLD_HEIGHT && nextX >= 0 && nextX < WORLD_WIDTH) {
        if (m->grid[nextY][nextX] == '.' || m->grid[nextY][nextX] == 'R') {
            p->y = nextY;
            p->x = nextX;
            m->explored[p->y][p->x] = 1;
        }
    }
}

void draw_player(Player *p) {
    clamp_cam(p->y, p->x, WORLD_HEIGHT, WORLD_WIDTH);
    attron(COLOR_PAIR(1) | A_BOLD);
    mvaddch(p->y - startY + 1, (p->x - startX) * 2, p->symbol);
    attroff(COLOR_PAIR(1) | A_BOLD);
}

void gain_xp(Player *p) {
    p->xp++;

    if(p->xp >= p->xp_cap) {
        p->xp -= p->xp_cap;
        p->xp_cap+=2;
        p->level++;
        p->melee_damage++;
    }
}

void add_score(Player *p, int points) {
    p->score += points;
}