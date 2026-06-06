#pragma once

#include "Common.h"

void initBarriers();
void drawBarriers();

void initMovingBarriers();
void updateMovingBarriers();
void drawMovingBarriers();
void makeMovingBarrierBlocks(const MovingBarrier& src, Barrier &top, Barrier &bottom);


void initArena5MovingBarriers();
void updateArena5MovingBarriers();
void drawArena5MovingBarriers();
void resolveArena5BarrierCollisions();


void initBridge5Barriers();
void updateBridge5Barriers();
void drawBridge5Barriers();
void resolveBridge5BarrierCollisions();


void updateArena6Spikes();
void drawArena6Spikes();
void resolveArena6SpikeCollisions();


void initArena7FallingSpike();
void updateArena7FallingSpike();
void drawArena7FallingSpike();
void resolveArena7FallingSpikeCollision();
