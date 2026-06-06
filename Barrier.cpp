#include "Barrier.h"
#include <GL/glut.h>
#include <cmath>

float clamp01Barrier(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

float getThirdArenaSlopeY(float x, float z) {
    float bridgeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;

    float thirdMinX = LOWER_ARENA_HALF + BRIDGE2_LENGTH;
    float thirdMaxX = thirdMinX + THIRD_ARENA_HALF * 2.0f;
    float thirdMinZ = bridgeCenterZ - THIRD_ARENA_HALF;
    float thirdMaxZ = bridgeCenterZ + THIRD_ARENA_HALF;
    float thirdY    = LOWER_Y - BALL_RADIUS;

    float thirdHoleCenterX = (thirdMinX + thirdMaxX) * 0.5f;
    float thirdHoleCenterZ = (thirdMinZ + thirdMaxZ) * 0.5f;

    float thirdHoleHalfX = THIRD_ARENA_HALF * 0.30f;
    float thirdHoleHalfZ = THIRD_ARENA_HALF * 0.30f;

    float thirdHoleMinX = thirdHoleCenterX - thirdHoleHalfX;
    float thirdHoleMaxX = thirdHoleCenterX + thirdHoleHalfX;
    float thirdHoleMinZ = thirdHoleCenterZ - thirdHoleHalfZ;
    float thirdHoleMaxZ = thirdHoleCenterZ + thirdHoleHalfZ;

    float slopeDepth = 1.4f;
    float holeTopY = thirdY - slopeDepth;

    float tx = 1.0f;
    float tz = 1.0f;

    if (x < thirdHoleMinX) {
        tx = (x - thirdMinX) / (thirdHoleMinX - thirdMinX);
    } else if (x > thirdHoleMaxX) {
        tx = (thirdMaxX - x) / (thirdMaxX - thirdHoleMaxX);
    }

    if (z < thirdHoleMinZ) {
        tz = (z - thirdMinZ) / (thirdHoleMinZ - thirdMinZ);
    } else if (z > thirdHoleMaxZ) {
        tz = (thirdMaxZ - z) / (thirdMaxZ - thirdHoleMaxZ);
    }

    tx = clamp01Barrier(tx);
    tz = clamp01Barrier(tz);

    float t = tx;
    if (tz < t) {
        t = tz;
    }

    return thirdY - (thirdY - holeTopY) * t;
}

void getFourthArenaBounds(float &minX, float &maxX, float &minZ, float &maxZ) {
    getFourthArenaLayout(minX, maxX, minZ, maxZ);
}


void getFifthArenaBounds(float &minX, float &maxX, float &minZ, float &maxZ) {
    float fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ;
    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);
    float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;
    float fifthHalf     = FIFTH_ARENA_HALF;
    minX = fourthMaxX + BRIDGE3_LENGTH;
    maxX = minX + fifthHalf * 2.0f;
    minZ = fourthCenterZ - fifthHalf;
    maxZ = fourthCenterZ + fifthHalf;
}


void initArena5MovingBarriers() {
    arena5MovingBarriers.clear();

    float minX, maxX, minZ, maxZ;
    getFifthArenaBounds(minX, maxX, minZ, maxZ);

    float centerZ    = (minZ + maxZ) * 0.5f;
    float laneOffset = (maxZ - minZ) * 0.28f;

    for (int i = 0; i < 3; i++) {
        Arena5MovingBarrier b;
        b.minX  = minX + 0.85f;
        b.maxX  = maxX - 0.85f;
        b.x     = b.minX + (b.maxX - b.minX) * (0.20f + 0.30f * i);
        b.z     = centerZ + laneOffset * (1 - i);
        b.speed = (i % 2 == 0) ? 0.070f : -0.070f;
        arena5MovingBarriers.push_back(b);
    }
}

void updateArena5MovingBarriers() {
    for (size_t i = 0; i < arena5MovingBarriers.size(); i++) {
        Arena5MovingBarrier& b = arena5MovingBarriers[i];
        b.x += b.speed;
        if (b.x < b.minX) { b.x = b.minX; if (b.speed < 0.0f) b.speed = -b.speed; }
        if (b.x > b.maxX) { b.x = b.maxX; if (b.speed > 0.0f) b.speed = -b.speed; }
    }
}

