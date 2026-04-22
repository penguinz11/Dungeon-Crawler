#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#define MAX_SCORES 10

typedef struct {
    int score;
    int level;
} ScoreEntry;

void load_leaderboard(void);
void save_leaderboard(void);
void add_to_leaderboard(int score, int level);
void draw_leaderboard(void);

extern ScoreEntry leaderboard[MAX_SCORES];
extern int leaderboard_count;

#endif
