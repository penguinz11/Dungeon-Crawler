#ifndef MAP_H
#define MAP_H

#define HEIGHT 24
#define WIDTH 80

typedef struct {
    char grid[HEIGHT][WIDTH];
} Map;

void init_map(Map *m);
void draw_map(Map *m);

#endif