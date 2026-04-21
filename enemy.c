#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "enemy.h"
#include "cam.h"
#include "player.h"
#include "combat.h"

void init_enemy(enemy *e, int y, int x) {
    e->x = x;
    e->y = y;
    e->max_hp = 5;
    e->hp = e->max_hp;
    e->symbol = 'E';
    e->allowed_to_move = 1;
    e->damage = 10;
    e->attack_cooldown = 50;
    e->attack_timer = 0;
}

void draw_enemy(enemy *e, Map *m) {
    int ey = e->y - startY;
    int ex = e->x - startX;

    if(ex >= 0 && ex < VIEW_WIDTH && ey >= 0 && ey < VIEW_HEIGHT) {
        //check explored
        if (m->explored[e->y][e->x]) {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvaddch(ey + 1, (ex) * 2, e->symbol);
            attroff(COLOR_PAIR(1) | A_BOLD);
            
            //draw attack effect
            if (e->attack_timer > 0) {
                for(int i = -1; i <= 1; i++) {
                    for(int j = -1; j <= 1; j++) {
                        if (i == 0 && j == 0) continue;//skip self
                        int ny = e->y + i, nx = e->x + j;
                        if (ny >= 0 && ny < WORLD_HEIGHT && nx >= 0 && nx < WORLD_WIDTH && m->grid[ny][nx] != 'X') {
                            int ey2 = ny - startY, ex2 = nx - startX;
                            if (ex2 >= 0 && ex2 < VIEW_WIDTH && ey2 >= 0 && ey2 < VIEW_HEIGHT) {
                                mvaddch(ey2 + 1, (ex2) * 2, '^');
                            }
                        }
                    }
                }
            }
        }
    }
}

static int find_next_step_towards_player(enemy *e, Player *p, Map *m, enemy *enemies, int enemy_count, int self_index, int *out_dx, int *out_dy) {
    int start_y = e->y;
    int start_x = e->x;
    int target_y = p->y;
    int target_x = p->x;

    static short parent_y[WORLD_HEIGHT][WORLD_WIDTH];
    static short parent_x[WORLD_HEIGHT][WORLD_WIDTH];
    static unsigned char visited[WORLD_HEIGHT][WORLD_WIDTH];
    int queue_y[WORLD_HEIGHT * WORLD_WIDTH];
    int queue_x[WORLD_HEIGHT * WORLD_WIDTH];
    int front = 0, back = 0;

    memset(visited, 0, sizeof(visited));

    visited[start_y][start_x] = 1;
    parent_y[start_y][start_x] = -1;
    parent_x[start_y][start_x] = -1;
    queue_y[back] = start_y;
    queue_x[back] = start_x;
    back++;

    int dirs[4][2] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}};

    while (front < back) {
        int cy = queue_y[front];
        int cx = queue_x[front];
        front++;

        if (cy == target_y && cx == target_x) {
            int py = cy;
            int px = cx;
            while (parent_y[py][px] != -1 && parent_x[py][px] != -1) {
                int py2 = parent_y[py][px];
                int px2 = parent_x[py][px];
                if (py2 == start_y && px2 == start_x) {
                    *out_dy = py - start_y;
                    *out_dx = px - start_x;
                    return 1;
                }
                py = py2;
                px = px2;
            }
            return 0;
        }

        for (int i = 0; i < 4; i++) {
            int ny = cy + dirs[i][1];
            int nx = cx + dirs[i][0];

            if (ny < 0 || ny >= WORLD_HEIGHT || nx < 0 || nx >= WORLD_WIDTH) continue;
            if (visited[ny][nx]) continue;
            if (!m->explored[ny][nx] && !(ny == target_y && nx == target_x)) continue;
            if (!(m->grid[ny][nx] == '.' || m->grid[ny][nx] == 'R')) continue;
            if (ny == target_y && nx == target_x) {
                visited[ny][nx] = 1;
                parent_y[ny][nx] = cy;
                parent_x[ny][nx] = cx;
                queue_y[back] = ny;
                queue_x[back] = nx;
                back++;
                continue;
            }
            visited[ny][nx] = 1;
            parent_y[ny][nx] = cy;
            parent_x[ny][nx] = cx;
            queue_y[back] = ny;
            queue_x[back] = nx;
            back++;
        }
    }

    return 0;
}

void move_enemy(enemy *e, Player *p, Map *m, enemy *enemies, int enemy_count, int self_index) {
    if (e->hp <= 0) return;
    if (e->allowed_to_move == 0) {return;}
    
    //only move if enemy is in an explored area
    if (!m->explored[e->y][e->x]) return; //only chase if in explored area
    
    int dx = p->x - e->x;
    int dy = p->y - e->y;
    
    if(abs(dx) <= 1 && abs(dy) <= 1) {
        e->attack_cooldown = 20;  //shorter cooldown for fluidity
        e->allowed_to_move = 0;
        return;  //don't move when adjacent, attack instead
    }
    
    int out_dx = 0;
    int out_dy = 0;
    if (find_next_step_towards_player(e, p, m, enemies, enemy_count, self_index, &out_dx, &out_dy)) {
        if (out_dx != 0 || out_dy != 0) {
            e->x += out_dx;
            e->y += out_dy;
        }
    }
}

int damage_enemy(enemy *e, int damage) {
    e->hp -= damage;
    if(e->hp <= 0) {
        e->x = -1;
        e->y = -1;
        return 1;
    }
    return 0;
}

int is_enemy_at(enemy *enemies, int count, int x, int y, int self_index) {
    for(int i=0; i<count; i++) {
        if (i == self_index) continue;
        if(enemies[i].x == x && enemies[i].y == y)
            return 1;
    }
    return 0;
}

void spawn_enemies(int max_enemies, enemy *enemies, Map *m) {
    int k = 0;

    for (int i = 1; i < m->room_count; i++) {
        if (k >= max_enemies) break;

        Room *r = &m->rooms[i];

        for (int j = 0; j < 2; j++) {  //spawn 2 enemies per room
            if (k >= max_enemies) break;

            int x, y;
            int attempts = 0;
            do {
                x = r->x + 1 + rand() % (r->w - 2);
                y = r->y + 1 + rand() % (r->h - 2);
                attempts++;
            } while (((m->grid[y][x] != '.' && m->grid[y][x] != 'R') || is_enemy_at(enemies, k, x, y, -1)) && attempts < 20);

            if ((m->grid[y][x] == '.' || m->grid[y][x] == 'R') && !is_enemy_at(enemies, k, x, y, -1)) {
                init_enemy(&enemies[k], y, x);
                k++;
            }
        }
    }
}