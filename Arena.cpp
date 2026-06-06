#include "Arena.h"
#include "Barrier.h"
#include "Building.h"
#include "Lighting.h"
#include <GL/glut.h>


struct Color3 { float r, g, b; };

static void setColor(const Color3& c) { glColor3f(c.r, c.g, c.b); }
static void vertex(float x, float y, float z) { glVertex3f(x, y, z); }


void initGL()
{
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    applySceneLight();

    initBarriers();
}


//  Langit & lantai dasar
void drawSky()
{
    const float s    = SKY_SIZE ;
    const float yTop = s * 0.8f;
    const float yBot = BUILDING_BOTTOM - 1.0f;

    
    const float r0 = 0.53f, g0 = 0.81f, b0 = 1.0f;  
    const float r1 = 0.85f, g1 = 0.93f, b1 = 1.0f;  

    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);

    
    glColor3f(r0, g0, b0); glVertex3f(-s, yTop, -s);
    glColor3f(r0, g0, b0); glVertex3f( s, yTop, -s);
    glColor3f(r1, g1, b1); glVertex3f( s, yBot, -s);
    glColor3f(r1, g1, b1); glVertex3f(-s, yBot, -s);

    glColor3f(r0, g0, b0); glVertex3f( s, yTop,  s);
    glColor3f(r0, g0, b0); glVertex3f(-s, yTop,  s);
    glColor3f(r1, g1, b1); glVertex3f(-s, yBot,  s);
    glColor3f(r1, g1, b1); glVertex3f( s, yBot,  s);

    glColor3f(r0, g0, b0); glVertex3f(-s, yTop,  s);
    glColor3f(r0, g0, b0); glVertex3f(-s, yTop, -s);
    glColor3f(r1, g1, b1); glVertex3f(-s, yBot, -s);
    glColor3f(r1, g1, b1); glVertex3f(-s, yBot,  s);

    glColor3f(r0, g0, b0); glVertex3f( s, yTop, -s);
    glColor3f(r0, g0, b0); glVertex3f( s, yTop,  s);
    glColor3f(r1, g1, b1); glVertex3f( s, yBot,  s);
    glColor3f(r1, g1, b1); glVertex3f( s, yBot, -s);

    
    glColor3f(0.4f, 0.75f, 0.95f);
    glVertex3f(-s, yTop, -s);
    glVertex3f( s, yTop, -s);
    glVertex3f( s, yTop,  s);
    glVertex3f(-s, yTop,  s);

    glEnd();
    glEnable(GL_LIGHTING);
}

void drawBottomFloor()
{
    const float s        = SKY_SIZE * 1.0f;
    const float floorY   = BUILDING_BOTTOM;
    const int   grid     = 20;
    const float tileSize = (s * 2.0f) / grid;

    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    for (int i = 0; i < grid; i++)
    {
        for (int j = 0; j < grid; j++)
        {
            float x0 = -s + i * tileSize,  z0 = -s + j * tileSize;
            float x1 = x0 + tileSize,      z1 = z0 + tileSize;

            if ((i + j) % 2 == 0) glColor3f(0.50f, 0.95f, 0.50f);
            else                   glColor3f(1.00f, 1.00f, 0.85f);

            glVertex3f(x0, floorY, z0); glVertex3f(x1, floorY, z0);
            glVertex3f(x1, floorY, z1); glVertex3f(x0, floorY, z1);
        }
    }
    glEnd();
}



static void drawBlockSides(float minX, float maxX, float minZ, float maxZ,
                            float topY, float bottomY, const Color3& sideColor)
{
    Color3 dark = {
        sideColor.r * 0.65f,
        sideColor.g * 0.65f,
        sideColor.b * 0.65f
    };

    glBegin(GL_QUADS);
    setColor(sideColor);

    glNormal3f( 0, 0, -1);
    vertex(minX, topY,    minZ); vertex(maxX, topY,    minZ);
    vertex(maxX, bottomY, minZ); vertex(minX, bottomY, minZ);

    glNormal3f( 0, 0,  1);
    vertex(maxX, topY,    maxZ); vertex(minX, topY,    maxZ);
    vertex(minX, bottomY, maxZ); vertex(maxX, bottomY, maxZ);

    setColor(dark);

    glNormal3f(-1, 0,  0);
    vertex(minX, topY,    maxZ); vertex(minX, topY,    minZ);
    vertex(minX, bottomY, minZ); vertex(minX, bottomY, maxZ);

    glNormal3f( 1, 0,  0);
    vertex(maxX, topY,    minZ); vertex(maxX, topY,    maxZ);
    vertex(maxX, bottomY, maxZ); vertex(maxX, bottomY, minZ);

    glEnd();
}


