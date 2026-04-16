#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include "map.h"
#include "cam.h"

void connect_points(Map *m, int x1, int y1, int x2, int y2) {
    int curX = x1;
    int curY = y1;

    while (curX != x2) {
        m->grid[curY][curX] = '.';
        curX += (x2 > curX) ? 1 : -1;
    }
    while (curY != y2) {
        m->grid[curY][curX] = '.';
        curY += (y2 > curY) ? 1 : -1;
    }
}

void split_node(Map *m, int x, int y, int w, int h, int depth) {
    //if we've reached max depth or the area is too small, carve a room
    if (depth <= 0 || (w < MIN_ROOM_SIZE * 2 + 2 && h < MIN_ROOM_SIZE * 2 + 2)) {
        if (m->room_count < MAX_ROOMS) {
            //create a room smaller than the available partition (the "node")
            int rw = (rand() % (w - MIN_ROOM_SIZE)) + MIN_ROOM_SIZE - 2;
            int rh = (rand() % (h - MIN_ROOM_SIZE)) + MIN_ROOM_SIZE - 2;
            int rx = x + (rand() % (w - rw - 1)) + 1;
            int ry = y + (rand() % (h - rh - 1)) + 1;

            m->rooms[m->room_count] = (Room){rx, ry, rw, rh};

            //carve floor tiles
            for (int i = ry; i < ry + rh; i++) {
                for (int j = rx; j < rx + rw; j++) {
                    m->grid[i][j] = '.';
                }
            }

            //connect to the previous room
            if (m->room_count > 0) {
                Room prev = m->rooms[m->room_count - 1];
                connect_points(m, rx + rw/2, ry + rh/2, prev.x + prev.w/2, prev.y + prev.h/2);
            }

            m->room_count++;
        }
        return;
    }

    //decide split direction
    int split_horiz = rand() % 2;
    if (w > h * 1.5) split_horiz = 0; //force vertical if too wide
    else if (h > w * 1.5) split_horiz = 1; //force horizontal if too tall

    if (split_horiz) {
        int split = (h / 2); //simple middle split, can be randomized
        split_node(m, x, y, w, split, depth - 1);
        split_node(m, x, y + split, w, h - split, depth - 1);
    } else {
        int split = (w / 2);
        split_node(m, x, y, split, h, depth - 1);
        split_node(m, x + split, y, w - split, h, depth - 1);
    }
}

void init_map(Map *m) {
    m->room_count = 0;
    //fill with walls first
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            m->grid[y][x] = 'X';
        }
    }
    //start splitting the whole map (depth of 4 gives up to 16 partitions)
    split_node(m, 0, 0, WORLD_WIDTH, WORLD_HEIGHT, 4);
}

void draw_map(Map *m) {
    for (int y = 0; y < VIEW_HEIGHT; y++) {
        for (int x = 0; x < VIEW_WIDTH; x++) {
            //calculate coordinates in the actual world grid
            int worldY = startY + y;
            int worldX = startX + x;

            //boundary check: only draw if within world limits
            if (worldY >= 0 && worldY < WORLD_HEIGHT && worldX >= 0 && worldX < WORLD_WIDTH) {
                char tile = m->grid[worldY][worldX];

                if (tile == 'X') {
                    //draw walls (bedrock/stone)
                    attron(COLOR_PAIR(3) | A_BOLD); 
                    mvaddch(y + 1, x * 2, tile);
                    attroff(COLOR_PAIR(3) | A_BOLD);
                } else {
                    //draw floor dots
                    attron(A_DIM);
                    mvaddch(y + 1, x * 2, '.');
                    attroff(A_DIM);
                }
            } else {
                //draw empty space if the camera looks outside the world map
                mvaddch(y + 1, x * 2, ' ');
            }
        }
    }
}