static void makeArena5BarrierBlock(const Arena5MovingBarrier& src, Barrier& block) {
    block.x      = src.x;
    block.z      = src.z;
    block.hw     = 0.35f;
    block.hd     = 0.75f;
    block.floorY = FOURTH_Y - BALL_RADIUS;
}

void drawArena5MovingBarriers() {
    for (size_t i = 0; i < arena5MovingBarriers.size(); i++) {
        Barrier block;
        makeArena5BarrierBlock(arena5MovingBarriers[i], block);

        float bY0 = block.floorY;
        float bY1 = bY0 + 0.7f;
        float bx0 = block.x - block.hw;
        float bx1 = block.x + block.hw;
        float bz0 = block.z - block.hd;
        float bz1 = block.z + block.hd;

        
        if (i % 2 == 0)
            glColor3f(0.95f, 0.20f, 0.20f);
        else
            glColor3f(1.00f, 0.60f, 0.10f);

        glBegin(GL_QUADS);
            glNormal3f( 0, 1, 0);
            glVertex3f(bx0, bY1, bz0); glVertex3f(bx1, bY1, bz0);
            glVertex3f(bx1, bY1, bz1); glVertex3f(bx0, bY1, bz1);

            glNormal3f( 0, 0, 1);
            glVertex3f(bx0, bY0, bz1); glVertex3f(bx1, bY0, bz1);
            glVertex3f(bx1, bY1, bz1); glVertex3f(bx0, bY1, bz1);

            glNormal3f( 0, 0,-1);
            glVertex3f(bx1, bY0, bz0); glVertex3f(bx0, bY0, bz0);
            glVertex3f(bx0, bY1, bz0); glVertex3f(bx1, bY1, bz0);

            glNormal3f(-1, 0, 0);
            glVertex3f(bx0, bY0, bz0); glVertex3f(bx0, bY0, bz1);
            glVertex3f(bx0, bY1, bz1); glVertex3f(bx0, bY1, bz0);

            glNormal3f( 1, 0, 0);
            glVertex3f(bx1, bY0, bz1); glVertex3f(bx1, bY0, bz0);
            glVertex3f(bx1, bY1, bz0); glVertex3f(bx1, bY1, bz1);
        glEnd();
    }
}

void resolveArena5BarrierCollisions() {
    float hw     = 0.35f;
    float hd     = 0.75f;
    float height = 0.7f;
    float floorY = FOURTH_Y - BALL_RADIUS;

    for (size_t i = 0; i < arena5MovingBarriers.size(); i++) {
        Arena5MovingBarrier& b = arena5MovingBarriers[i];

        float bx0 = b.x - hw - BALL_RADIUS;
        float bx1 = b.x + hw + BALL_RADIUS;
        float bz0 = b.z - hd - BALL_RADIUS;
        float bz1 = b.z + hd + BALL_RADIUS;
        float bY1 = floorY + height;

        if (ballY > bY1) continue;
        if (!(ballX > bx0 && ballX < bx1 && ballZ > bz0 && ballZ < bz1)) continue;

        float dLeft  = ballX - bx0;
        float dRight = bx1   - ballX;
        float dFront = ballZ - bz0;
        float dBack  = bz1   - ballZ;

        float minD = dLeft; int side = 0;
        if (dRight < minD) { minD = dRight; side = 1; }
        if (dFront < minD) { minD = dFront; side = 2; }
        if (dBack  < minD) { minD = dBack;  side = 3; }

        float pushVX = b.speed;
        switch (side) {
            case 0: ballX = bx0; velX = -std::fabs(velX) * 0.4f;                   break;
            case 1: ballX = bx1; velX =  std::fabs(velX) * 0.4f + pushVX;          break;
            case 2: ballZ = bz0; velZ = -std::fabs(velZ) * 0.4f;                   break;
            case 3: ballZ = bz1; velZ =  std::fabs(velZ) * 0.4f;                   break;
        }
    }
}


void initMovingBarriers() {
    movingBarriers.clear();

    float minX, maxX, minZ, maxZ;
    getFourthArenaBounds(minX, maxX, minZ, maxZ);

    for (int i = 0; i < 6; i++) {
        MovingBarrier b;
        b.x = minX + 0.35f + i * 1.10f;
        b.gapZ = 0.0f;
        b.speed = 0.018f;
        movingBarriers.push_back(b);
    }
}

