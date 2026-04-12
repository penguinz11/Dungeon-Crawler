#include <ncurses.h>
#include "player.h"

void init_player(Player *p, int startY, int startX) {
    p->y = startY;
    p->x = startX;
    p->hp = 100;
    p->max_hp = 100;
    p->symbol = '@';
}

void move_player(Player *p, int input, Map *m) {
    int nextY = p->y;
    int nextX = p->x;

    switch (input) {
        case 'w': nextY--; break;
        case 's': nextY++; break;
        case 'a': nextX--; break;
        case 'd': nextX++; break;
    }

    // Collision check: only move if the tile is floor ('.') or an item
    if (nextY >= 0 && nextY < HEIGHT && nextX >= 0 && nextX < WIDTH) {
        if (m->grid[nextY][nextX] == '.') {
            p->y = nextY;
            p->x = nextX;
        }
    }
}

void player_attack(Player *p, Map *m) {
    // Look at all 8 squares around the player
    for (int y = p->y - 1; y <= p->y + 1; y++) {
        for (int x = p->x - 1; x <= p->x + 1; x++) {
            // Don't attack yourself
            if (y == p->y && x == p->x) continue;

            // In a real game, you'd check if an enemy is at m->grid[y][x]
            // For now, let's "flash" the area to show the attack happened
            mvaddch(y, x, '*'); 
        }
    }
    refresh();
    // Short delay so the user sees the "flash" of the attack
    napms(50); 
}

void draw_player(Player *p) {
    attron(COLOR_PAIR(1) | A_BOLD);
    mvaddch(p->y, p->x, p->symbol);
    attroff(COLOR_PAIR(1) | A_BOLD);
}