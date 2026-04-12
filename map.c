#include <ncurses.h>
#include <stdlib.h>
#include "map.h"

void init_map(Map *m) {
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            // 1. Unbreakable Bedrock Border
            if (y == 0 || y == WORLD_HEIGHT - 1 || x == 0 || x == WORLD_WIDTH - 1) {
                m->grid[y][x] = 'X';
            } 
            // 2. Random Breakable Walls (20% density)
            else if (rand() % 100 < 20) {
                m->grid[y][x] = '#';
            } 
            // 3. Floor
            else {
                m->grid[y][x] = '.';
            }
        }
    }
    // Clear start area
    m->grid[10][10] = '.'; m->grid[10][11] = '.'; m->grid[11][10] = '.';
}

void draw_map(Map *m, int playerY, int playerX) {
    int startY = playerY - (VIEW_HEIGHT / 2);
    int startX = playerX - (VIEW_WIDTH / 2);

    if (startY < 0) startY = 0;
    if (startX < 0) startX = 0;
    if (startY > WORLD_HEIGHT - VIEW_HEIGHT) startY = WORLD_HEIGHT - VIEW_HEIGHT;
    if (startX > WORLD_WIDTH - VIEW_WIDTH) startX = WORLD_WIDTH - VIEW_WIDTH;

    for (int y = 0; y < VIEW_HEIGHT; y++) {
        for (int x = 0; x < VIEW_WIDTH; x++) {
            char tile = m->grid[startY + y][startX + x];
            
            // Note: We use (x * 2) in all mvaddch calls for symmetry
            if (tile == 'X') {
                attron(COLOR_PAIR(3) | A_BOLD); // White Bedrock
                mvaddch(y + 1, x * 2, tile);
                attroff(COLOR_PAIR(3) | A_BOLD);
            } else if (tile == '#') {
                attron(COLOR_PAIR(2)); // Blue Walls
                mvaddch(y + 1, x * 2, tile);
                attroff(COLOR_PAIR(2));
            } else {
                attron(A_DIM); // Dim Floor dots
                mvaddch(y + 1, x * 2, '.');
                attroff(A_DIM);
            }
        }
    }
}