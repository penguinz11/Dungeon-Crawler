#include <ncurses.h>
#include "combat.h"
#include "enemy.h"
#include "player.h"
#include "cam.h"
#include <stdlib.h>

void player_melee_attack(Player *p, enemy *enemies, int enemy_count) {
    //check the enemies that are in range of the player
    for(int i = 0; i < enemy_count;i++) {
        int dx = abs(enemies[i].x - p->x);
        int dy = abs(enemies[i].y - p->y);

        if(dx <= 1 && dy <= 1) {
            if(damage_enemy(&enemies[i], p->melee_damage) == 1) {
                gain_xp(p);
            }
        }
    }

    //visual flash
    for (int y = p->y - 1; y <= p->y + 1; y++) {
        for (int x = p->x - 1; x <= p->x + 1; x++) {
            attron(COLOR_PAIR(4)); 
            mvaddch(y - startY + 1, (x - startX) * 2, '*');
            attroff(COLOR_PAIR(4));
        }
    }
    refresh();
    napms(50); // Small delay so the flash is visible
}