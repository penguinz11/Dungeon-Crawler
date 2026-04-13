#ifndef ENEMY_H
#define ENEMY_H

#include "player.h"

typedef struct {
    int x, y;
    int hp;
    int max_hp;
    char symbol;
} enemy;

void init_enemy(enemy *e, int y, int x);
void move_enemy(enemy *e);
void enemy_attack(enemy *e, Player *p);
void draw_enemy(enemy *e);

#endif