void initBarriers() {
    barriers.clear();

    float a2Z0 = ARENA_HALF + RAMP_LENGTH;

    
    {
        Barrier b;
        b.x = -1.8f;
        b.z = a2Z0 + LOWER_ARENA_HALF;
        b.hw = 0.3f;
        b.hd = 0.8f;
        b.floorY = LOWER_Y - BALL_RADIUS;
        barriers.push_back(b);
    }

    
    {
        Barrier b;
        b.x = 1.8f;
        b.z = a2Z0 + LOWER_ARENA_HALF;
        b.hw = 0.3f;
        b.hd = 0.8f;
        b.floorY = LOWER_Y - BALL_RADIUS;
        barriers.push_back(b);
    }

    
    float bridgeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;

    float thirdMinX = LOWER_ARENA_HALF + BRIDGE2_LENGTH;
    float thirdMaxX = thirdMinX + THIRD_ARENA_HALF * 2.0f;
    float thirdMinZ = bridgeCenterZ - THIRD_ARENA_HALF;
    float thirdMaxZ = bridgeCenterZ + THIRD_ARENA_HALF;

    float thirdCenterX = (thirdMinX + thirdMaxX) * 0.5f;
    float thirdCenterZ = (thirdMinZ + thirdMaxZ) * 0.5f;

    float thirdHoleHalfX = THIRD_ARENA_HALF * 0.30f;
    float thirdHoleHalfZ = THIRD_ARENA_HALF * 0.30f;

    float thirdHoleMinX = thirdCenterX - thirdHoleHalfX;
    float thirdHoleMaxX = thirdCenterX + thirdHoleHalfX;
    float thirdHoleMinZ = thirdCenterZ - thirdHoleHalfZ;
    float thirdHoleMaxZ = thirdCenterZ + thirdHoleHalfZ;

    
    {
        Barrier b;
        b.x = thirdHoleMinX - 1.15f;
        b.z = thirdCenterZ;
        b.hw = 0.25f;
        b.hd = thirdHoleHalfZ + 0.95f;
        b.floorY = getThirdArenaSlopeY(b.x, b.z);
        barriers.push_back(b);
    }

   
    {
        Barrier b;
        b.x = thirdMinX + 1.15f;
        b.z = thirdCenterZ - 0.85f;
        b.hw = 0.85f;
        b.hd = 0.22f;
        b.floorY = getThirdArenaSlopeY(b.x, b.z);
        barriers.push_back(b);
    }

    float fourthMinX;
    float fourthMaxX;
    float fourthMinZ;
    float fourthMaxZ;
    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);
    float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;

    float fifthHalf = FIFTH_ARENA_HALF;
    float fifthMinX = fourthMaxX + BRIDGE3_LENGTH;
    float fifthMaxX = fifthMinX + fifthHalf * 2.0f;
    float fifthMinZ = fourthCenterZ - fifthHalf;

    float bridge4MinZ = fifthMinZ - BRIDGE4_LENGTH;
    float sixthMinZ = bridge4MinZ - SIXTH_ARENA_LENGTH;
    
}

void drawBarrierBlock(const Barrier& b) {
    float bY0 = b.floorY;
    float bY1 = bY0 + 0.7f;
    float bx0 = b.x - b.hw;
    float bx1 = b.x + b.hw;
    float bz0 = b.z - b.hd;
    float bz1 = b.z + b.hd;

    glColor3f(0.9f, 0.3f, 0.2f);

    glBegin(GL_QUADS);
        
        glNormal3f(0, 1, 0);
        glVertex3f(bx0, bY1, bz0);
        glVertex3f(bx1, bY1, bz0);
        glVertex3f(bx1, bY1, bz1);
        glVertex3f(bx0, bY1, bz1);

        
        glNormal3f(0, 0, 1);
        glVertex3f(bx0, bY0, bz1);
        glVertex3f(bx1, bY0, bz1);
        glVertex3f(bx1, bY1, bz1);
        glVertex3f(bx0, bY1, bz1);

        
        glNormal3f(0, 0, -1);
        glVertex3f(bx0, bY0, bz0);
        glVertex3f(bx1, bY0, bz0);
        glVertex3f(bx1, bY1, bz0);
        glVertex3f(bx0, bY1, bz0);

        
        glNormal3f(-1, 0, 0);
        glVertex3f(bx0, bY0, bz0);
        glVertex3f(bx0, bY0, bz1);
        glVertex3f(bx0, bY1, bz1);
        glVertex3f(bx0, bY1, bz0);

        
        glNormal3f(1, 0, 0);
        glVertex3f(bx1, bY0, bz0);
        glVertex3f(bx1, bY0, bz1);
        glVertex3f(bx1, bY1, bz1);
        glVertex3f(bx1, bY1, bz0);
    glEnd();
}

