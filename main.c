#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include "player.h"
#include "map.h"
#include "enemy.h"
#include "combat.h"

int main() {
    //standard setup
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    //this tells getch() to only wait 50 milliseconds. 
    //if no key is pressed, it returns ERR and moves on.
    timeout(50); 

    start_color();
    //pair 1: player (Cyan)
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    //pair 2: breakable walls (Blue)
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    //pair 3: bedrock (White)
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    //pair 4: the attack flash (Black on White)
    init_pair(4, COLOR_BLACK, COLOR_WHITE);

    srand(time(NULL));

    Map myMap;
    Player myPlayer;
    init_map(&myMap);

    //enemies
    int enemy_count = 12;
    enemy enemies[enemy_count];

    //spawn player in the first generated room
    int start_y = myMap.rooms[0].y + (myMap.rooms[0].h / 2);
    int start_x = myMap.rooms[0].x + (myMap.rooms[0].w / 2);
    init_player(&myPlayer, start_y, start_x);

    //do the same for enemies but pick random rooms
    for(int i = 0; i < enemy_count; i++) {
        int r = rand() % myMap.room_count;
        int ey = myMap.rooms[r].y + (rand() % myMap.rooms[r].h);
        int ex = myMap.rooms[r].x + (rand() % myMap.rooms[r].w);
        init_enemy(&enemies[i], ey, ex);
    }

    int ch;
    while (1) {
        // 1. Get input (now non-blocking)
        ch = getch();

        // 2. Process logic immediately
        if (ch == 'q' || ch == 'Q') break;
        
        if (ch == ' ') {
            player_melee_attack(&myPlayer, enemies, enemy_count);
        } else if (ch != ERR) { 
            // Only move if an actual key was pressed (ignores the timeout ERR)
            move_player(&myPlayer, ch, &myMap);
        }

        // 3. Render (Do this every loop for smoothness)
        erase();
        draw_map(&myMap);
        
        //draw enemies
        for(int i = 0;i < enemy_count; i++) {
            draw_enemy(&enemies[i]);
        }
        
        //draw player
        draw_player(&myPlayer);

        attron(COLOR_PAIR(1));
        mvprintw(0, 0, "HP: %d | xp = %d / %d | level = %d | WASD to Move | SPACE to MELEE Attack | Q to Quit", myPlayer.hp, myPlayer.xp, myPlayer.xp_cap, myPlayer.level);
        attroff(COLOR_PAIR(1));

        refresh(); 
    }

    endwin();
    return 0;
}