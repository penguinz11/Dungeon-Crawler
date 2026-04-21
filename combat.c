#include <ncurses.h>
#include "combat.h"
#include "enemy.h"
#include "player.h"
#include "cam.h"
#include <stdlib.h>

void player_melee_attack(Player *p, enemy *enemies, int enemy_count) {
    //hit nearby
    for(int i = 0; i < enemy_count;i++) {
        int dx = abs(enemies[i].x - p->x);
        int dy = abs(enemies[i].y - p->y);

        if(dx <= 1 && dy <= 1) {
            if(damage_enemy(&enemies[i], p->melee_damage) == 1) {
                gain_xp(p);
            }
        }
    }

    //flash effect
    for (int y = p->y - 1; y <= p->y + 1; y++) {
        for (int x = p->x - 1; x <= p->x + 1; x++) {
            attron(COLOR_PAIR(4)); 
            mvaddch(y - startY + 1, (x - startX) * 2, '*');
            attroff(COLOR_PAIR(4));
        }
    }
    refresh();
    napms(50);
}

void try_to_damage_player(enemy *e, Player *p, int damage, Map *m) {
    (void)m;//suppress warning
    int dx = abs(p->x - e->x);
    int dy = abs(p->y - e->y);
    if(dx <= 1 && dy <= 1) {
        p->hp -= damage;
        if(p->hp <= 0) {
            //player death
        }
        e->attack_timer = 10;//show attack effect
    }
    e->attack_cooldown = 20;
    e->allowed_to_move = 1;
}