void updateMovingBarriers() {
    float minX, maxX, minZ, maxZ;
    getFourthArenaBounds(minX, maxX, minZ, maxZ);

    float wallHalfX = 0.15f;
    for (size_t i = 0; i < movingBarriers.size(); i++) {
        MovingBarrier& b = movingBarriers[i];
        b.x += b.speed;

        if (b.x + wallHalfX > maxX) {
            b.x = minX - wallHalfX;
        }
    }
}

void makeMovingBarrierBlocks(const MovingBarrier& src, Barrier &top, Barrier &bottom) {
    float minX, maxX, minZ, maxZ;
    getFourthArenaBounds(minX, maxX, minZ, maxZ);

    float wallHalfX = 0.15f;
    float floorY = FOURTH_Y - BALL_RADIUS;

    top.x = src.x;
    top.z = (minZ + maxZ) * 0.5f;
    top.hw = wallHalfX;
    top.hd = (maxZ - minZ) * 0.5f;
    top.floorY = floorY;

    bottom.x = src.x;
    bottom.z = top.z;
    bottom.hw = wallHalfX;
    bottom.hd = 0.0f;
    bottom.floorY = floorY;
}

void drawBarriers() {
    for (size_t i = 0; i < barriers.size(); i++) {
        drawBarrierBlock(barriers[i]);
    }
}

void drawMovingBarriers() {
    for (size_t i = 0; i < movingBarriers.size(); i++) {
        Barrier top, bottom;
        makeMovingBarrierBlocks(movingBarriers[i], top, bottom);

        if (top.hd > 0.05f) {
            drawBarrierBlock(top);
        }

        if (bottom.hd > 0.05f) {
            drawBarrierBlock(bottom);
        }
    }
}



static void getBridge5Layout(float &centerX, float &minX, float &maxX,
                              float &bridgeMinZ, float &bridgeMaxZ) {
    
    float fourthMinX_l, fourthMaxX_l, fourthMinZ_l, fourthMaxZ_l;
    getFourthArenaLayout(fourthMinX_l, fourthMaxX_l, fourthMinZ_l, fourthMaxZ_l);

    float fourthCenterZ = (fourthMinZ_l + fourthMaxZ_l) * 0.5f;
    float fifthHalf     = FIFTH_ARENA_HALF;
    float fifthMinX_l   = fourthMaxX_l + BRIDGE3_LENGTH;
    float fifthMaxX_l   = fifthMinX_l + fifthHalf * 2.0f;
    float fifthMinZ_l   = fourthCenterZ - fifthHalf;

    float bridge4MinZ_l = fifthMinZ_l - BRIDGE4_LENGTH;
    float sixthMinZ_l   = bridge4MinZ_l - SIXTH_ARENA_LENGTH;
    float sixthMinX_l   = fifthMinX_l;
    float sixthMaxX_l   = fifthMaxX_l;

    centerX    = (sixthMinX_l + sixthMaxX_l) * 0.5f;
    minX       = centerX - BRIDGE5_HALF_WIDTH;
    maxX       = centerX + BRIDGE5_HALF_WIDTH;
    bridgeMaxZ = sixthMinZ_l;
    bridgeMinZ = sixthMinZ_l - BRIDGE5_LENGTH;
}

void initBridge5Barriers() {
    bridge5Barriers.clear();

    float centerX, minX, maxX, bridgeMinZ, bridgeMaxZ;
    getBridge5Layout(centerX, minX, maxX, bridgeMinZ, bridgeMaxZ);

    
    int count = 4;
    float bridgeLen = bridgeMaxZ - bridgeMinZ;
    float step = bridgeLen / (count + 1);

    for (int i = 0; i < count; i++) {
        Bridge5Barrier b;
        b.centerZ = bridgeMinZ + step * (i + 1);
        b.speed   = 0.030f - i * 0.004f;   
        b.minX    = minX;
        b.maxX    = maxX;

        
        if (i % 2 == 0) {
            b.posX = minX + 0.4;
            b.dir  = +1;
        } else {
            b.posX = maxX +0.4;
            b.dir  = -1;
        }

        bridge5Barriers.push_back(b);
    }
}

