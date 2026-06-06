#pragma once

#include <vector>
#include <cmath>
#include <cstdlib>

#define PI 3.14159265f
#define ARENA_HALF   6.0f
#define ARENA_SIZE   (ARENA_HALF * 2.0f)
#define LOWER_ARENA_HALF 4.0f
#define LOWER_Y         -1.0f
#define LOWER_HOLE_HALF 1.0f
#define RAMP_LENGTH     2.0f
#define RAMP_HALF_WIDTH 2.5f
#define BRIDGE2_LENGTH  10.0f
#define BRIDGE2_HALF_WIDTH 0.6f
#define THIRD_ARENA_HALF LOWER_ARENA_HALF
#define BALL_RADIUS   0.35f
#define FRICTION      0.95f
#define ACCEL         0.008f
#define GRAVITY 0.02f

#define ARENA_SCALE       0.85f
#define FOURTH_ARENA_HALF (THIRD_ARENA_HALF * ARENA_SCALE)
#define RAMP2_LENGTH      2.5f
#define RAMP2_HALF_WIDTH  2.0f
#define FOURTH_Y         -2.8f
#define BRIDGE3_LENGTH      6.0f
#define BRIDGE3_HALF_WIDTH  0.8f
#define FIFTH_ARENA_HALF   (FOURTH_ARENA_HALF * ARENA_SCALE)
#define FIFTH_Y            -4.3f
#define BRIDGE4_LENGTH      3.0f
#define BRIDGE4_HALF_WIDTH  0.7f
#define SIXTH_ARENA_LENGTH  5.0f
#define BRIDGE5_LENGTH      7.5f
#define BRIDGE5_HALF_WIDTH  0.65f
#define SEVENTH_ARENA_LENGTH 4.5f
#define BRIDGE6_HALF_WIDTH  0.40f
#define BRIDGE6_SEG1_LENGTH 4.0f
#define BRIDGE6_SEG2_LENGTH 7.5f
#define BRIDGE6_SEG3_LENGTH 10.0f
#define BRIDGE6_SEG4_LENGTH 9.0f
#define BRIDGE6_SEG5_LENGTH 4.0f
#define BRIDGE6_SEG6_LENGTH 4.0f
#define BRIDGE6_SEG7_LENGTH 6.0f
#define EIGHTH_ARENA_HALF   2.6f
#define EIGHTH_ARENA_FORWARD_OFFSET 1.2f
#define FLAG_TOUCH_RADIUS   0.75f
#define WIN_STAR_COUNT      12

#define BUILDING_BOTTOM  -20.0f
#define SKY_SIZE          80.0f
#define FLOOR_DEATH      -18.5f

inline void getRamp2Layout(float &minX, float &maxX, float &z0, float &z1) {
    float bridgeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;
    float thirdMinX = LOWER_ARENA_HALF + BRIDGE2_LENGTH;
    float thirdMaxX = thirdMinX + THIRD_ARENA_HALF * 2.0f;
    float thirdMinZ = bridgeCenterZ - THIRD_ARENA_HALF;

    minX = thirdMinX;
    maxX = thirdMaxX;
    z0 = thirdMinZ;
    z1 = thirdMinZ - RAMP2_LENGTH;
}

inline void getFourthArenaLayout(float &minX, float &maxX, float &minZ, float &maxZ) {
    float bridgeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;
    float thirdMinX = LOWER_ARENA_HALF + BRIDGE2_LENGTH;
    float thirdCenterX = thirdMinX + THIRD_ARENA_HALF;
    float thirdMinZ = bridgeCenterZ - THIRD_ARENA_HALF;

    float fourthHalf = FOURTH_ARENA_HALF;
    minX = thirdCenterX - fourthHalf;
    maxX = thirdCenterX + fourthHalf;
    maxZ = thirdMinZ - RAMP2_LENGTH;
    minZ = maxZ - fourthHalf * 2.0f;
}

struct Barrier {
    float x, z;
    float hw, hd;
    float floorY;
};

struct MovingBarrier {
    float x;
    float gapZ;
    float speed;
};

struct Star {
    float x, z;
    float spawnTime;
    bool collected;
};


struct Bridge5Barrier {
    float posX;
    float centerZ;
    float speed;
    int   dir;
    float minX;
    float maxX;
};


struct Arena5MovingBarrier {
    float x;       
    float z;       
    float speed;   
    float minX;    
    float maxX;    
};

enum GameState { PLAYING, GAMEOVER, WIN };

extern std::vector<Barrier> barriers;
extern std::vector<MovingBarrier> movingBarriers;
extern std::vector<Star> stars;
extern std::vector<Bridge5Barrier> bridge5Barriers;
extern std::vector<Arena5MovingBarrier> arena5MovingBarriers;

extern float starAngle;
extern GameState gameState;

extern float ballX;
extern float ballZ;
extern float velX;
extern float velZ;
extern float ballY;
extern float velY;
extern bool falling;

extern float timeLeft;
extern int lastTime;
extern int score;
extern float lastSpawnTime;

extern bool keyW;
extern bool keyS;
extern bool keyA;
extern bool keyD;

void resetGame();
