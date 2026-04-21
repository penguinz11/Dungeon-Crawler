#ifndef ENEMY_H
#define ENEMY_H

#include "map.h"
#include "player.h"

typedef struct enemy {
    int x, y;
    int hp;
    int max_hp;
    char symbol;
    int damage;
    int allowed_to_move;
    int attack_cooldown;
    int attack_timer;
} enemy;

void init_enemy(enemy *e, int y, int x);
void move_enemy(enemy *e, Player *p, Map *m, enemy *enemies, int enemy_count, int self_index);
int damage_enemy(enemy *e, int damage);
void draw_enemy(enemy *e, Map *m);
int is_enemy_at(enemy *enemies, int count, int x, int y, int self_index);

void spawn_enemies(int max_enemies, enemy *enemies, Map *m);

#endif