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

    int term_height, term_width;
    getmaxyx(stdscr, term_height, term_width);
    VIEW_HEIGHT = term_height - 1;
    VIEW_WIDTH = term_width / 2;

    srand(time(NULL));

    Map myMap;
    Player myPlayer;
    init_map(&myMap);

    int level = 1;
    char message[100] = "";

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

        int on_door = (myPlayer.y == myMap.door_y && myPlayer.x == myMap.door_x);

        if (on_door) {
            level++;
            strcpy(message, "Entering next level...");
            enemy_count = 12 + level * 2;
            if (enemy_count > MAX_ENEMIES) enemy_count = MAX_ENEMIES;

            init_map(&myMap);

            start_y = myMap.rooms[0].y + (myMap.rooms[0].h / 2);
            start_x = myMap.rooms[0].x + (myMap.rooms[0].w / 2);
            init_player(&myPlayer, start_y, start_x);

            for(int i = 0; i < enemy_count; i++) {
                int r = rand() % myMap.room_count;
                int ey = myMap.rooms[r].y + (rand() % myMap.rooms[r].h);
                int ex = myMap.rooms[r].x + (rand() % myMap.rooms[r].w);
                init_enemy(&enemies[i], ey, ex);
            }
        } else {
            message[0] = '\0';
        }

        if (myPlayer.hp <= 0) {
            strcpy(message, "Game Over! You died.");
            break;
        }

        erase();
        draw_map(&myMap);
        
        for(int i = 0;i < enemy_count; i++) {
            draw_enemy(&enemies[i]);
        }
        
        draw_player(&myPlayer);

        attron(COLOR_PAIR(1));
        mvprintw(0, 0, "HP: %d | xp = %d / %d | level = %d | Level: %d | %s | WASD to Move | SPACE to MELEE Attack | Q to Quit", myPlayer.hp, myPlayer.xp, myPlayer.xp_cap, myPlayer.level, level, message);
        attroff(COLOR_PAIR(1));

        refresh(); 
    }

    endwin();
    return 0;
}