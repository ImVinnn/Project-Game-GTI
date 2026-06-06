#include "Star.h"
#include "Lighting.h"
#include "Physics.h"
#include <GL/glut.h>


static void randomArenaPosition(int area, float &outX, float &outZ) {
    float margin = 1.0f;
    if(area == 1) {
        float minX = -LOWER_ARENA_HALF + margin;
        float maxX =  LOWER_ARENA_HALF - margin;
        float minZ = ARENA_HALF + RAMP_LENGTH + margin;
        float maxZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF * 2.0f - margin;
        outX = minX + static_cast<float>(rand()) / RAND_MAX * (maxX - minX);
        outZ = minZ + static_cast<float>(rand()) / RAND_MAX * (maxZ - minZ);
    } else if(area == 2) {
        float minX = LOWER_ARENA_HALF + BRIDGE2_LENGTH + margin;
        float maxX = LOWER_ARENA_HALF + BRIDGE2_LENGTH + THIRD_ARENA_HALF * 2.0f - margin;
        float centerZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;
        float minZ = centerZ - THIRD_ARENA_HALF + margin;
        float maxZ = centerZ + THIRD_ARENA_HALF - margin;
        outX = minX + static_cast<float>(rand()) / RAND_MAX * (maxX - minX);
        outZ = minZ + static_cast<float>(rand()) / RAND_MAX * (maxZ - minZ);
    } else if (area == 3) {
        float fourthMinX;
        float fourthMaxX;
        float fourthMinZ;
        float fourthMaxZ;
        getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);

        fourthMinX += margin;
        fourthMaxX -= margin;
        fourthMinZ += margin;
        float fourthMaxZm = fourthMaxZ - margin;
        outX = fourthMinX + static_cast<float>(rand()) / RAND_MAX * (fourthMaxX - fourthMinX);
        outZ = fourthMinZ + static_cast<float>(rand()) / RAND_MAX * (fourthMaxZm - fourthMinZ);
    } else if (area == 4) {
        
        float fourthMinX;
        float fourthMaxX;
        float fourthMinZ;
        float fourthMaxZ;
        getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);
        float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;
        float fifthHalf     = FIFTH_ARENA_HALF;
        float fifthMinX     = fourthMaxX + BRIDGE3_LENGTH + margin;
        float fifthMaxX     = fifthMinX + fifthHalf * 2.0f - margin;
        float fifthMinZ     = fourthCenterZ - fifthHalf + margin;
        float fifthMaxZ     = fourthCenterZ + fifthHalf - margin;
        outX = fifthMinX + static_cast<float>(rand()) / RAND_MAX * (fifthMaxX - fifthMinX);
        outZ = fifthMinZ + static_cast<float>(rand()) / RAND_MAX * (fifthMaxZ - fifthMinZ);
    } else if (area == 5) {
        
        float fourthMinX;
        float fourthMaxX;
        float fourthMinZ;
        float fourthMaxZ;
        getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);
        float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;
        float fifthHalf     = FIFTH_ARENA_HALF;
        float fifthMinX     = fourthMaxX + BRIDGE3_LENGTH;
        float fifthMaxX     = fifthMinX + fifthHalf * 2.0f;
        float fifthMinZ     = fourthCenterZ - fifthHalf;
        float bridge4MinZ   = fifthMinZ - BRIDGE4_LENGTH;
        float sixthMinX     = fifthMinX + margin;
        float sixthMaxX     = fifthMaxX - margin;
        float sixthMinZ     = bridge4MinZ - SIXTH_ARENA_LENGTH + margin;
        float sixthMaxZ     = bridge4MinZ - margin;
        outX = sixthMinX + static_cast<float>(rand()) / RAND_MAX * (sixthMaxX - sixthMinX);
        outZ = sixthMinZ + static_cast<float>(rand()) / RAND_MAX * (sixthMaxZ - sixthMinZ);
    } else if (area == 6) {
        
        float fourthMinX;
        float fourthMaxX;
        float fourthMinZ;
        float fourthMaxZ;
        getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);
        float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;
        float fifthHalf     = FIFTH_ARENA_HALF;
        float fifthMinX     = fourthMaxX + BRIDGE3_LENGTH;
        float fifthMaxX     = fifthMinX + fifthHalf * 2.0f;
        float fifthMinZ     = fourthCenterZ - fifthHalf;
        float bridge4MinZ   = fifthMinZ - BRIDGE4_LENGTH;
        float sixthMinZ     = bridge4MinZ - SIXTH_ARENA_LENGTH;
        float bridge5MinZ   = sixthMinZ - BRIDGE5_LENGTH;
        float seventhMinX   = fifthMinX + margin;
        float seventhMaxX   = fifthMaxX - margin;
        float seventhMinZ   = bridge5MinZ - SEVENTH_ARENA_LENGTH + margin;
        float seventhMaxZ   = bridge5MinZ - margin;
        outX = seventhMinX + static_cast<float>(rand()) / RAND_MAX * (seventhMaxX - seventhMinX);
        outZ = seventhMinZ + static_cast<float>(rand()) / RAND_MAX * (seventhMaxZ - seventhMinZ);
    }
}

