#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "combat.h"
#include "cam.h"

#define MAX_ENEMIES 50

void handle_resize(int sig) {
    int height, width;
    getmaxyx(stdscr, height, width);
    VIEW_HEIGHT = height - 1;
    VIEW_WIDTH = width / 2;
    (void)sig;
}

int main() {
    signal(SIGWINCH, handle_resize);
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    timeout(50);

    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_BLACK, COLOR_WHITE);
    init_pair(5, COLOR_YELLOW, COLOR_BLACK);
    init_pair(6, COLOR_RED, COLOR_BLACK);
    init_pair(7, COLOR_GREEN, COLOR_BLACK);
    init_pair(8, COLOR_MAGENTA, COLOR_BLACK);

    int term_height, term_width;
    getmaxyx(stdscr, term_height, term_width);
    VIEW_HEIGHT = term_height - 2;  // Leave room for instructions at bottom
    VIEW_WIDTH = term_width / 2;

    srand(time(NULL));

    Map myMap;
    Player myPlayer;
    init_map(&myMap);

    // mark starting room as explored
    myMap.explored[0] = 1;

    int level = 1;
    char message[100] = "";
    int message_timer = 0;

    int move_cooldown = 0;

    int enemy_count = 12 + level * 2;
    if (enemy_count > MAX_ENEMIES) enemy_count = MAX_ENEMIES;
    enemy enemies[MAX_ENEMIES];

    int start_y = myMap.rooms[0].y + (myMap.rooms[0].h / 2);
    int start_x = myMap.rooms[0].x + (myMap.rooms[0].w / 2);
    init_player(&myPlayer, start_y, start_x);

    for(int i = 0; i < enemy_count; i++) {
        int r = rand() % myMap.room_count;
        int ey = myMap.rooms[r].y + (rand() % myMap.rooms[r].h);
        int ex = myMap.rooms[r].x + (rand() % myMap.rooms[r].w);
        init_enemy(&enemies[i], ey, ex);
    }

    int ch;
    while (1) {
        ch = getch();
        if (move_cooldown > 0) move_cooldown--;

        if (ch == 'q' || ch == 'Q') break;
        
        if (ch == ' ') {
            player_melee_attack(&myPlayer, enemies, enemy_count);
        } else if (ch != ERR && move_cooldown == 0) { 
            move_player(&myPlayer, ch, &myMap);
            move_cooldown = 5;
        }

        // check if player collects the key
        if (myPlayer.x == myMap.key_x && myPlayer.y == myMap.key_y && !myPlayer.has_key) {
            myPlayer.has_key = 1;
            myMap.key_collected = 1;
            strcpy(message, "Key collected!");
            message_timer = 160;  // 8 seconds at 50ms per frame (50ms * 160 = 8000ms = 8s)
        }

        // update message timer
        if (message_timer > 0) {
            message_timer--;
            if (message_timer == 0) {
                message[0] = '\0';  // clear message when timer expires
            }
        }

        // check if player entered a new room
        for (int r = 0; r < myMap.room_count; r++) {
            Room room = myMap.rooms[r];
            if (myPlayer.x >= room.x && myPlayer.x < room.x + room.w &&
                myPlayer.y >= room.y && myPlayer.y < room.y + room.h) {
                myMap.explored[r] = 1;
                break;
            }
        }

        int on_door = (myPlayer.y == myMap.door_y && myPlayer.x == myMap.door_x);

        if (on_door) {
            if (!myPlayer.has_key) {
                strcpy(message, "You need the KEY to enter!");
                message_timer = 40;  // 2 seconds for immediate feedback
            } else {
                level++;
                strcpy(message, "Entering next level...");
                message_timer = 80;  // 4 seconds for level transition
                enemy_count = 12 + level * 2;
                if (enemy_count > MAX_ENEMIES) enemy_count = MAX_ENEMIES;

                init_map(&myMap);

                // mark starting room as explored
                myMap.explored[0] = 1;

                start_y = myMap.rooms[0].y + (myMap.rooms[0].h / 2);
                start_x = myMap.rooms[0].x + (myMap.rooms[0].w / 2);
                
                // preserve player level and xp, reset position, hp, and key
                int saved_level = myPlayer.level;
                int saved_xp = myPlayer.xp;
                int saved_xp_cap = myPlayer.xp_cap;
                init_player(&myPlayer, start_y, start_x);
                myPlayer.level = saved_level;
                myPlayer.xp = saved_xp;
                myPlayer.xp_cap = saved_xp_cap;

                for(int i = 0; i < enemy_count; i++) {
                    int r = rand() % myMap.room_count;
                    int ey = myMap.rooms[r].y + (rand() % myMap.rooms[r].h);
                    int ex = myMap.rooms[r].x + (rand() % myMap.rooms[r].w);
                    init_enemy(&enemies[i], ey, ex);
                }
            }
        } else {
            if (myPlayer.has_key == 0) {
                message[0] = '\0';
            }
        }

        if (myPlayer.hp <= 0) {
            strcpy(message, "Game Over! You died.");
            break;
        }

        erase();
        draw_map(&myMap);
        
        for(int i = 0;i < enemy_count; i++) {
            draw_enemy(&enemies[i], &myMap);
        }
        
        draw_player(&myPlayer);

        // Top left status panel
        int health_color = (myPlayer.hp > 50) ? 7 : ((myPlayer.hp > 25) ? 5 : 6); // Green > Yellow > Red
        attron(COLOR_PAIR(health_color) | A_BOLD);
        mvprintw(0, 0, "HP: %d", myPlayer.hp);
        attroff(COLOR_PAIR(health_color) | A_BOLD);

        attron(COLOR_PAIR(5)); // Yellow for XP
        mvprintw(0, 10, "XP: %d/%d", myPlayer.xp, myPlayer.xp_cap);
        attroff(COLOR_PAIR(5));

        attron(COLOR_PAIR(3) | A_BOLD); // White bold for player level
        mvprintw(0, 25, "Player: %d", myPlayer.level);
        attroff(COLOR_PAIR(3) | A_BOLD);

        // Top right status panel
        attron(COLOR_PAIR(2) | A_BOLD); // Blue bold for dungeon level
        mvprintw(0, term_width - 30, "Dungeon: %d", level);
        attroff(COLOR_PAIR(2) | A_BOLD);

        attron(COLOR_PAIR(5)); // Yellow for key status
        mvprintw(0, term_width - 18, "Key: %s", myPlayer.has_key ? "YES" : "NO");
        attroff(COLOR_PAIR(5));

        // Message in center top (if any)
        if (strlen(message) > 0) {
            attron(COLOR_PAIR(1) | A_BOLD); // Cyan bold for messages
            int msg_x = (term_width - strlen(message)) / 2;
            mvprintw(0, msg_x, "%s", message);
            attroff(COLOR_PAIR(1) | A_BOLD);
        }

        // Bottom control panel
        attron(COLOR_PAIR(3) | A_DIM); // Dim white for controls
        mvprintw(term_height - 1, 0, "WASD: Move | SPACE: Attack | Q: Quit");
        attroff(COLOR_PAIR(3) | A_DIM);

        refresh(); 
    }

    endwin();
    return 0;
}