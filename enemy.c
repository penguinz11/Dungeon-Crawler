#include <ncurses.h>
#include <stdlib.h>
#include "enemy.h"
#include "cam.h"
#include "player.h"

void init_enemy(enemy *e, int y, int x) {
    e->x = x;
    e->y = y;
    e->max_hp = 5;
    e->hp = e->max_hp;
    e->symbol = 'E';
}

void draw_enemy(enemy *e, Map *m) {
    int ey = e->y - startY;
    int ex = e->x - startX;

    if(ex >= 0 && ex < VIEW_WIDTH && ey >= 0 && ey < VIEW_HEIGHT) {
        // check if enemy is in an explored room
        int in_explored_room = 0;
        for (int r = 0; r < m->room_count; r++) {
            Room room = m->rooms[r];
            if (e->x >= room.x && e->x < room.x + room.w &&
                e->y >= room.y && e->y < room.y + room.h) {
                if (m->explored[r]) {
                    in_explored_room = 1;
                }
                break;
            }
        }
        
        if (in_explored_room) {
            attron(COLOR_PAIR(1) | A_BOLD);
            mvaddch(ey + 1, (ex) * 2, e->symbol);
            attroff(COLOR_PAIR(1) | A_BOLD);
        }
    }
}

void move_enemy(enemy *e, Player *p, Map *m) {
    // Only move if enemy is alive
    if (e->hp <= 0) return;
    
    // Check if enemy is in an explored room - only chase if visible
    int in_explored_room = 0;
    for (int r = 0; r < m->room_count; r++) {
        Room room = m->rooms[r];
        if (e->x >= room.x && e->x < room.x + room.w &&
            e->y >= room.y && e->y < room.y + room.h) {
            if (m->explored[r]) {
                in_explored_room = 1;
            }
            break;
        }
    }
    
    if (!in_explored_room) return; // Don't move if not in explored area
    
    // Simple chasing: move towards player
    int dx = p->x - e->x;
    int dy = p->y - e->y;
    
    // Determine primary direction to move
    int move_x = 0, move_y = 0;
    
    if (abs(dx) > abs(dy)) {
        // Move horizontally towards player
        move_x = (dx > 0) ? 1 : -1;
    } else if (abs(dy) > abs(dx)) {
        // Move vertically towards player
        move_y = (dy > 0) ? 1 : -1;
    } else {
        // Equal distance, randomly choose direction
        if (rand() % 2) {
            move_x = (dx > 0) ? 1 : -1;
        } else {
            move_y = (dy > 0) ? 1 : -1;
        }
    }
    
    // Try to move
    int new_x = e->x + move_x;
    int new_y = e->y + move_y;
    
    // Check bounds and valid terrain
    if (new_x >= 0 && new_x < WORLD_WIDTH && new_y >= 0 && new_y < WORLD_HEIGHT) {
        if (m->grid[new_y][new_x] == '.' || m->grid[new_y][new_x] == 'R') {
            // Check if player is at the target position (don't move into player)
            if (!(new_x == p->x && new_y == p->y)) {
                e->x = new_x;
                e->y = new_y;
            }
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

int is_enemy_at(enemy *enemies, int count, int x, int y) {
    for(int i=0; i<count; i++)
    {
        if(enemies[i].x == x && enemies[i].y == y)
            return 1;
    }
    return 0;
}