static bool randomValidArenaPosition(int area, float &outX, float &outZ) {
    for (int i = 0; i < 50; i++) {
        randomArenaPosition(area, outX, outZ);

        if (getGroundHeight(outX, outZ) > -1e5f) {
            return true;
        }
    }

    return false;
}

static bool isFarEnoughFromOtherStars(float x, float z) {
    float minDist = 1.35f;

    for (std::vector<Star>::const_iterator it = stars.begin(); it != stars.end(); ++it) {
        float dx = x - it->x;
        float dz = z - it->z;

        if (dx * dx + dz * dz < minDist * minDist) {
            return false;
        }
    }

    return true;
}

static bool randomValidSeparatedArenaPosition(int area, float &outX, float &outZ) {
    for (int i = 0; i < 80; i++) {
        if (!randomValidArenaPosition(area, outX, outZ)) {
            continue;
        }

        if (isFarEnoughFromOtherStars(outX, outZ)) {
            return true;
        }
    }

    return false;
}

void spawnInitialStar() {
    const int firstPlayableArena = 1;
    const int lastPlayableArena  = 6;
    const int starsPerArena      = 2;

    for (int area = firstPlayableArena; area <= lastPlayableArena; area++) {
        for (int i = 0; i < starsPerArena; i++) {
            Star newStar;
            if (!randomValidSeparatedArenaPosition(area, newStar.x, newStar.z) &&
                !randomValidArenaPosition(area, newStar.x, newStar.z)) {
                continue;
            }

            newStar.spawnTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
            newStar.collected = false;
            stars.push_back(newStar);
        }
    }
}

void drawStar3D(float size) {
    int points = 5;
    float outer = size;
    float inner = size * 0.5f;
    float depth = size * 0.3f;

    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0, 0, 1);
        glVertex3f(0, 0, depth);
        for(int i = 0; i <= points * 2; i++) {
            float angle = i * PI / points;
            float r = (i % 2 == 0) ? outer : inner;
            glVertex3f(cos(angle)*r, sin(angle)*r, depth);
        }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
        glNormal3f(0, 0, -1);
        glVertex3f(0, 0, -depth);
        for(int i = 0; i <= points * 2; i++) {
            float angle = i * PI / points;
            float r = (i % 2 == 0) ? outer : inner;
            glVertex3f(cos(angle)*r, sin(angle)*r, -depth);
        }
    glEnd();

    glBegin(GL_QUAD_STRIP);
        for(int i = 0; i <= points * 2; i++) {
            float angle = i * PI / points;
            float r = (i % 2 == 0) ? outer : inner;
            float sx = cos(angle)*r;
            float sy = sin(angle)*r;
            glNormal3f(sx, sy, 0);
            glVertex3f(sx, sy, depth);
            glVertex3f(sx, sy, -depth);
        }
    glEnd();
}

void drawStar(float x, float y, float z) {
    glPushMatrix();
        glTranslatef(x, y, z);
        glRotatef(starAngle, 0, 1, 0);
        glColor3f(1.0f, 0.8f, 0.2f);
        drawStar3D(0.5f);
    glPopMatrix();
}
void drawStarsShadow() {
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glColor4f(0.0f, 0.0f, 0.0f, 0.32f);

    for (std::vector<Star>::const_iterator it = stars.begin(); it != stars.end(); ++it) {
        const Star& star = *it;
        if (star.collected) continue;

        float groundY = getGroundHeight(star.x, star.z);
        if (groundY < -1e5f) continue;

        float starY = groundY + BALL_RADIUS + 0.3f;
        float shadowY = groundY + 0.018f;
        float shadowMat[16];
        buildSceneShadowMatrix(shadowMat, shadowY);

        glPushMatrix();
            glMultMatrixf(shadowMat);
            glTranslatef(star.x, starY, star.z);
            glRotatef(starAngle, 0, 1, 0);
            drawStar3D(0.5f);
        glPopMatrix();
    }

    glDepthMask(GL_TRUE);
    glPopAttrib();
}
