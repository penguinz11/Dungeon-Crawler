#include <ncurses.h>
#include "player.h"
#include "map.h"

int main() {
    // 1. Setup ncurses
    initscr();            // Start ncurses mode
    cbreak();             // Disable line buffering
    noecho();             // Don't show pressed keys on screen
    curs_set(0);          // Hide the blinking cursor
    keypad(stdscr, TRUE); // Enable special keys (like arrows)
    start_color();        // Enable color functionality

    // 2. Define Colors
    init_pair(1, COLOR_CYAN, COLOR_BLACK);  // Player color
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); // Wall color
    init_pair(3, COLOR_RED, COLOR_BLACK);    // Enemy color (for later)

    // 3. Initialize Game Objects
    Map myMap;
    Player myPlayer;
    
    init_map(&myMap);
    init_player(&myPlayer, 10, 10); // Start player at Y=10, X=10

    int ch;
    // 4. The Game Loop
    while ((ch = getch()) != 'q') { // Press 'q' to quit
        // Clear the screen for the new frame
        erase();

        // Handle Input
        if (ch == ' ') {
            player_attack(&myPlayer, &myMap);
        } else {
            move_player(&myPlayer, ch, &myMap);
        }

        // Draw Everything
        draw_map(&myMap);
        draw_player(&myPlayer);

        // UI Info
        mvprintw(0, 0, "HP: %d | WASD to Move | SPACE to Attack | Q to Quit", myPlayer.hp);

        // Refresh the physical screen
        refresh();
    }

    // 5. Clean up and Close
    endwin();
    return 0;
}