static void drawHoleWalls(float hMinX, float hMaxX, float hMinZ, float hMaxZ,
                           float topY, float bottomY, const Color3& color)
{
    setColor(color);
    glBegin(GL_QUADS);

    glNormal3f( 0, 0,  1);
    vertex(hMinX, topY,    hMinZ); vertex(hMaxX, topY,    hMinZ);
    vertex(hMaxX, bottomY, hMinZ); vertex(hMinX, bottomY, hMinZ);

    glNormal3f( 0, 0, -1);
    vertex(hMaxX, topY,    hMaxZ); vertex(hMinX, topY,    hMaxZ);
    vertex(hMinX, bottomY, hMaxZ); vertex(hMaxX, bottomY, hMaxZ);

    glNormal3f( 1, 0,  0);
    vertex(hMinX, topY,    hMaxZ); vertex(hMinX, topY,    hMinZ);
    vertex(hMinX, bottomY, hMinZ); vertex(hMinX, bottomY, hMaxZ);

    glNormal3f(-1, 0,  0);
    vertex(hMaxX, topY,    hMinZ); vertex(hMaxX, topY,    hMaxZ);
    vertex(hMaxX, bottomY, hMaxZ); vertex(hMaxX, bottomY, hMinZ);

    glEnd();
}


static void drawCheckeredTop(float minX, float maxX, float minZ, float maxZ,
                              float y, int tiles,
                              const Color3& colorA, const Color3& colorB)
{
    float tileX = (maxX - minX) / tiles;
    float tileZ = (maxZ - minZ) / tiles;

    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    for (int i = 0; i < tiles; i++)
    {
        for (int j = 0; j < tiles; j++)
        {
            float x0 = minX + i * tileX, z0 = minZ + j * tileZ;
            float x1 = x0 + tileX,       z1 = z0 + tileZ;

            if ((i + j) % 2 == 0) setColor(colorA);
            else                   setColor(colorB);

            vertex(x0, y, z0); vertex(x1, y, z0);
            vertex(x1, y, z1); vertex(x0, y, z1);
        }
    }
    glEnd();
}

static void drawCheckeredTopWithHole(float minX, float maxX, float minZ, float maxZ,
                                      float y, int tiles,
                                      const Color3& colorA, const Color3& colorB,
                                      float hMinX, float hMaxX,
                                      float hMinZ, float hMaxZ)
{
    float tileX = (maxX - minX) / tiles;
    float tileZ = (maxZ - minZ) / tiles;

    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    for (int i = 0; i < tiles; i++)
    {
        for (int j = 0; j < tiles; j++)
        {
            float x0 = minX + i * tileX, z0 = minZ + j * tileZ;
            float x1 = x0 + tileX,       z1 = z0 + tileZ;

            bool inHole = (x0 >= hMinX && x1 <= hMaxX &&
                           z0 >= hMinZ && z1 <= hMaxZ);
            if (inHole) continue;

            if ((i + j) % 2 == 0) setColor(colorA);
            else                   setColor(colorB);

            vertex(x0, y, z0); vertex(x1, y, z0);
            vertex(x1, y, z1); vertex(x0, y, z1);
        }
    }
    glEnd();
}


static float getSlopedHoleY(float x, float z,
                             float minX, float maxX, float minZ, float maxZ,
                             float topY, float holeTopY,
                             float hMinX, float hMaxX, float hMinZ, float hMaxZ)
{
    float tx = 1.0f, tz = 1.0f;
    if      (x < hMinX) tx = (x - minX)  / (hMinX - minX);
    else if (x > hMaxX) tx = (maxX - x)  / (maxX - hMaxX);
    if      (z < hMinZ) tz = (z - minZ)  / (hMinZ - minZ);
    else if (z > hMaxZ) tz = (maxZ - z)  / (maxZ - hMaxZ);

    tx = (tx < 0.0f) ? 0.0f : (tx > 1.0f) ? 1.0f : tx;
    tz = (tz < 0.0f) ? 0.0f : (tz > 1.0f) ? 1.0f : tz;

    float t = (tz < tx) ? tz : tx;
    return topY - (topY - holeTopY) * t;
}