void updateBridge5Barriers() {
    for (size_t i = 0; i < bridge5Barriers.size(); i++) {
        Bridge5Barrier& b = bridge5Barriers[i];
        b.posX += b.speed * b.dir;

        if (b.posX >= b.maxX) {
            b.posX = b.maxX;
            b.dir  = -1;
        } else if (b.posX <= b.minX) {
            b.posX = b.minX;
            b.dir  = +1;
        }
    }
}

void drawBridge5Barriers() {
    
    float hw = 0.08f;   
    float hd = 0.18f;   
    float height = 0.65f;

    float fourthY = FOURTH_Y - BALL_RADIUS;

    for (size_t i = 0; i < bridge5Barriers.size(); i++) {
        const Bridge5Barrier& b = bridge5Barriers[i];

        float bx0 = b.posX - hw;
        float bx1 = b.posX + hw;
        float bz0 = b.centerZ - hd;
        float bz1 = b.centerZ + hd;
        float bY0 = fourthY;
        float bY1 = fourthY + height;

        
        if (i % 2 == 0) {
            glColor3f(0.95f, 0.20f, 0.20f);   
        } else {
            glColor3f(1.00f, 0.60f, 0.10f);  
        }

        glBegin(GL_QUADS);
           
            glNormal3f(0, 1, 0);
            glVertex3f(bx0, bY1, bz0);
            glVertex3f(bx1, bY1, bz0);
            glVertex3f(bx1, bY1, bz1);
            glVertex3f(bx0, bY1, bz1);

            
            glNormal3f(0, 0, 1);
            glVertex3f(bx0, bY0, bz1);
            glVertex3f(bx1, bY0, bz1);
            glVertex3f(bx1, bY1, bz1);
            glVertex3f(bx0, bY1, bz1);

            
            glNormal3f(0, 0, -1);
            glVertex3f(bx1, bY0, bz0);
            glVertex3f(bx0, bY0, bz0);
            glVertex3f(bx0, bY1, bz0);
            glVertex3f(bx1, bY1, bz0);

           
            glNormal3f(-1, 0, 0);
            glVertex3f(bx0, bY0, bz1);
            glVertex3f(bx0, bY0, bz0);
            glVertex3f(bx0, bY1, bz0);
            glVertex3f(bx0, bY1, bz1);

           
            glNormal3f(1, 0, 0);
            glVertex3f(bx1, bY0, bz0);
            glVertex3f(bx1, bY0, bz1);
            glVertex3f(bx1, bY1, bz1);
            glVertex3f(bx1, bY1, bz0);
        glEnd();
    }
}

void resolveBridge5BarrierCollisions() {
    float hw = 0.08f;
    float hd = 0.18f;
    float height = 0.65f;
    float fourthY = FOURTH_Y - BALL_RADIUS;

    for (size_t i = 0; i < bridge5Barriers.size(); i++) {
        Bridge5Barrier& b = bridge5Barriers[i];

        float bx0 = b.posX - hw - BALL_RADIUS;
        float bx1 = b.posX + hw + BALL_RADIUS;
        float bz0 = b.centerZ - hd - BALL_RADIUS;
        float bz1 = b.centerZ + hd + BALL_RADIUS;
        float bY1 = fourthY + height;

        
        if (ballY > bY1) continue;
        if (!(ballX > bx0 && ballX < bx1 && ballZ > bz0 && ballZ < bz1)) continue;

        float dLeft  = ballX - bx0;
        float dRight = bx1  - ballX;
        float dFront = ballZ - bz0;
        float dBack  = bz1  - ballZ;

        float minD = dLeft;
        int side = 0;
        if (dRight < minD) { minD = dRight; side = 1; }
        if (dFront < minD) { minD = dFront; side = 2; }
        if (dBack  < minD) { minD = dBack;  side = 3; }

        
        float pushVX = b.speed * b.dir;

        switch (side) {
            case 0:
                ballX = bx0;
                velX  = -std::fabs(velX) * 0.4f;
                break;
            case 1:
                ballX = bx1;
                velX  =  std::fabs(velX) * 0.4f + pushVX;
                break;
            case 2:
                ballZ = bz0;
                velZ  = -std::fabs(velZ) * 0.4f;
                break;
            case 3:
                ballZ = bz1;
                velZ  =  std::fabs(velZ) * 0.4f;
                break;
        }
    }
}



static float arena6SpikeTime = 0.0f;

