#include "GameLogic.h"
#include "Lighting.h"
#include <GL/glut.h>
#include <cmath>
#include <cstdio>

float starAngle = 0.0f;
GameState gameState = PLAYING;

float ballX = 0.0f;
float ballZ = 0.0f;
float velX  = 0.0f;
float velZ  = 0.0f;
float ballY = 0.0f;
float velY = 0.0f;
bool falling = false;
bool unrecoverableFall = false;

float timeLeft = 120.0f;
int score = 0;
int lastTime = 0;
float lastSpawnTime = 0.0f;

bool keyW = false;
bool keyS = false;
bool keyA = false;
bool keyD = false;

void resetGame() {
    ballX = 0.0f;
    ballZ = 0.0f;
    velX = 0.0f;
    velZ = 0.0f;
    ballY = 0.0f;
    velY = 0.0f;
    falling = false;
    unrecoverableFall = false;

    timeLeft = 120.0f;
    score = 0;

    stars.clear();
    initBarriers();
    initMovingBarriers();
    initBridge5Barriers();
    initArena5MovingBarriers();
    initArena7FallingSpike();
    spawnInitialStar();

    gameState = PLAYING;

    keyW = keyS = keyA = keyD = false;

    lastTime = glutGet(GLUT_ELAPSED_TIME);
    lastSpawnTime = lastTime / 1000.0f;
}

void setCamera() {
    float camX = ballX;
    float camZ = ballZ + 8.0f;
    float camY = ballY + 5.0f;
    float targetY = ballY;

    gluLookAt(camX, camY, camZ,
              ballX, targetY, ballZ,
              0, 1, 0);
}

void getFlagPosition(float &flagX, float &flagY, float &flagZ) {
    float bridgeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;

    float thirdMinX = LOWER_ARENA_HALF + BRIDGE2_LENGTH;
    float thirdCenterX = thirdMinX + THIRD_ARENA_HALF;
    float thirdMinZ = bridgeCenterZ - THIRD_ARENA_HALF;

    float fourthMinX;
    float fourthMaxX;
    float fourthMinZ;
    float fourthMaxZ;
    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);

    float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;

    float bridge3MaxX = fourthMaxX + BRIDGE3_LENGTH;

    float fifthHalf = FIFTH_ARENA_HALF;
    float fifthMinX = bridge3MaxX;
    float fifthMaxX = fifthMinX + fifthHalf * 2.0f;
    float fifthMinZ = fourthCenterZ - fifthHalf;

    
    float bridge4MaxZ = fifthMinZ;
    float bridge4MinZ = fifthMinZ - BRIDGE4_LENGTH;

    
    float sixthMinX = fifthMinX;
    float sixthMaxX = fifthMaxX;
    float sixthMinZ = bridge4MinZ - SIXTH_ARENA_LENGTH;

    
    float bridge5MinZ = sixthMinZ - BRIDGE5_LENGTH;

    float seventhMinX = sixthMinX;
    float seventhMaxX = sixthMaxX;
    float seventhMaxZ = bridge5MinZ;
    float seventhMinZ = bridge5MinZ - SEVENTH_ARENA_LENGTH;

    
    float seventhCenterZ = (seventhMinZ + seventhMaxZ) * 0.5f;
    float bridge6Seg1MinX = seventhMinX - BRIDGE6_SEG1_LENGTH;
    float bridge6Seg3MinX = bridge6Seg1MinX - BRIDGE6_SEG3_LENGTH;
    float bridge6Seg5MinX = bridge6Seg3MinX - BRIDGE6_SEG5_LENGTH;
    float bridge6Seg7MinX = bridge6Seg5MinX - BRIDGE6_SEG7_LENGTH;
    float bridge6Seg7CenterZ = seventhCenterZ + BRIDGE6_SEG2_LENGTH - BRIDGE6_SEG4_LENGTH + BRIDGE6_SEG6_LENGTH;

    float eighthMaxX = bridge6Seg7MinX;
    float eighthMinX = eighthMaxX - EIGHTH_ARENA_HALF * 2.0f;
    float eighthCenterZ = bridge6Seg7CenterZ - EIGHTH_ARENA_FORWARD_OFFSET;
    float eighthMinZ = eighthCenterZ - EIGHTH_ARENA_HALF;
    float eighthMaxZ = eighthCenterZ + EIGHTH_ARENA_HALF;

    flagX = (eighthMinX + eighthMaxX) * 0.5f;
    flagZ = (eighthMinZ + eighthMaxZ) * 0.5f;
    flagY = FOURTH_Y - BALL_RADIUS;
}

