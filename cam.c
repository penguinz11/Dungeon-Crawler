#include <ncurses.h>
#include <stdlib.h>
#include "cam.h"

int VIEW_HEIGHT;
int VIEW_WIDTH;
int startX, startY;

void clamp_cam(int playerY, int playerX,int worldHeight,int worldWidth) {
    startY = playerY - (VIEW_HEIGHT / 2);
    startX = playerX - (VIEW_WIDTH / 2);

    if (startY < 0) startY = 0;
    if (startX < 0) startX = 0;
    if (startY > worldHeight - VIEW_HEIGHT) startY = worldHeight - VIEW_HEIGHT;
    if (startX > worldWidth - VIEW_WIDTH) startX = worldWidth - VIEW_WIDTH;
}