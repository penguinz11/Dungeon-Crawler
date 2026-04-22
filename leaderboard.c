#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "leaderboard.h"

ScoreEntry leaderboard[MAX_SCORES];
int leaderboard_count = 0;

void load_leaderboard(void) {
    FILE *f = fopen("leaderboard.dat", "rb");
    if (f == NULL) {
        leaderboard_count = 0;
        return;
    }
    fread(&leaderboard_count, sizeof(int), 1, f);
    if (leaderboard_count > MAX_SCORES) leaderboard_count = MAX_SCORES;
    fread(leaderboard, sizeof(ScoreEntry), leaderboard_count, f);
    fclose(f);
}

void save_leaderboard(void) {
    FILE *f = fopen("leaderboard.dat", "wb");
    if (f == NULL) return;
    fwrite(&leaderboard_count, sizeof(int), 1, f);
    fwrite(leaderboard, sizeof(ScoreEntry), leaderboard_count, f);
    fclose(f);
}

void add_to_leaderboard(int score, int level) {
    if (leaderboard_count < MAX_SCORES) {
        leaderboard[leaderboard_count].score = score;
        leaderboard[leaderboard_count].level = level;
        leaderboard_count++;
    } else if (score > leaderboard[MAX_SCORES - 1].score) {
        leaderboard[MAX_SCORES - 1].score = score;
        leaderboard[MAX_SCORES - 1].level = level;
    }
    
    //sort descending
    for (int i = 0; i < leaderboard_count - 1; i++) {
        for (int j = i + 1; j < leaderboard_count; j++) {
            if (leaderboard[i].score < leaderboard[j].score) {
                ScoreEntry temp = leaderboard[i];
                leaderboard[i] = leaderboard[j];
                leaderboard[j] = temp;
            }
        }
    }
    
    save_leaderboard();
}

void draw_leaderboard(void) {
    //draw leaderboard (non-blocking)
    erase();
    attron(COLOR_PAIR(5) | A_BOLD);
    mvprintw(2, 40, "=== leaderboard ===");
    attroff(COLOR_PAIR(5) | A_BOLD);

    attron(COLOR_PAIR(3));
    mvprintw(4, 38, "rank   score   level");
    attroff(COLOR_PAIR(3));

    for (int i = 0; i < leaderboard_count; i++) {
        mvprintw(5 + i, 40, "%d.   %5d    %d", i + 1, leaderboard[i].score, leaderboard[i].level);
    }

    mvprintw(LINES - 2, 35, "press l to return");
    refresh();
}
