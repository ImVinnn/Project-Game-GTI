#pragma once

#include "Common.h"

void initGL();
void drawSky();
void drawBottomFloor();
void drawArena();
void drawFlagPoleShadow(float x, float baseY, float z);
void drawPlatformWalls(float minX, float maxX, float minZ, float maxZ,
                       float topY, float botY,
                       float cr, float cg, float cb);