static void getArena6Layout(float &minX, float &maxX, float &minZ, float &maxZ)
{
    float fourthMinX;
    float fourthMaxX;
    float fourthMinZ;
    float fourthMaxZ;
    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);

    float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;
    float fifthHalf     = FIFTH_ARENA_HALF;
    minX                = fourthMaxX + BRIDGE3_LENGTH;
    maxX                = minX + fifthHalf * 2.0f;
    float fifthMinZ     = fourthCenterZ - fifthHalf;
    float bridge4MinZ   = fifthMinZ - BRIDGE4_LENGTH;
    maxZ                = bridge4MinZ;
    minZ                = bridge4MinZ - SIXTH_ARENA_LENGTH;
}

void updateArena6Spikes()
{
    arena6SpikeTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
}

static float getArena6SpikeRise(int row, int col)
{
    const float riseTime       = 0.65f;
    const float holdTopTime    = 3.0f;
    const float fallTime       = 0.65f;
    const float holdBottomTime = 3.0f;
    const float cycleTime      = riseTime + holdTopTime + fallTime + holdBottomTime;
    const float offset         = ((row + col) % 2 == 0) ? 0.0f : cycleTime * 0.5f;

    float t = arena6SpikeTime + offset;
    float phase = std::fmod(t, cycleTime);

    if (phase < riseTime) {
        return phase / riseTime;
    }
    if (phase < riseTime + holdTopTime) {
        return 1.0f;
    }
    if (phase < riseTime + holdTopTime + fallTime) {
        float fallPhase = phase - riseTime - holdTopTime;
        return 1.0f - (fallPhase / fallTime);
    }

    return 0.0f;
}

static void drawSingleArena6Spike(float x, float floorY, float z, float rise)
{
    const float fullHeight = 1.05f;
    const float baseHalf   = 0.22f;
    float h = fullHeight * rise;

    if (h <= 0.02f) {
        return;
    }

    float y0 = floorY + 0.01f;
    float y1 = y0 + h;

    glBegin(GL_TRIANGLES);
        glColor3f(0.72f, 0.60f, 0.45f); glNormal3f(0.0f, 0.8f, 0.6f);
        glVertex3f(x - baseHalf, y0, z + baseHalf);
        glVertex3f(x + baseHalf, y0, z + baseHalf);
        glVertex3f(x, y1, z);

        glColor3f(0.52f, 0.39f, 0.32f); glNormal3f(0.0f, 0.8f, -0.6f);
        glVertex3f(x + baseHalf, y0, z - baseHalf);
        glVertex3f(x - baseHalf, y0, z - baseHalf);
        glVertex3f(x, y1, z);

        glColor3f(0.34f, 0.23f, 0.23f); glNormal3f(-0.7f, 0.7f, 0.0f);
        glVertex3f(x - baseHalf, y0, z - baseHalf);
        glVertex3f(x - baseHalf, y0, z + baseHalf);
        glVertex3f(x, y1, z);

        glColor3f(0.52f, 0.39f, 0.32f); glNormal3f(0.7f, 0.7f, 0.0f);
        glVertex3f(x + baseHalf, y0, z + baseHalf);
        glVertex3f(x + baseHalf, y0, z - baseHalf);
        glVertex3f(x, y1, z);
    glEnd();

    
    float baseY = y0 + h * 0.18f;
    glBegin(GL_QUADS);
        glColor3f(0.34f, 0.23f, 0.23f); glNormal3f(0, 1, 0);
        glVertex3f(x - baseHalf, baseY, z - baseHalf);
        glVertex3f(x + baseHalf, baseY, z - baseHalf);
        glVertex3f(x + baseHalf, baseY, z + baseHalf);
        glVertex3f(x - baseHalf, baseY, z + baseHalf);
    glEnd();
}

void drawArena6Spikes()
{
    float minX, maxX, minZ, maxZ;
    getArena6Layout(minX, maxX, minZ, maxZ);

    float floorY = FOURTH_Y - BALL_RADIUS;
    float centerX = (minX + maxX) * 0.5f;
    float centerZ = (minZ + maxZ) * 0.5f;

    const float xOffsets[4] = { -2.25f, -0.75f, 0.75f, 2.25f };
    const float zOffsets[3] = { -1.45f, 0.0f, 1.45f };

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 4; c++) {
            drawSingleArena6Spike(centerX + xOffsets[c],
                                  floorY,
                                  centerZ + zOffsets[r],
                                  getArena6SpikeRise(r, c));
        }
    }
}

