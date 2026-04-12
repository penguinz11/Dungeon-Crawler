#include <ncurses.h>
#include "map.h"

void init_map(Map *m) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            // Create a border of walls ('#') and a floor of dots ('.')
            if (y == 0 || y == HEIGHT - 1 || x == 0 || x == WIDTH - 1) {
                m->grid[y][x] = '#';
            } else {
                m->grid[y][x] = '.';
            }
        }
    }
}

void draw_map(Map *m) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (m->grid[y][x] == '#') {
                attron(COLOR_PAIR(2)); // Wall color
                mvaddch(y, x, m->grid[y][x]);
                attroff(COLOR_PAIR(2));
            } else {
                mvaddch(y, x, m->grid[y][x]);
            }
        }
    }
}