static void drawSlopedHoleTop(float minX, float maxX, float minZ, float maxZ,
                               float topY, float holeTopY,
                               float hMinX, float hMaxX, float hMinZ, float hMaxZ,
                               const Color3& colorA, const Color3& colorB)
{
    const int tiles = 24;
    float tileX = (maxX - minX) / tiles;
    float tileZ = (maxZ - minZ) / tiles;

    glBegin(GL_QUADS);
    for (int i = 0; i < tiles; i++)
    {
        for (int j = 0; j < tiles; j++)
        {
            float x0 = minX + i * tileX, z0 = minZ + j * tileZ;
            float x1 = x0 + tileX,       z1 = z0 + tileZ;

            bool inHole = (x0 >= hMinX && x1 <= hMaxX &&
                           z0 >= hMinZ && z1 <= hMaxZ);
            if (inHole) continue;

            float y00 = getSlopedHoleY(x0, z0, minX, maxX, minZ, maxZ, topY, holeTopY, hMinX, hMaxX, hMinZ, hMaxZ);
            float y10 = getSlopedHoleY(x1, z0, minX, maxX, minZ, maxZ, topY, holeTopY, hMinX, hMaxX, hMinZ, hMaxZ);
            float y11 = getSlopedHoleY(x1, z1, minX, maxX, minZ, maxZ, topY, holeTopY, hMinX, hMaxX, hMinZ, hMaxZ);
            float y01 = getSlopedHoleY(x0, z1, minX, maxX, minZ, maxZ, topY, holeTopY, hMinX, hMaxX, hMinZ, hMaxZ);

            float nx = -(y10 - y00), nz = -(y01 - y00);
            glNormal3f(nx, 1.0f, nz);

            if ((i + j) % 2 == 0) setColor(colorA);
            else                   setColor(colorB);

            vertex(x0, y00, z0); vertex(x1, y10, z0);
            vertex(x1, y11, z1); vertex(x0, y01, z1);
        }
    }
    glEnd();
}



void drawArenaBlock(float minX, float maxX, float minZ, float maxZ,
                    float topY, float bottomY,
                    const Color3& topA, const Color3& topB,
                    const Color3& sideColor)
{
    drawCheckeredTop(minX, maxX, minZ, maxZ, topY, 24, topA, topB);
    drawBlockSides  (minX, maxX, minZ, maxZ, topY, bottomY, sideColor);
}


void drawArenaBlockWithHole(float minX, float maxX, float minZ, float maxZ,
                             float topY, float bottomY,
                             const Color3& topA, const Color3& topB,
                             const Color3& sideColor,
                             float hMinX, float hMaxX, float hMinZ, float hMaxZ)
{
    static const Color3 holeColor = { 0.12f, 0.06f, 0.03f };
    drawCheckeredTopWithHole(minX, maxX, minZ, maxZ, topY, 24,
                             topA, topB, hMinX, hMaxX, hMinZ, hMaxZ);
    drawBlockSides (minX, maxX, minZ, maxZ, topY, bottomY, sideColor);
    drawHoleWalls  (hMinX, hMaxX, hMinZ, hMaxZ, topY, bottomY, holeColor);
}


void drawArenaBlockWithSlopedHole(float minX, float maxX, float minZ, float maxZ,
                                   float topY, float bottomY,
                                   const Color3& topA, const Color3& topB,
                                   const Color3& sideColor,
                                   float hMinX, float hMaxX, float hMinZ, float hMaxZ,
                                   float slopeDepth)
{
    static const Color3 holeWallColor = { 0.12f, 0.06f, 0.03f };
    float holeTopY = topY - slopeDepth;
    drawSlopedHoleTop(minX, maxX, minZ, maxZ, topY, holeTopY,
                      hMinX, hMaxX, hMinZ, hMaxZ, topA, topB);
    drawBlockSides(minX, maxX, minZ, maxZ, topY, bottomY, sideColor);
    drawHoleWalls (hMinX, hMaxX, hMinZ, hMaxZ, holeTopY, bottomY, holeWallColor);
}