void resolveArena6SpikeCollisions()
{
    float minX, maxX, minZ, maxZ;
    getArena6Layout(minX, maxX, minZ, maxZ);

    float floorY = FOURTH_Y - BALL_RADIUS;
    if (ballY < floorY || ballY > floorY + 1.25f) {
        return;
    }

    float centerX = (minX + maxX) * 0.5f;
    float centerZ = (minZ + maxZ) * 0.5f;
    const float xOffsets[4] = { -2.25f, -0.75f, 0.75f, 2.25f };
    const float zOffsets[3] = { -1.45f, 0.0f, 1.45f };
    const float hitRadius = 0.36f;

    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 4; c++) {
            if (getArena6SpikeRise(r, c) < 0.35f) {
                continue;
            }

            float dx = ballX - (centerX + xOffsets[c]);
            float dz = ballZ - (centerZ + zOffsets[r]);
            if (dx * dx + dz * dz <= hitRadius * hitRadius) {
                gameState = GAMEOVER;
                return;
            }
        }
    }
}



struct Arena7FallingSpike {
    float x;
    float z;
    float startTime;
};

static const int ARENA7_FALLING_SPIKE_COUNT = 3;
static Arena7FallingSpike arena7Spikes[ARENA7_FALLING_SPIKE_COUNT];

static void getArena7LayoutForSpike(float &minX, float &maxX,
                                    float &minZ, float &maxZ)
{
    float fourthMinX;
    float fourthMaxX;
    float fourthMinZ;
    float fourthMaxZ;
    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);

    float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;
    float fifthHalf     = FIFTH_ARENA_HALF;
    minX                = fourthMaxX + BRIDGE3_LENGTH;
    maxX                = minX + fifthHalf * 2.0f;
    float fifthMinZ     = fourthCenterZ - fifthHalf;
    float bridge4MinZ   = fifthMinZ - BRIDGE4_LENGTH;
    float sixthMinZ     = bridge4MinZ - SIXTH_ARENA_LENGTH;
    maxZ                = sixthMinZ - BRIDGE5_LENGTH;
    minZ                = maxZ - SEVENTH_ARENA_LENGTH;
}

static float randomRange(float lo, float hi)
{
    return lo + (static_cast<float>(rand()) / RAND_MAX) * (hi - lo);
}

static void chooseArena7FallingSpikeTarget(int index, float delay)
{
    float minX, maxX, minZ, maxZ;
    getArena7LayoutForSpike(minX, maxX, minZ, maxZ);

    const float margin = 0.65f;
    arena7Spikes[index].x = randomRange(minX + margin, maxX - margin);
    arena7Spikes[index].z = randomRange(minZ + margin, maxZ - margin);
    arena7Spikes[index].startTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f + delay;
}

void initArena7FallingSpike()
{
    for (int i = 0; i < ARENA7_FALLING_SPIKE_COUNT; i++) {
        chooseArena7FallingSpikeTarget(i, i * 0.85f);
    }
}

void updateArena7FallingSpike()
{
    const float warnTime   = 1.5f;
    const float fallTime   = 0.55f;
    const float impactTime = 0.45f;
    const float restTime   = 0.60f;

    float now = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float cycleTime = warnTime + fallTime + impactTime + restTime;

    for (int i = 0; i < ARENA7_FALLING_SPIKE_COUNT; i++) {
        float age = now - arena7Spikes[i].startTime;
        if (age > cycleTime) {
            chooseArena7FallingSpikeTarget(i, 0.0f);
        }
    }
}

static void drawArena7SpikeWarning(float x, float z, float alpha)
{
    float floorY = FOURTH_Y - BALL_RADIUS + 0.025f;
    float half = 0.55f;

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glColor4f(0.0f, 0.0f, 0.0f, alpha);

    glBegin(GL_QUADS);
        glVertex3f(x - half, floorY, z - half);
        glVertex3f(x + half, floorY, z - half);
        glVertex3f(x + half, floorY, z + half);
        glVertex3f(x - half, floorY, z + half);
    glEnd();

    glDepthMask(GL_TRUE);
    glPopAttrib();
}