void resolveBallBarrierCollision(const Barrier& b, float pushX) {
    if (ballY > b.floorY + 0.8f) {
        return;
    }

    float bx0 = b.x - b.hw - BALL_RADIUS;
    float bx1 = b.x + b.hw + BALL_RADIUS;
    float bz0 = b.z - b.hd - BALL_RADIUS;
    float bz1 = b.z + b.hd + BALL_RADIUS;

    if (!(ballX > bx0 && ballX < bx1 && ballZ > bz0 && ballZ < bz1)) {
        return;
    }

    float dLeft  = ballX - bx0;
    float dRight = bx1 - ballX;
    float dFront = ballZ - bz0;
    float dBack  = bz1 - ballZ;

    float minD = dLeft;
    int side = 0;

    if (dRight < minD) {
        minD = dRight;
        side = 1;
    }

    if (dFront < minD) {
        minD = dFront;
        side = 2;
    }

    if (dBack < minD) {
        minD = dBack;
        side = 3;
    }

    if (side == 0) {
        ballX = bx0;
        velX = -std::fabs(velX) * 0.5f;
    }

    if (side == 1) {
        ballX = bx1;
        velX = std::fabs(velX) * 0.5f + pushX;
    }

    if (side == 2) {
        ballZ = bz0;
        velZ = -std::fabs(velZ) * 0.5f;
    }

    if (side == 3) {
        ballZ = bz1;
        velZ = std::fabs(velZ) * 0.5f;
    }
}

void update() {
    if (gameState != PLAYING) {
        return;
    }

    starAngle += 1.0f;

    updateMovingBarriers();
    updateBridge5Barriers();
    updateArena5MovingBarriers();
    updateArena6Spikes();
    updateArena7FallingSpike();

    if (keyW) velZ -= ACCEL;
    if (keyS) velZ += ACCEL;
    if (keyA) velX -= ACCEL;
    if (keyD) velX += ACCEL;

    
    if (!falling) {
        float sfX = 0.0f;
        float sfZ = 0.0f;

        getSlopeForce(ballX, ballZ, sfX, sfZ);

        velX += sfX;
        velZ += sfZ;
    }

    velX *= FRICTION;
    velZ *= FRICTION;

    ballX += velX;
    ballZ += velZ;

    resolveWorldSolidCollision(ballX, ballZ, ballY, velX, velZ);

    updateBallRotation(velX, velZ);

    
    for (std::vector<Star>::iterator it = stars.begin(); it != stars.end(); ++it) {
        Star& star = *it;

        if (!star.collected) {
            float dx = ballX - star.x;
            float dz = ballZ - star.z;
            float dist = std::sqrt(dx * dx + dz * dz);

            float starY = getGroundHeight(star.x, star.z) + BALL_RADIUS + 0.3f;
            float dy = ballY - starY;

            if (dist < BALL_RADIUS + 0.5f && std::fabs(dy) < 1.2f) {
                star.collected = true;
                score += 10;
            }
        }
    }

    
    float groundY = getGroundHeight(ballX, ballZ);

    if (isInLowerHole(ballX, ballZ)) {
        groundY = -1e6f;
    }

    bool noGroundUnderBall = groundY <= -1e5f;

    if (noGroundUnderBall) {
        unrecoverableFall = true;
    }

    if (!unrecoverableFall &&
        groundY > -1e5f &&
        ballY <= groundY + BALL_RADIUS + 0.05f) {
        falling = false;
        velY = 0.0f;
        ballY = groundY + BALL_RADIUS;
    } else {
        if (!falling) {
            falling = true;
            velY = 0.0f;
        }

        velY -= GRAVITY;
        ballY += velY;

        if (ballY <= FLOOR_DEATH) {
            ballY = FLOOR_DEATH;
            gameState = GAMEOVER;
        }
    }

    
    for (size_t i = 0; i < barriers.size(); i++) {
        resolveBallBarrierCollision(barriers[i], 0.0f);
    }

    
    for (size_t i = 0; i < movingBarriers.size(); i++) {
        Barrier top, bottom;
        makeMovingBarrierBlocks(movingBarriers[i], top, bottom);

        if (top.hd > 0.05f) {
            resolveBallBarrierCollision(top, movingBarriers[i].speed);
        }

        if (bottom.hd > 0.05f) {
            resolveBallBarrierCollision(bottom, movingBarriers[i].speed);
        }
    }

    
    resolveBridge5BarrierCollisions();
    resolveArena5BarrierCollisions();
    resolveArena6SpikeCollisions();
    resolveArena7FallingSpikeCollision();

    
    if (gameState == PLAYING) {
        float flagX, flagY, flagZ;

        getFlagPosition(flagX, flagY, flagZ);

        float dxFlag = ballX - flagX;
        float dzFlag = ballZ - flagZ;
        float distFlag = std::sqrt(dxFlag * dxFlag + dzFlag * dzFlag);

        if (score >= WIN_STAR_COUNT * 10 &&
            distFlag <= FLAG_TOUCH_RADIUS &&
            ballY >= flagY &&
            ballY <= flagY + 2.0f) {
            gameState = WIN;
        }
    }

    
    if (gameState == PLAYING && timeLeft <= 0) {
        gameState = GAMEOVER;
    }

    glutPostRedisplay();
}

