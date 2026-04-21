#ifndef ENEMY_H
#define ENEMY_H

#include "map.h"
#include "player.h"

typedef struct enemy {
    int x, y;
    int hp;
    int max_hp;
    char symbol;
} enemy;

void init_enemy(enemy *e, int y, int x);
void move_enemy(enemy *e, Player *p, Map *m);
int damage_enemy(enemy *e, int damage);
void draw_enemy(enemy *e, Map *m);

#endif