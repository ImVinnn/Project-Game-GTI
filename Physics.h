#pragma once

#include "Common.h"

bool isInLowerHole(float x, float z);
float getRampHeight(float x, float z);
float getRamp2Height(float x, float z);
float getGroundHeight(float x, float z);
void getSlopeForce(float x, float z, float &outFX, float &outFZ); 
void resolveWorldSolidCollision(float &x, float &z, float y, float &vx, float &vz);