void timerCallback(int val) {
    if (gameState == PLAYING && !falling) {
        int now = glutGet(GLUT_ELAPSED_TIME);
        float dt = (now - lastTime) / 1000.0f;

        lastTime = now;
        timeLeft -= dt;

        if (timeLeft < 0) {
            timeLeft = 0;
        }
    }

    update();

    glutTimerFunc(16, timerCallback, 0);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    setCamera();
    applySceneLight();

    drawSky();
    drawBottomFloor();
    drawArena();
    drawMovingBarriers();
    drawArena5MovingBarriers();
    drawBridge5Barriers();
    drawArena6Spikes();
    drawArena7FallingSpike();
    drawBallShadow();
    drawStarsShadow();
    drawBall();

    for (std::vector<Star>::const_iterator it = stars.begin(); it != stars.end(); ++it) {
        const Star& star = *it;

        if (!star.collected) {
            float sy = getGroundHeight(star.x, star.z) + BALL_RADIUS + 0.3f;
            drawStar(star.x, sy, star.z);
        }
    }

    drawHUD();

    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) {
        h = 1;
    }

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(60.0f, (float)w / h, 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);
}

void keyDown(unsigned char key, int x, int y) {
    if (key == 'w' || key == 'W') keyW = true;
    if (key == 's' || key == 'S') keyS = true;
    if (key == 'a' || key == 'A') keyA = true;
    if (key == 'd' || key == 'D') keyD = true;

    if ((key == 'r' || key == 'R') && gameState != PLAYING) {
        resetGame();
    }
}

void keyUp(unsigned char key, int x, int y) {
    if (key == 'w' || key == 'W') keyW = false;
    if (key == 's' || key == 'S') keyS = false;
    if (key == 'a' || key == 'A') keyA = false;
    if (key == 'd' || key == 'D') keyD = false;
}

void specialDown(int key, int x, int y) {
    if (key == GLUT_KEY_UP)    keyW = true;
    if (key == GLUT_KEY_DOWN)  keyS = true;
    if (key == GLUT_KEY_LEFT)  keyA = true;
    if (key == GLUT_KEY_RIGHT) keyD = true;
}

void specialUp(int key, int x, int y) {
    if (key == GLUT_KEY_UP)    keyW = false;
    if (key == GLUT_KEY_DOWN)  keyS = false;
    if (key == GLUT_KEY_LEFT)  keyA = false;
    if (key == GLUT_KEY_RIGHT) keyD = false;
}
