#ifndef CAM_H
#define CAM_H

#define VIEW_HEIGHT 20
#define VIEW_WIDTH 40

extern int startX;
extern int startY;

void clamp_cam(int playerY, int playerX, int worldHeight, int worldWidth);

#endif