#ifndef PLAYER_H
#define PLAYER_H

#include "map.h"

typedef struct {
    int x, y;
    int hp;
    int max_hp;
    int melee_damage;
    char symbol;
} Player;

void init_player(Player *p, int y, int x);
void move_player(Player *p, int input, Map *m);
void draw_player(Player *p);

#endif