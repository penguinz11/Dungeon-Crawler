#ifndef PLAYER_H
#define PLAYER_H

#include "map.h"

typedef struct {
    int x, y;
    int hp;
    int max_hp;
    char symbol;
} Player;

void init_player(Player *p, int startY, int startX);
void move_player(Player *p, int input, Map *m);
void player_attack(Player *p, Map *m); // The "Spacebar" AOE attack
void draw_player(Player *p);

#endif