#include <ncurses.h>
#include "player.h"
#include "map.h"
#include "enemy.h"

int main() {
    // Standard setup...
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    start_color();

    // This tells getch() to only wait 50 milliseconds. 
    // If no key is pressed, it returns ERR and moves on.
    timeout(50); 

    // Use a dark grey or blue for a "dungeon" feel
    start_color();
    // Pair 1: Player (Cyan)
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    // Pair 2: Breakable Walls (Blue)
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    // Pair 3: Bedrock (White)
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    // Pair 4: The Attack Flash (Black on White)
    init_pair(4, COLOR_BLACK, COLOR_WHITE);

    Map myMap;
    Player myPlayer;
    init_map(&myMap);
    init_player(&myPlayer, 10, 10);

    int ch;
    while (1) {
        // 1. Get input (now non-blocking)
        ch = getch();

        // 2. Process logic immediately
        if (ch == 'q' || ch == 'Q') break;
        
        if (ch == ' ') {
            player_attack(&myPlayer, &myMap);
        } else if (ch != ERR) { 
            // Only move if an actual key was pressed (ignores the timeout ERR)
            move_player(&myPlayer, ch, &myMap);
        }

        // 3. Render (Do this every loop for smoothness)
        erase();
        draw_map(&myMap, myPlayer.y, myPlayer.x);
        draw_player(&myPlayer);
        
        attron(COLOR_PAIR(1));
        mvprintw(0, 0, "HP: %d | WASD to Move | SPACE to Attack | Q to Quit", myPlayer.hp);
        attroff(COLOR_PAIR(1));

        refresh(); 
    }

    endwin();
    return 0;
}