static bool isInsideRect(float x0, float x1, float z0, float z1,
                          float minX, float maxX, float minZ, float maxZ)
{
    return (x0 >= minX && x1 <= maxX && z0 >= minZ && z1 <= maxZ);
}

void drawArenaBlockWithSmallHoles(float minX, float maxX, float minZ, float maxZ,
                                   float topY, float bottomY,
                                   const Color3& topA, const Color3& topB,
                                   const Color3& sideColor,
                                   const float holes[][4], int holeCount)
{
    static const Color3 holeColor = { 0.12f, 0.06f, 0.03f };
    const int   tiles = 24;
    const float tileX = (maxX - minX) / tiles;
    const float tileZ = (maxZ - minZ) / tiles;

    glNormal3f(0, 1, 0);
    glBegin(GL_QUADS);
    for (int i = 0; i < tiles; i++)
    {
        for (int j = 0; j < tiles; j++)
        {
            float x0 = minX + i * tileX, z0 = minZ + j * tileZ;
            float x1 = x0 + tileX,       z1 = z0 + tileZ;

            bool inHole = false;
            for (int h = 0; h < holeCount && !inHole; h++)
                inHole = isInsideRect(x0, x1, z0, z1,
                                      holes[h][0], holes[h][1],
                                      holes[h][2], holes[h][3]);
            if (inHole) continue;

            if ((i + j) % 2 == 0) setColor(topA);
            else                   setColor(topB);

            vertex(x0, topY, z0); vertex(x1, topY, z0);
            vertex(x1, topY, z1); vertex(x0, topY, z1);
        }
    }
    glEnd();

    drawBlockSides(minX, maxX, minZ, maxZ, topY, bottomY, sideColor);
    for (int h = 0; h < holeCount; h++)
        drawHoleWalls(holes[h][0], holes[h][1], holes[h][2], holes[h][3],
                      topY, bottomY, holeColor);
}



void drawBridgeBlock(float minX, float maxX, float minZ, float maxZ,
                     float topY, float bottomY,
                     const Color3& topColor, const Color3& sideColor)
{
    setColor(topColor);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    vertex(minX, topY, minZ); vertex(maxX, topY, minZ);
    vertex(maxX, topY, maxZ); vertex(minX, topY, maxZ);
    glEnd();

    drawBlockSides(minX, maxX, minZ, maxZ, topY, bottomY, sideColor);
}

void drawRampBlock(float minX, float maxX,
                   float z0,  float z1,
                   float y0,  float y1, float bottomY,
                   const Color3& topColor, const Color3& sideColor)
{
    
    setColor(topColor);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    vertex(minX, y0, z0); vertex(maxX, y0, z0);
    vertex(maxX, y1, z1); vertex(minX, y1, z1);
    glEnd();

    
    setColor(sideColor);
    glBegin(GL_QUADS);
    glNormal3f(-1, 0,  0);
    vertex(minX, y0,      z0); vertex(minX, y1,      z1);
    vertex(minX, bottomY, z1); vertex(minX, bottomY, z0);

    glNormal3f( 1, 0,  0);
    vertex(maxX, y1,      z1); vertex(maxX, y0,      z0);
    vertex(maxX, bottomY, z0); vertex(maxX, bottomY, z1);

    glNormal3f( 0, 0, -1);
    vertex(minX, y0,      z0); vertex(maxX, y0,      z0);
    vertex(maxX, bottomY, z0); vertex(minX, bottomY, z0);

    glNormal3f( 0, 0,  1);
    vertex(maxX, y1,      z1); vertex(minX, y1,      z1);
    vertex(minX, bottomY, z1); vertex(maxX, bottomY, z1);
    glEnd();
}


void drawFlagPole(float x, float baseY, float z)
{
    glPushMatrix();
    glTranslatef(x, baseY, z);

    
    GLUquadric* q = gluNewQuadric();
    glColor3f(0.95f, 0.95f, 0.95f);
    glPushMatrix();
    glRotatef(-90.0f, 1, 0, 0);
    gluCylinder(q, 0.06f, 0.06f, 1.8f, 16, 4);
    glPopMatrix();
    gluDeleteQuadric(q);

    
    glColor3f(1.0f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
    glNormal3f(0, 0, 1);
    glVertex3f(0.07f, 1.55f, 0.0f);
    glVertex3f(0.07f, 1.10f, 0.0f);
    glVertex3f(0.85f, 1.33f, 0.0f);
    glEnd();


    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-0.25f, 0.01f, -0.25f);
    glVertex3f( 0.25f, 0.01f, -0.25f);
    glVertex3f( 0.25f, 0.01f,  0.25f);
    glVertex3f(-0.25f, 0.01f,  0.25f);
    glEnd();

    glPopMatrix();
}

