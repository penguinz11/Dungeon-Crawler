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
#include "leaderboard.h"

#define MAX_ENEMIES 180

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
    VIEW_HEIGHT = term_height - 2;//room for bottom instructions
    VIEW_WIDTH = term_width / 2;

    srand(time(NULL));

    load_leaderboard();

    Map myMap;
    Player myPlayer;
    init_map(&myMap);

    //mark start room explored
    Room start_room = myMap.rooms[0];
    for(int yy = start_room.y; yy < start_room.y + start_room.h; yy++) {
        for(int xx = start_room.x; xx < start_room.x + start_room.w; xx++) {
            myMap.explored[yy][xx] = 1;
        }
    }

    int level = 1;
    char message[100] = "";
    int message_timer = 0;
    int game_over = 0;
    int paused = 0;

    int move_cooldown = 0;
    int enemy_move_cooldown = 0;
    int show_leaderboard = 0;
    int leaderboard_drawn = 0;

    enemy enemies[MAX_ENEMIES];

    int start_y = myMap.rooms[0].y + (myMap.rooms[0].h / 2);
    int start_x = myMap.rooms[0].x + (myMap.rooms[0].w / 2);
    init_player(&myPlayer, start_y, start_x);

    spawn_enemies(MAX_ENEMIES, enemies, &myMap);

    int ch;
    while (1) {
        timeout(game_over ? -1 : 50);
        ch = getch();
        
        if (show_leaderboard) {
            if (!leaderboard_drawn) {
                draw_leaderboard();
                leaderboard_drawn = 1;
            }
            if (ch == 'l' || ch == 'L') {
                show_leaderboard = 0;
                leaderboard_drawn = 0;
            } else if (ch == 'q' || ch == 'Q') {
                break;
            }
            continue;
        }
        
        if (game_over && (ch == 'l' || ch == 'L')) {
            show_leaderboard = 1;
            leaderboard_drawn = 0;
            continue;
        }
        
        if (ch == 'p' || ch == 'P') {
            paused = 1;
        }
        
        if (paused) {
            if (ch == 'r' || ch == 'R') {
                paused = 0;
            }
            continue;
        }
        
        if (!game_over) {
            if (enemy_move_cooldown > 0) enemy_move_cooldown--;
            if (move_cooldown > 0) move_cooldown--;

            if (ch == 'q' || ch == 'Q') break;
            
            if (ch == ' ') {
                player_melee_attack(&myPlayer, enemies, MAX_ENEMIES);
            } else if (ch != ERR && move_cooldown == 0) { 
                move_player(&myPlayer, ch, &myMap);
                move_cooldown = 2;
            }

            //check key collection
            if (myPlayer.x == myMap.key_x && myPlayer.y == myMap.key_y && !myPlayer.has_key) {
                myPlayer.has_key = 1;
                myMap.key_collected = 1;
                strcpy(message, "Key collected!");
                message_timer = 160;//8 seconds
            }

            //update timer
            if (message_timer > 0) {
                message_timer--;
                if (message_timer == 0) {
                    message[0] = '\0';//clear message
                }
            }

            //check room entry
            for (int r = 0; r < myMap.room_count; r++) {
                Room room = myMap.rooms[r];
                if (myPlayer.x >= room.x && myPlayer.x < room.x + room.w &&
                    myPlayer.y >= room.y && myPlayer.y < room.y + room.h) {
                    //mark room explored
                    for(int yy = room.y; yy < room.y + room.h; yy++) {
                        for(int xx = room.x; xx < room.x + room.w; xx++) {
                            myMap.explored[yy][xx] = 1;
                        }
                    }
                    break;
                }
            }

            int on_door = (myPlayer.y == myMap.door_y && myPlayer.x == myMap.door_x);

            if (on_door) {
                if (!myPlayer.has_key) {
                    strcpy(message, "You need the KEY to enter!");
                    message_timer = 40;//2 seconds
                } else {
                    level++;
                    add_score(&myPlayer, 100);//level clear bonus
                    strcpy(message, "Entering next level...");
                    message_timer = 80;//4 seconds

                    init_map(&myMap);

                    //mark start room explored
                    Room start_room = myMap.rooms[0];
                    for(int yy = start_room.y; yy < start_room.y + start_room.h; yy++) {
                        for(int xx = start_room.x; xx < start_room.x + start_room.w; xx++) {
                            myMap.explored[yy][xx] = 1;
                        }
                    }

                    start_y = myMap.rooms[0].y + (myMap.rooms[0].h / 2);
                    start_x = myMap.rooms[0].x + (myMap.rooms[0].w / 2);
                    
                    //preserve level/xp/score, reset others
                    int saved_level = myPlayer.level;
                    int saved_xp = myPlayer.xp;
                    int saved_xp_cap = myPlayer.xp_cap;
                    int saved_score = myPlayer.score;
                    init_player(&myPlayer, start_y, start_x);
                    myPlayer.level = saved_level;
                    myPlayer.xp = saved_xp;
                    myPlayer.xp_cap = saved_xp_cap;
                    myPlayer.score = saved_score;

                    spawn_enemies(MAX_ENEMIES, enemies, &myMap);
                }
            } else {
                if (myPlayer.has_key == 0) {
                    message[0] = '\0';
                }
            }

            if (myPlayer.hp <= 0) {
                add_to_leaderboard(myPlayer.score, level);
                game_over = 1;
            }

            if(enemy_move_cooldown == 0) {
                for(int i = 0; i < MAX_ENEMIES;i++) {
                    move_enemy(&enemies[i], &myPlayer, &myMap, enemies, MAX_ENEMIES, i);
                }
                enemy_move_cooldown = 4;//faster movement
            }

            for(int i = 0;i < MAX_ENEMIES;i++) {
                if(enemies[i].attack_cooldown > 0) {enemies[i].attack_cooldown--;}
                else {try_to_damage_player(&enemies[i], &myPlayer, enemies[i].damage, &myMap);}
                if(enemies[i].attack_timer > 0) {enemies[i].attack_timer--;}
            }
        } else {
            //game over
            if (ch == 'r' || ch == 'R') {
                //reset game
                game_over = 0;
                init_map(&myMap);
                start_y = myMap.rooms[0].y + (myMap.rooms[0].h / 2);
                start_x = myMap.rooms[0].x + (myMap.rooms[0].w / 2);
                Room start_room = myMap.rooms[0];
                for(int yy = start_room.y; yy < start_room.y + start_room.h; yy++) {
                    for(int xx = start_room.x; xx < start_room.x + start_room.w; xx++) {
                        myMap.explored[yy][xx] = 1;
                    }
                }
                init_player(&myPlayer, start_y, start_x);
                spawn_enemies(MAX_ENEMIES, enemies, &myMap);
                level = 1;
                message[0] = '\0';
                message_timer = 0;
                move_cooldown = 0;
                enemy_move_cooldown = 0;
            }
        }

        erase();
        
        if (game_over) {
            //game over screen
            attron(COLOR_PAIR(6) | A_BOLD);//red bold
            mvprintw(LINES/2 - 2, COLS/2 - 10, "GAME OVER!");
            mvprintw(LINES/2, COLS/2 - 15, "You have been defeated");
            mvprintw(LINES/2 + 2, COLS/2 - 15, "Press R to restart | L for leaderboard");
            attroff(COLOR_PAIR(6) | A_BOLD);
        } else {
            //normal drawing
            draw_map(&myMap);
            
            for(int i = 0;i < MAX_ENEMIES; i++) {
                draw_enemy(&enemies[i], &myMap);
            }
            
            draw_player(&myPlayer);

            //status panel
            int health_color = (myPlayer.hp > 50) ? 7 : ((myPlayer.hp > 25) ? 5 : 6);//green > yellow > red
            attron(COLOR_PAIR(health_color) | A_BOLD);
            mvprintw(0, 0, "HP: %d", myPlayer.hp);
            attroff(COLOR_PAIR(health_color) | A_BOLD);

            attron(COLOR_PAIR(5));//yellow for xp
            mvprintw(0, 10, "XP: %d/%d", myPlayer.xp, myPlayer.xp_cap);
            attroff(COLOR_PAIR(5));

            attron(COLOR_PAIR(3) | A_BOLD); //white bold for player level
            mvprintw(0, 25, "Player: %d", myPlayer.level);
            attroff(COLOR_PAIR(3) | A_BOLD);
            
            mvprintw(0, 40, "Score: %d", myPlayer.score);

            //top right status panel
            attron(COLOR_PAIR(2) | A_BOLD); //blue bold for dungeon level
            mvprintw(0, term_width - 30, "Dungeon: %d", level);
            attroff(COLOR_PAIR(2) | A_BOLD);

            attron(COLOR_PAIR(5)); //yellow for key status
            mvprintw(0, term_width - 18, "Key: %s", myPlayer.has_key ? "YES" : "NO");
            attroff(COLOR_PAIR(5));

            //message in center top (if any)
            if (strlen(message) > 0) {
                attron(COLOR_PAIR(1) | A_BOLD); //cyan bold for messages
                int msg_x = (term_width - strlen(message)) / 2;
                mvprintw(0, msg_x, "%s", message);
                attroff(COLOR_PAIR(1) | A_BOLD);
            }

            //bottom control panel
            attron(COLOR_PAIR(3) | A_DIM); //dim white for controls
            mvprintw(term_height - 1, 0, "WASD: Move | SPACE: Attack | Q: Quit");
            attroff(COLOR_PAIR(3) | A_DIM);
        }

        refresh(); 
    }

    endwin();
    return 0;
}