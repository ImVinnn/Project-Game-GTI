#pragma once

#include "Common.h"
#include "Arena.h"
#include "Barrier.h"
#include "Ball.h"
#include "Star.h"
#include "Physics.h"
#include "HUD.h"

void update();
void timerCallback(int val);
void display();
void reshape(int w, int h);
void keyDown(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void specialDown(int key, int x, int y);
void specialUp(int key, int x, int y);
void setCamera();