static void buildFlagShadowMatrix(float mat[16], float planeY)
{
    buildSceneShadowMatrix(mat, planeY);
}

void drawFlagPoleShadow(float x, float baseY, float z)
{
    const float shadowY = baseY + 0.018f;
    const float alpha   = 0.40f;
    const float r = 0.06f, poleH = 1.8f;

    float mat[16];
    buildFlagShadowMatrix(mat, shadowY);

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    glColor4f(0.0f, 0.0f, 0.0f, alpha);

    glPushMatrix();
    glMultMatrixf(mat);
    glTranslatef(x, baseY, z);

    glBegin(GL_QUADS);
    glVertex3f(-r, 0.0f,  r); glVertex3f( r, 0.0f,  r);
    glVertex3f( r, poleH, r); glVertex3f(-r, poleH, r);

    glVertex3f( r, 0.0f, -r); glVertex3f(-r, 0.0f, -r);
    glVertex3f(-r, poleH,-r); glVertex3f( r, poleH,-r);

    glVertex3f(-r, 0.0f, -r); glVertex3f(-r, 0.0f,  r);
    glVertex3f(-r, poleH, r); glVertex3f(-r, poleH,-r);

    glVertex3f( r, 0.0f,  r); glVertex3f( r, 0.0f, -r);
    glVertex3f( r, poleH,-r); glVertex3f( r, poleH, r);

    glVertex3f(-r, poleH,-r); glVertex3f( r, poleH,-r);
    glVertex3f( r, poleH, r); glVertex3f(-r, poleH, r);
    glEnd();

    glBegin(GL_TRIANGLES);
    glVertex3f(0.07f, 1.55f, 0.0f);
    glVertex3f(0.07f, 1.10f, 0.0f);
    glVertex3f(0.85f, 1.33f, 0.0f);
    glEnd();

    glPopMatrix();
    glDepthMask(GL_TRUE);
    glPopAttrib();
}

