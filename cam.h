#ifndef CAM_H
#define CAM_H

extern int VIEW_HEIGHT;
extern int VIEW_WIDTH;

extern int startX;
extern int startY;

void clamp_cam(int playerY, int playerX, int worldHeight, int worldWidth);

#endif