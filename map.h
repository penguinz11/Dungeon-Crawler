#ifndef MAP_H
#define MAP_H

#define WORLD_HEIGHT 100
#define WORLD_WIDTH 200

#define MAX_ROOMS 30
#define MIN_ROOM_SIZE 4

typedef struct {
    int x, y, w, h;
} Room;

typedef struct {
    char grid[WORLD_HEIGHT][WORLD_WIDTH];
    Room rooms[MAX_ROOMS];
    int room_count;
    int target_room;
    int door_y, door_x;
} Map;

void connect_points(Map *m, int x1, int y1, int x2, int y2);
void split_node(Map *m, int x, int y, int w, int h, int depth);
void init_map(Map *m);
void draw_map(Map *m);

#endif