void drawArena()
{
    const float wallBot = BUILDING_BOTTOM;

    
    const Color3 mainTopA  = {1.0f,  0.85f, 0.3f},  mainTopB  = {1.0f,  1.0f,  1.0f},  mainSide  = {1.0f,  0.55f, 0.05f};
    const Color3 lowerTopA = {0.9f,  0.5f,  0.3f},  lowerTopB = {0.9f,  0.8f,  0.6f},  lowerSide = {0.55f, 0.25f, 0.12f};
    const Color3 bridgeTop = {0.6f,  0.6f,  0.6f},  bridgeSide= {0.35f, 0.35f, 0.35f};
    const Color3 rampTop   = {0.7f,  0.7f,  0.7f},  rampSide  = {0.45f, 0.45f, 0.45f};
    const Color3 thirdTopA = {1.0f,  0.55f, 0.1f},  thirdTopB = {1.0f,  1.0f,  1.0f},  thirdSide = {0.8f,  0.35f, 0.05f};
    const Color3 ramp2Top  = {0.55f, 0.45f, 0.65f}, ramp2Side = {0.30f, 0.22f, 0.40f};
    const Color3 fourthTopA= {0.45f, 0.3f,  0.65f}, fourthTopB= {0.75f, 0.65f, 0.9f},  fourthSide= {0.3f,  0.18f, 0.45f};
    const Color3 bridge3Top= {0.5f,  0.4f,  0.6f},  bridge3Side={0.32f, 0.25f, 0.40f};
    const Color3 fifthTopA = {0.2f,  0.7f,  0.6f},  fifthTopB = {0.6f,  0.95f, 0.85f}, fifthSide = {0.1f,  0.45f, 0.38f};


    const float mainY   = -BALL_RADIUS;
    const float mainMin = -ARENA_HALF;
    const float mainMax =  ARENA_HALF;
    drawArenaBlock(mainMin, mainMax, mainMin, mainMax, mainY, wallBot, mainTopA, mainTopB, mainSide);

    
    const float rampZ0 = ARENA_HALF, rampZ1 = ARENA_HALF + RAMP_LENGTH;
    const float rampY0 = mainY,      rampY1 = LOWER_Y - BALL_RADIUS;
    drawRampBlock(-RAMP_HALF_WIDTH, RAMP_HALF_WIDTH, rampZ0, rampZ1, rampY0, rampY1, wallBot, rampTop, rampSide);

    
    const float lowerMinX = -LOWER_ARENA_HALF, lowerMaxX = LOWER_ARENA_HALF;
    const float lowerZ0   = ARENA_HALF + RAMP_LENGTH;
    const float lowerZ1   = lowerZ0 + LOWER_ARENA_HALF * 2.0f;
    const float lowerY    = LOWER_Y - BALL_RADIUS;
    const float holeCenterZ = lowerZ0 + LOWER_ARENA_HALF;
    drawArenaBlockWithHole(lowerMinX, lowerMaxX, lowerZ0, lowerZ1, lowerY, wallBot,
                           lowerTopA, lowerTopB, lowerSide,
                           -LOWER_HOLE_HALF, LOWER_HOLE_HALF,
                           holeCenterZ - LOWER_HOLE_HALF, holeCenterZ + LOWER_HOLE_HALF);

   
    const float bridgeZ0 = lowerZ0 + LOWER_ARENA_HALF - BRIDGE2_HALF_WIDTH;
    const float bridgeZ1 = lowerZ0 + LOWER_ARENA_HALF + BRIDGE2_HALF_WIDTH;
    const float bridgeX1 = lowerMaxX + BRIDGE2_LENGTH;
    drawBridgeBlock(lowerMaxX, bridgeX1, bridgeZ0, bridgeZ1, lowerY, wallBot, bridgeTop, bridgeSide);

    
    const float thirdMinX = bridgeX1;
    const float thirdMaxX = bridgeX1 + THIRD_ARENA_HALF * 2.0f;
    const float thirdMinZ = lowerZ0 + LOWER_ARENA_HALF - THIRD_ARENA_HALF;
    const float thirdMaxZ = lowerZ0 + LOWER_ARENA_HALF + THIRD_ARENA_HALF;
    const float thirdCX   = (thirdMinX + thirdMaxX) * 0.5f;
    const float thirdCZ   = (thirdMinZ + thirdMaxZ) * 0.5f;
    const float thirdHHX  = THIRD_ARENA_HALF * 0.30f;
    const float thirdHHZ  = THIRD_ARENA_HALF * 0.30f;
    drawArenaBlockWithSlopedHole(thirdMinX, thirdMaxX, thirdMinZ, thirdMaxZ, lowerY, wallBot,
                                 thirdTopA, thirdTopB, thirdSide,
                                 thirdCX - thirdHHX, thirdCX + thirdHHX,
                                 thirdCZ - thirdHHZ, thirdCZ + thirdHHZ,
                                 1.4f);

    
    float ramp2MinX, ramp2MaxX, ramp2Z0, ramp2Z1;
    getRamp2Layout(ramp2MinX, ramp2MaxX, ramp2Z0, ramp2Z1);
    drawRampBlock(ramp2MinX, ramp2MaxX, ramp2Z0, ramp2Z1,
                  lowerY, FOURTH_Y - BALL_RADIUS, wallBot, ramp2Top, ramp2Side);

    
    float fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ;
    const float fourthY = FOURTH_Y - BALL_RADIUS;
    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);

    const float fourthCZ   = (fourthMinZ + fourthMaxZ) * 0.5f;
    const float fourthCX   = (fourthMinX + fourthMaxX) * 0.5f;
    const float fhh        = 0.45f;  
    const float fourthHoles[4][4] = {
        { fourthCX + 2.00f - fhh, fourthCX + 2.00f + fhh, fourthCZ + 1.20f - fhh, fourthCZ + 1.20f + fhh },
        { fourthCX + 1.20f - fhh, fourthCX + 1.20f + fhh, fourthCZ + 0.45f - fhh, fourthCZ + 0.45f + fhh },
        { fourthCX - 0.60f - fhh, fourthCX - 0.60f + fhh, fourthCZ - 1.15f - fhh, fourthCZ - 1.15f + fhh },
        { fourthCX + 2.10f - fhh, fourthCX + 2.10f + fhh, fourthCZ - 1.85f - fhh, fourthCZ - 1.85f + fhh },
    };
    drawArenaBlockWithSmallHoles(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ,
                                 fourthY, wallBot,
                                 fourthTopA, fourthTopB, fourthSide,
                                 fourthHoles, 4);

    
    const float bridge3MaxX = fourthMaxX + BRIDGE3_LENGTH;
    drawBridgeBlock(fourthMaxX, bridge3MaxX,
                    fourthCZ - BRIDGE3_HALF_WIDTH, fourthCZ + BRIDGE3_HALF_WIDTH,
                    fourthY, wallBot, bridge3Top, bridge3Side);

   
    const float fifthHalf = FIFTH_ARENA_HALF;
    const float fifthMinX = bridge3MaxX;
    const float fifthMaxX = fifthMinX + fifthHalf * 2.0f;
    const float fifthMinZ = fourthCZ - fifthHalf;
    const float fifthMaxZ = fourthCZ + fifthHalf;
    drawArenaBlock(fifthMinX, fifthMaxX, fifthMinZ, fifthMaxZ,
                   fourthY, wallBot, fifthTopA, fifthTopB, fifthSide);

    
    const Color3 bridge4Top  = { 0.55f, 0.55f, 0.55f };
    const Color3 bridge4Side = { 0.30f, 0.30f, 0.30f };
    const float  fifthCX     = (fifthMinX + fifthMaxX) * 0.5f;
    const float  bridge4MinZ = fifthMinZ - BRIDGE4_LENGTH;
    drawBridgeBlock(fifthCX - BRIDGE4_HALF_WIDTH, fifthCX + BRIDGE4_HALF_WIDTH,
                    bridge4MinZ, fifthMinZ, fourthY, wallBot, bridge4Top, bridge4Side);

    
    const Color3 sixthTopA = { 0.55f, 0.95f, 0.90f };
    const Color3 sixthTopB = { 0.90f, 1.00f, 0.95f };
    const Color3 sixthSide = { 0.08f, 0.38f, 0.32f };
    const float  sixthMinZ = bridge4MinZ - SIXTH_ARENA_LENGTH;
    drawArenaBlock(fifthMinX, fifthMaxX, sixthMinZ, bridge4MinZ,
                   fourthY, wallBot, sixthTopA, sixthTopB, sixthSide);

    
    const Color3 bridge5Top  = { 0.48f, 0.48f, 0.52f };
    const Color3 bridge5Side = { 0.26f, 0.26f, 0.30f };
    const float  sixthCX     = (fifthMinX + fifthMaxX) * 0.5f;
    const float  bridge5MinZ = sixthMinZ - BRIDGE5_LENGTH;
    drawBridgeBlock(sixthCX - BRIDGE5_HALF_WIDTH, sixthCX + BRIDGE5_HALF_WIDTH,
                    bridge5MinZ, sixthMinZ, fourthY, wallBot, bridge5Top, bridge5Side);

   
    const Color3 seventhTopA = { 0.95f, 0.82f, 0.30f };
    const Color3 seventhTopB = { 0.98f, 0.96f, 0.80f };
    const Color3 seventhSide = { 0.52f, 0.40f, 0.08f };
    const float  seventhMinZ = bridge5MinZ - SEVENTH_ARENA_LENGTH;
    drawArenaBlock(fifthMinX, fifthMaxX, seventhMinZ, bridge5MinZ,
                   fourthY, wallBot, seventhTopA, seventhTopB, seventhSide);

    
    const Color3 bridge6Top  = { 0.43f, 0.43f, 0.46f };
    const Color3 bridge6Side = { 0.22f, 0.22f, 0.25f };
    const float  seventhCZ   = (seventhMinZ + bridge5MinZ) * 0.5f;

    
    float b6s1MaxX = fifthMinX,                    b6s1MinX = b6s1MaxX - BRIDGE6_SEG1_LENGTH;
    float b6s1MinZ = seventhCZ - BRIDGE6_HALF_WIDTH, b6s1MaxZ = seventhCZ + BRIDGE6_HALF_WIDTH;
    drawBridgeBlock(b6s1MinX, b6s1MaxX, b6s1MinZ, b6s1MaxZ, fourthY, wallBot, bridge6Top, bridge6Side);

    
    float b6s2MinX = b6s1MinX - BRIDGE6_HALF_WIDTH, b6s2MaxX = b6s1MinX + BRIDGE6_HALF_WIDTH;
    float b6s2MinZ = seventhCZ,                     b6s2MaxZ = seventhCZ + BRIDGE6_SEG2_LENGTH;
    drawBridgeBlock(b6s2MinX, b6s2MaxX, b6s2MinZ, b6s2MaxZ, fourthY, wallBot, bridge6Top, bridge6Side);

    
    float b6s3MaxX = b6s1MinX,                      b6s3MinX = b6s3MaxX - BRIDGE6_SEG3_LENGTH;
    float b6s3MinZ = b6s2MaxZ - BRIDGE6_HALF_WIDTH, b6s3MaxZ = b6s2MaxZ + BRIDGE6_HALF_WIDTH;
    drawBridgeBlock(b6s3MinX, b6s3MaxX, b6s3MinZ, b6s3MaxZ, fourthY, wallBot, bridge6Top, bridge6Side);

   
    float b6s4MinX = b6s3MinX - BRIDGE6_HALF_WIDTH, b6s4MaxX = b6s3MinX + BRIDGE6_HALF_WIDTH;
    float b6s4MinZ = b6s2MaxZ - BRIDGE6_SEG4_LENGTH, b6s4MaxZ = b6s2MaxZ;
    drawBridgeBlock(b6s4MinX, b6s4MaxX, b6s4MinZ, b6s4MaxZ, fourthY, wallBot, bridge6Top, bridge6Side);

   
    float b6s5MaxX = b6s3MinX,                      b6s5MinX = b6s5MaxX - BRIDGE6_SEG5_LENGTH;
    float b6s5MinZ = b6s4MinZ - BRIDGE6_HALF_WIDTH, b6s5MaxZ = b6s4MinZ + BRIDGE6_HALF_WIDTH;
    drawBridgeBlock(b6s5MinX, b6s5MaxX, b6s5MinZ, b6s5MaxZ, fourthY, wallBot, bridge6Top, bridge6Side);

    
    float b6s6MinX = b6s5MinX - BRIDGE6_HALF_WIDTH, b6s6MaxX = b6s5MinX + BRIDGE6_HALF_WIDTH;
    float b6s6MinZ = b6s4MinZ,                      b6s6MaxZ = b6s4MinZ + BRIDGE6_SEG6_LENGTH;
    drawBridgeBlock(b6s6MinX, b6s6MaxX, b6s6MinZ, b6s6MaxZ, fourthY, wallBot, bridge6Top, bridge6Side);

    
    float b6s7MaxX = b6s5MinX,                      b6s7MinX = b6s7MaxX - BRIDGE6_SEG7_LENGTH;
    float b6s7MinZ = b6s6MaxZ - BRIDGE6_HALF_WIDTH, b6s7MaxZ = b6s6MaxZ + BRIDGE6_HALF_WIDTH;
    drawBridgeBlock(b6s7MinX, b6s7MaxX, b6s7MinZ, b6s7MaxZ, fourthY, wallBot, bridge6Top, bridge6Side);

    
    const Color3 eighthTopA = { 0.35f, 0.55f, 0.95f };
    const Color3 eighthTopB = { 0.82f, 0.90f, 1.00f };
    const Color3 eighthSide = { 0.10f, 0.22f, 0.48f };
    const float  eighthMaxX  = b6s7MinX;
    const float  eighthMinX  = eighthMaxX - EIGHTH_ARENA_HALF * 2.0f;
    const float  eighthCZ    = b6s6MaxZ - EIGHTH_ARENA_FORWARD_OFFSET;
    const float  eighthMinZ  = eighthCZ - EIGHTH_ARENA_HALF;
    const float  eighthMaxZ  = eighthCZ + EIGHTH_ARENA_HALF;
    drawArenaBlock(eighthMinX, eighthMaxX, eighthMinZ, eighthMaxZ,
                   fourthY, wallBot, eighthTopA, eighthTopB, eighthSide);

    
    const float flagX = (eighthMinX + eighthMaxX) * 0.5f;
    const float flagZ = (eighthMinZ + eighthMaxZ) * 0.5f;
    drawFlagPoleShadow(flagX, fourthY, flagZ);
    drawFlagPole      (flagX, fourthY, flagZ);

    
    drawDecorativeWorld();
    drawBarriers();
}
