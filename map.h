#ifndef MAP_H
#define MAP_H

#define WORLD_HEIGHT 30
#define WORLD_WIDTH 80

#define VIEW_HEIGHT 20
#define VIEW_WIDTH 40  
typedef struct {
    char grid[WORLD_HEIGHT][WORLD_WIDTH];
} Map;

void init_map(Map *m);
void draw_map(Map *m, int playerY, int playerX);

#endif