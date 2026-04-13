#include <ncurses.h>
#include "player.h"
#include "map.h"
#include "cam.h"

void init_player(Player *p, int y, int x) {
    p->y = y;
    p->x = x;
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

    if (nextY >= 0 && nextY < WORLD_HEIGHT && nextX >= 0 && nextX < WORLD_WIDTH) {
        // Only move if it's a floor tile (keeps us out of Walls and Bedrock)
        if (m->grid[nextY][nextX] == '.') {
            p->y = nextY;
            p->x = nextX;
        }
    }
}

void player_attack(Player *p, Map *m) {
    // 2. Loop through the 8 tiles around the player
    for (int y = p->y - 1; y <= p->y + 1; y++) {
        for (int x = p->x - 1; x <= p->x + 1; x++) {
            if (y == p->y && x == p->x) continue;

            if (y >= 0 && y < WORLD_HEIGHT && x >= 0 && x < WORLD_WIDTH) {
                // LOGIC: Break ONLY blue walls (#), ignore bedrock (X)
                if (m->grid[y][x] == '#') {
                    m->grid[y][x] = '.';
                }
            }

            // VISUALS: Draw the attack flash with symmetry (* 2)
            attron(COLOR_PAIR(4)); 
            mvaddch(y - startY + 1, (x - startX) * 2, '*');
            attroff(COLOR_PAIR(4));
        }
    }
    refresh();
    napms(50); // Small delay so the flash is visible
}

void draw_player(Player *p) {
    clamp_cam(p->y, p->x, WORLD_HEIGHT, WORLD_WIDTH);
    attron(COLOR_PAIR(1) | A_BOLD);
    // Note the symmetry: (x * 2)
    mvaddch(p->y - startY + 1, (p->x - startX) * 2, p->symbol);
    attroff(COLOR_PAIR(1) | A_BOLD);
}