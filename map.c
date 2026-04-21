#include <stdlib.h>
#include <math.h>
#include <ncurses.h>
#include <string.h>
#include "map.h"
#include "cam.h"

void connect_points(Map *m, int x1, int y1, int x2, int y2) {
    int curX = x1;
    int curY = y1;

    while (curX != x2) {
        if (m->grid[curY][curX] == 'X') {
            m->grid[curY][curX] = '.';
        }
        curX += (x2 > curX) ? 1 : -1;
    }
    while (curY != y2) {
        if (m->grid[curY][curX] == 'X') {
            m->grid[curY][curX] = '.';
        }
        curY += (y2 > curY) ? 1 : -1;
    }
}

void split_node(Map *m, int x, int y, int w, int h, int depth) {
    if (depth <= 0 || (w < MIN_ROOM_SIZE * 2 + 2 && h < MIN_ROOM_SIZE * 2 + 2)) {
        if (m->room_count < MAX_ROOMS) {
            int rw = (rand() % (w - MIN_ROOM_SIZE)) + MIN_ROOM_SIZE - 2;
            int rh = (rand() % (h - MIN_ROOM_SIZE)) + MIN_ROOM_SIZE - 2;
            int rx = x + (rand() % (w - rw - 1)) + 1;
            int ry = y + (rand() % (h - rh - 1)) + 1;

            m->rooms[m->room_count] = (Room){rx, ry, rw, rh, 0};

            for (int i = ry; i < ry + rh; i++) {
                for (int j = rx; j < rx + rw; j++) {
                    m->grid[i][j] = 'R';//room floor
                }
            }

            if (m->room_count > 0) {
                Room prev = m->rooms[m->room_count - 1];
                connect_points(m, rx + rw/2, ry + rh/2, prev.x + prev.w/2, prev.y + prev.h/2);
            }

            m->room_count++;
        }
        return;
    }

    int split_horiz = rand() % 2;
    if (w > h * 1.5) split_horiz = 0;
    else if (h > w * 1.5) split_horiz = 1;

    if (split_horiz) {
        int split = (h / 2);
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
    for (int y = 0; y < WORLD_HEIGHT; y++) {
        for (int x = 0; x < WORLD_WIDTH; x++) {
            m->grid[y][x] = 'X';
        }
    }
    memset(m->explored, 0, sizeof(m->explored));
    m->key_collected = 0;
    split_node(m, 0, 0, WORLD_WIDTH, WORLD_HEIGHT, 4);

    int max_dist = 0;
    m->target_room = 0;
    if (m->room_count > 1) {
        Room start = m->rooms[0];
        for (int i = 1; i < m->room_count; i++) {
            Room r = m->rooms[i];
            int dx = (r.x + r.w / 2) - (start.x + start.w / 2);
            int dy = (r.y + r.h / 2) - (start.y + start.h / 2);
            int dist = dx * dx + dy * dy;
            if (dist > max_dist) {
                max_dist = dist;
                m->target_room = i;
            }
        }
    }

    //calculate distance from each room to target room
    Room target = m->rooms[m->target_room];
    int target_cx = target.x + target.w / 2;
    int target_cy = target.y + target.h / 2;
    int max_depth = WORLD_HEIGHT * WORLD_HEIGHT + WORLD_WIDTH * WORLD_WIDTH;

    for (int i = 0; i < m->room_count; i++) {
        Room r = m->rooms[i];
        int dx = (r.x + r.w / 2) - target_cx;
        int dy = (r.y + r.h / 2) - target_cy;
        int dist = dx * dx + dy * dy;
        float norm = (float)dist / max_depth;
        float inv = 1.0f - norm;
        m->rooms[i].depth = (int)(inv * 5) + 1;
    }

    //place door at farthest point in target room
    if (m->room_count > 0) {
        Room target = m->rooms[m->target_room];
        Room start = m->rooms[0];
        int start_cx = start.x + start.w / 2;
        int start_cy = start.y + start.h / 2;
        int max_dist = 0;
        int door_y = target.y + target.h / 2;
        int door_x = target.x + target.w / 2;
        for (int yy = target.y; yy < target.y + target.h; yy++) {
            for (int xx = target.x; xx < target.x + target.w; xx++) {
                int dx = xx - start_cx;
                int dy = yy - start_cy;
                int dist = dx * dx + dy * dy;
                if (dist > max_dist) {
                    max_dist = dist;
                    door_y = yy;
                    door_x = xx;
                }
            }
        }
        m->door_y = door_y;
        m->door_x = door_x;
        //dont store door on grid, only draw it
    }

    //spawn key in a random room away from player and door
    if (m->room_count > 2) {
        int key_room = -1;
        //try to find a room that's not the starting or door room
        for (int attempt = 0; attempt < 100; attempt++) {
            int r = rand() % m->room_count;
            if (r != 0 && r != m->target_room) {
                key_room = r;
                break;
            }
        }
        //if all random attempts were start or door room, find first valid one
        if (key_room == -1) {
            for (int r = 1; r < m->room_count; r++) {
                if (r != m->target_room) {
                    key_room = r;
                    break;
                }
            }
        }
        //spawn key at random position in the chosen room
        if (key_room != -1) {
            Room key_room_data = m->rooms[key_room];
            m->key_y = key_room_data.y + (rand() % key_room_data.h);
            m->key_x = key_room_data.x + (rand() % key_room_data.w);
        }
    }
}

void draw_map(Map *m) {
    for (int y = 0; y < VIEW_HEIGHT; y++) {
        for (int x = 0; x < VIEW_WIDTH; x++) {
            //actual world coordinates for this view tile
            int worldY = startY + y;
            int worldX = startX + x;

            //skip tiles outside the world
            if (worldY >= 0 && worldY < WORLD_HEIGHT && worldX >= 0 && worldX < WORLD_WIDTH) {
                char tile = m->grid[worldY][worldX];

                bool explored_tile = m->explored[worldY][worldX];

                //check for door first (highest priority)
                if (worldY == m->door_y && worldX == m->door_x && explored_tile) {
                    //door only visible in explored areas
                    attron(COLOR_PAIR(2) | A_BOLD);
                    mvaddch(y + 1, x * 2, 'D');
                    attroff(COLOR_PAIR(2) | A_BOLD);
                } else if (worldY == m->key_y && worldX == m->key_x && explored_tile && !m->key_collected) {
                    //key only visible in explored areas if not collected
                    attron(COLOR_PAIR(5));
                    mvaddch(y + 1, x * 2, 'K');
                    attroff(COLOR_PAIR(5));
                } else if (tile == '.' && explored_tile) {
                    //explored corridor
                    attron(A_DIM);
                    mvaddch(y + 1, x * 2, '.');
                    attroff(A_DIM);
                } else if (tile == 'R' && explored_tile) {
                    //explored room floor
                    attron(A_DIM);
                    mvaddch(y + 1, x * 2, '.');
                    attroff(A_DIM);
                } else if (tile == 'X') {
                    //wall
                    attron(COLOR_PAIR(3) | A_BOLD); 
                    mvaddch(y + 1, x * 2, tile);
                    attroff(COLOR_PAIR(3) | A_BOLD);
                } else if (explored_tile) {
                    //other explored tiles
                    attron(A_DIM);
                    mvaddch(y + 1, x * 2, tile);
                    attroff(A_DIM);
                } else {
                    //unexplored area
                    mvaddch(y + 1, x * 2, ' ');
                }
            } else {
                //blank outside the world
                mvaddch(y + 1, x * 2, ' ');
            }
        }
    }

    //draw map border
    attron(COLOR_PAIR(3) | A_BOLD);
    mvhline(1, 0, ACS_HLINE, VIEW_WIDTH * 2);
    mvhline(VIEW_HEIGHT, 0, ACS_HLINE, VIEW_WIDTH * 2);
    mvvline(1, 0, ACS_VLINE, VIEW_HEIGHT);
    mvvline(1, VIEW_WIDTH * 2 - 1, ACS_VLINE, VIEW_HEIGHT);
    mvaddch(1, 0, ACS_ULCORNER);
    mvaddch(1, VIEW_WIDTH * 2 - 1, ACS_URCORNER);
    mvaddch(VIEW_HEIGHT, 0, ACS_LLCORNER);
    mvaddch(VIEW_HEIGHT, VIEW_WIDTH * 2 - 1, ACS_LRCORNER);
    attroff(COLOR_PAIR(3) | A_BOLD);
}