static void drawSingleFallingSpike(float x, float tipY, float z)
{
    const float height = 1.35f;
    const float baseHalf = 0.32f;
    float baseY = tipY + height;

    glBegin(GL_TRIANGLES);
        glColor3f(0.70f, 0.58f, 0.44f); glNormal3f(0.0f, -0.8f, 0.6f);
        glVertex3f(x - baseHalf, baseY, z + baseHalf);
        glVertex3f(x + baseHalf, baseY, z + baseHalf);
        glVertex3f(x, tipY, z);

        glColor3f(0.48f, 0.34f, 0.28f); glNormal3f(0.0f, -0.8f, -0.6f);
        glVertex3f(x + baseHalf, baseY, z - baseHalf);
        glVertex3f(x - baseHalf, baseY, z - baseHalf);
        glVertex3f(x, tipY, z);

        glColor3f(0.34f, 0.23f, 0.22f); glNormal3f(-0.7f, -0.7f, 0.0f);
        glVertex3f(x - baseHalf, baseY, z - baseHalf);
        glVertex3f(x - baseHalf, baseY, z + baseHalf);
        glVertex3f(x, tipY, z);

        glColor3f(0.55f, 0.42f, 0.34f); glNormal3f(0.7f, -0.7f, 0.0f);
        glVertex3f(x + baseHalf, baseY, z + baseHalf);
        glVertex3f(x + baseHalf, baseY, z - baseHalf);
        glVertex3f(x, tipY, z);
    glEnd();

    glBegin(GL_QUADS);
        glColor3f(0.34f, 0.23f, 0.22f); glNormal3f(0.0f, 1.0f, 0.0f);
        glVertex3f(x - baseHalf, baseY, z - baseHalf);
        glVertex3f(x + baseHalf, baseY, z - baseHalf);
        glVertex3f(x + baseHalf, baseY, z + baseHalf);
        glVertex3f(x - baseHalf, baseY, z + baseHalf);
    glEnd();
}

void drawArena7FallingSpike()
{
    const float warnTime   = 1.5f;
    const float fallTime   = 0.55f;
    const float impactTime = 0.45f;

    float now = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

    for (int i = 0; i < ARENA7_FALLING_SPIKE_COUNT; i++) {
        const Arena7FallingSpike& spike = arena7Spikes[i];
        float age = now - spike.startTime;

        if (age < 0.0f) {
            continue;
        }

        if (age < warnTime) {
            float alpha = 0.20f + (age / warnTime) * 0.35f;
            drawArena7SpikeWarning(spike.x, spike.z, alpha);
            continue;
        }

        if (age < warnTime + fallTime + impactTime) {
            drawArena7SpikeWarning(spike.x, spike.z, 0.45f);

            float fallAge = age - warnTime;
            float t = (fallAge < fallTime) ? (fallAge / fallTime) : 1.0f;
            float floorY = FOURTH_Y - BALL_RADIUS + 0.02f;
            float startY = floorY + 5.0f;
            float tipY = startY + (floorY - startY) * t;

            drawSingleFallingSpike(spike.x, tipY, spike.z);
        }
    }
}

void resolveArena7FallingSpikeCollision()
{
    const float warnTime   = 1.5f;
    const float fallTime   = 0.55f;
    const float impactTime = 0.45f;
    const float spikeHeight = 1.35f;

    float now = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

    float floorY = FOURTH_Y - BALL_RADIUS;
    if (ballY < floorY || ballY > floorY + 1.4f) {
        return;
    }

    for (int i = 0; i < ARENA7_FALLING_SPIKE_COUNT; i++) {
        const Arena7FallingSpike& spike = arena7Spikes[i];
        float age = now - spike.startTime;

        if (age < warnTime || age > warnTime + fallTime + impactTime) {
            continue;
        }

        float fallAge = age - warnTime;
        float t = (fallAge < fallTime) ? (fallAge / fallTime) : 1.0f;
        float spikeFloorY = floorY + 0.02f;
        float startY = spikeFloorY + 5.0f;
        float tipY = startY + (spikeFloorY - startY) * t;
        float baseY = tipY + spikeHeight;
        float ballBottom = ballY - BALL_RADIUS;
        float ballTop = ballY + BALL_RADIUS;

        if (tipY > ballTop || baseY < ballBottom) {
            continue;
        }

        float dx = ballX - spike.x;
        float dz = ballZ - spike.z;
        const float hitRadius = 0.48f;

        if (dx * dx + dz * dz <= hitRadius * hitRadius) {
            gameState = GAMEOVER;
            return;
        }
    }
}
