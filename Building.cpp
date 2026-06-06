#include "Building.h"
#include "Common.h"
#include <GL/glut.h>



struct Color3 { float r, g, b; };

static void setColor(const Color3& c) { glColor3f(c.r, c.g, c.b); }
static void vertex(float x, float y, float z) { glVertex3f(x, y, z); }

struct BuildingFootprint {
    float minX, maxX;
    float minZ, maxZ;
};

static bool overlapsPlacedBuilding(const std::vector<BuildingFootprint>& placed,
                                   float cx, float cz,
                                   float hw, float hd,
                                   float gap)
{
    float minX = cx - hw - gap;
    float maxX = cx + hw + gap;
    float minZ = cz - hd - gap;
    float maxZ = cz + hd + gap;

    for (size_t i = 0; i < placed.size(); i++)
    {
        const BuildingFootprint& other = placed[i];
        bool separated = maxX <= other.minX || minX >= other.maxX ||
                         maxZ <= other.minZ || minZ >= other.maxZ;
        if (!separated) return true;
    }

    return false;
}

static bool overlapsArena(float cx, float cz,
                          float hw, float hd,
                          float clearance)
{
    float minX = cx - hw - clearance;
    float maxX = cx + hw + clearance;
    float minZ = cz - hd - clearance;
    float maxZ = cz + hd + clearance;

    BuildingFootprint arenaBounds[24];
    int count = 0;

    #define ADD_ARENA_RECT(a, b, c, d) \
        do { \
            arenaBounds[count].minX = ((a) < (b)) ? (a) : (b); \
            arenaBounds[count].maxX = ((a) > (b)) ? (a) : (b); \
            arenaBounds[count].minZ = ((c) < (d)) ? (c) : (d); \
            arenaBounds[count].maxZ = ((c) > (d)) ? (c) : (d); \
            count++; \
        } while (0)

    float mainMin = -ARENA_HALF;
    float mainMax =  ARENA_HALF;
    ADD_ARENA_RECT(mainMin, mainMax, mainMin, mainMax);

    float rampZ0 = ARENA_HALF;
    float rampZ1 = ARENA_HALF + RAMP_LENGTH;
    ADD_ARENA_RECT(-RAMP_HALF_WIDTH, RAMP_HALF_WIDTH, rampZ0, rampZ1);

    float lowerMinX = -LOWER_ARENA_HALF;
    float lowerMaxX =  LOWER_ARENA_HALF;
    float lowerZ0 = ARENA_HALF + RAMP_LENGTH;
    float lowerZ1 = lowerZ0 + LOWER_ARENA_HALF * 2.0f;
    ADD_ARENA_RECT(lowerMinX, lowerMaxX, lowerZ0, lowerZ1);

    float bridgeCenterZ = lowerZ0 + LOWER_ARENA_HALF;
    float bridgeX0 = lowerMaxX;
    float bridgeX1 = lowerMaxX + BRIDGE2_LENGTH;
    ADD_ARENA_RECT(bridgeX0, bridgeX1,
                   bridgeCenterZ - BRIDGE2_HALF_WIDTH,
                   bridgeCenterZ + BRIDGE2_HALF_WIDTH);

    float thirdMinX = bridgeX1;
    float thirdMaxX = thirdMinX + THIRD_ARENA_HALF * 2.0f;
    float thirdMinZ = bridgeCenterZ - THIRD_ARENA_HALF;
    float thirdMaxZ = bridgeCenterZ + THIRD_ARENA_HALF;
    ADD_ARENA_RECT(thirdMinX, thirdMaxX, thirdMinZ, thirdMaxZ);

    float ramp2MinX, ramp2MaxX, ramp2Z0, ramp2Z1;
    getRamp2Layout(ramp2MinX, ramp2MaxX, ramp2Z0, ramp2Z1);
    ADD_ARENA_RECT(ramp2MinX, ramp2MaxX, ramp2Z0, ramp2Z1);

    float fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ;
    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);
    ADD_ARENA_RECT(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);

    float fourthCZ = (fourthMinZ + fourthMaxZ) * 0.5f;
    float bridge3MinX = fourthMaxX;
    float bridge3MaxX = fourthMaxX + BRIDGE3_LENGTH;
    ADD_ARENA_RECT(bridge3MinX, bridge3MaxX,
                   fourthCZ - BRIDGE3_HALF_WIDTH,
                   fourthCZ + BRIDGE3_HALF_WIDTH);

    float fifthHalf = FIFTH_ARENA_HALF;
    float fifthMinX = bridge3MaxX;
    float fifthMaxX = fifthMinX + fifthHalf * 2.0f;
    float fifthMinZ = fourthCZ - fifthHalf;
    float fifthMaxZ = fourthCZ + fifthHalf;
    ADD_ARENA_RECT(fifthMinX, fifthMaxX, fifthMinZ, fifthMaxZ);

    float fifthCX = (fifthMinX + fifthMaxX) * 0.5f;
    float bridge4MinZ = fifthMinZ - BRIDGE4_LENGTH;
    ADD_ARENA_RECT(fifthCX - BRIDGE4_HALF_WIDTH,
                   fifthCX + BRIDGE4_HALF_WIDTH,
                   bridge4MinZ, fifthMinZ);

    float sixthMinX = fifthMinX;
    float sixthMaxX = fifthMaxX;
    float sixthMaxZ = bridge4MinZ;
    float sixthMinZ = bridge4MinZ - SIXTH_ARENA_LENGTH;
    ADD_ARENA_RECT(sixthMinX, sixthMaxX, sixthMinZ, sixthMaxZ);

    float sixthCX = (sixthMinX + sixthMaxX) * 0.5f;
    float bridge5MinZ = sixthMinZ - BRIDGE5_LENGTH;
    ADD_ARENA_RECT(sixthCX - BRIDGE5_HALF_WIDTH,
                   sixthCX + BRIDGE5_HALF_WIDTH,
                   bridge5MinZ, sixthMinZ);

    float seventhMinX = sixthMinX;
    float seventhMaxX = sixthMaxX;
    float seventhMaxZ = bridge5MinZ;
    float seventhMinZ = bridge5MinZ - SEVENTH_ARENA_LENGTH;
    ADD_ARENA_RECT(seventhMinX, seventhMaxX, seventhMinZ, seventhMaxZ);

    float seventhCZ = (seventhMinZ + seventhMaxZ) * 0.5f;
    float b6s1MaxX = fifthMinX;
    float b6s1MinX = b6s1MaxX - BRIDGE6_SEG1_LENGTH;
    float b6s1MinZ = seventhCZ - BRIDGE6_HALF_WIDTH;
    float b6s1MaxZ = seventhCZ + BRIDGE6_HALF_WIDTH;
    ADD_ARENA_RECT(b6s1MinX, b6s1MaxX, b6s1MinZ, b6s1MaxZ);

    float b6s2MinX = b6s1MinX - BRIDGE6_HALF_WIDTH;
    float b6s2MaxX = b6s1MinX + BRIDGE6_HALF_WIDTH;
    float b6s2MinZ = seventhCZ;
    float b6s2MaxZ = seventhCZ + BRIDGE6_SEG2_LENGTH;
    ADD_ARENA_RECT(b6s2MinX, b6s2MaxX, b6s2MinZ, b6s2MaxZ);

    float b6s3MaxX = b6s1MinX;
    float b6s3MinX = b6s3MaxX - BRIDGE6_SEG3_LENGTH;
    float b6s3MinZ = b6s2MaxZ - BRIDGE6_HALF_WIDTH;
    float b6s3MaxZ = b6s2MaxZ + BRIDGE6_HALF_WIDTH;
    ADD_ARENA_RECT(b6s3MinX, b6s3MaxX, b6s3MinZ, b6s3MaxZ);

    float b6s4MinX = b6s3MinX - BRIDGE6_HALF_WIDTH;
    float b6s4MaxX = b6s3MinX + BRIDGE6_HALF_WIDTH;
    float b6s4MinZ = b6s2MaxZ - BRIDGE6_SEG4_LENGTH;
    float b6s4MaxZ = b6s2MaxZ;
    ADD_ARENA_RECT(b6s4MinX, b6s4MaxX, b6s4MinZ, b6s4MaxZ);

    float b6s5MaxX = b6s3MinX;
    float b6s5MinX = b6s5MaxX - BRIDGE6_SEG5_LENGTH;
    float b6s5MinZ = b6s4MinZ - BRIDGE6_HALF_WIDTH;
    float b6s5MaxZ = b6s4MinZ + BRIDGE6_HALF_WIDTH;
    ADD_ARENA_RECT(b6s5MinX, b6s5MaxX, b6s5MinZ, b6s5MaxZ);

    float b6s6MinX = b6s5MinX - BRIDGE6_HALF_WIDTH;
    float b6s6MaxX = b6s5MinX + BRIDGE6_HALF_WIDTH;
    float b6s6MinZ = b6s4MinZ;
    float b6s6MaxZ = b6s4MinZ + BRIDGE6_SEG6_LENGTH;
    ADD_ARENA_RECT(b6s6MinX, b6s6MaxX, b6s6MinZ, b6s6MaxZ);

    float b6s7MaxX = b6s5MinX;
    float b6s7MinX = b6s7MaxX - BRIDGE6_SEG7_LENGTH;
    float b6s7MinZ = b6s6MaxZ - BRIDGE6_HALF_WIDTH;
    float b6s7MaxZ = b6s6MaxZ + BRIDGE6_HALF_WIDTH;
    ADD_ARENA_RECT(b6s7MinX, b6s7MaxX, b6s7MinZ, b6s7MaxZ);

    float eighthMaxX = b6s7MinX;
    float eighthMinX = eighthMaxX - EIGHTH_ARENA_HALF * 2.0f;
    float eighthCZ = b6s6MaxZ - EIGHTH_ARENA_FORWARD_OFFSET;
    ADD_ARENA_RECT(eighthMinX, eighthMaxX,
                   eighthCZ - EIGHTH_ARENA_HALF,
                   eighthCZ + EIGHTH_ARENA_HALF);

    #undef ADD_ARENA_RECT

    for (int i = 0; i < count; i++)
    {
        const BuildingFootprint& r = arenaBounds[i];
        bool separated = maxX <= r.minX || minX >= r.maxX ||
                         maxZ <= r.minZ || minZ >= r.maxZ;
        if (!separated) return true;
    }

    return false;
}

static void rememberBuilding(std::vector<BuildingFootprint>& placed,
                             float cx, float cz,
                             float hw, float hd)
{
    BuildingFootprint footprint;
    footprint.minX = cx - hw;
    footprint.maxX = cx + hw;
    footprint.minZ = cz - hd;
    footprint.maxZ = cz + hd;
    placed.push_back(footprint);
}


static void drawBuilding(float cx, float cz,
                         float hw, float hd,
                         float topY, float baseY,
                         const Color3& wallColor,
                         const Color3& roofColor,
                         const Color3& windowColor)
{
    float buildH = topY - baseY;

    Color3 dark = { wallColor.r * 0.60f, wallColor.g * 0.60f, wallColor.b * 0.60f };
    Color3 mid  = { wallColor.r * 0.78f, wallColor.g * 0.78f, wallColor.b * 0.78f };

    glPushMatrix();
    glTranslatef(cx, baseY, cz);
    glScalef(hw, buildH, hd);

    
    glBegin(GL_QUADS);
    setColor(roofColor); glNormal3f( 0, 1,  0);
    vertex(-1, 1, -1); vertex( 1, 1, -1);
    vertex( 1, 1,  1); vertex(-1, 1,  1);

    setColor(wallColor); glNormal3f( 0, 0,  1);
    vertex(-1, 0,  1); vertex( 1, 0,  1);
    vertex( 1, 1,  1); vertex(-1, 1,  1);

    setColor(wallColor); glNormal3f( 0, 0, -1);
    vertex( 1, 0, -1); vertex(-1, 0, -1);
    vertex(-1, 1, -1); vertex( 1, 1, -1);

    setColor(dark); glNormal3f(-1, 0,  0);
    vertex(-1, 0, -1); vertex(-1, 0,  1);
    vertex(-1, 1,  1); vertex(-1, 1, -1);

    setColor(mid); glNormal3f( 1, 0,  0);
    vertex( 1, 0,  1); vertex( 1, 0, -1);
    vertex( 1, 1, -1); vertex( 1, 1,  1);
    glEnd();

    
    float buildW = hw * 2.0f;

    int cols = (int)(buildW / 1.8f);
    int rows = (int)(buildH / 2.2f);
    cols = (cols < 1) ? 1 : (cols > 6) ? 6 : cols;
    rows = (rows < 1) ? 1 : (rows > 8) ? 8 : rows;

    float colStep = 2.0f / (cols + 1);
    float rowStep = 1.0f / (rows + 1);
    float ww      = colStep * 0.38f;
    float wh      = rowStep * 0.35f;
    float wOff    = 0.04f;

    setColor(windowColor);
    glDisable(GL_LIGHTING);
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    for (int c = 1; c <= cols; c++)
    {
        for (int r = 1; r <= rows; r++)
        {
            float wx = -1.0f + c * colStep;
            float wy = r * rowStep;
            float wz = 1.0f + wOff;
            vertex(wx - ww, wy - wh, wz); vertex(wx + ww, wy - wh, wz);
            vertex(wx + ww, wy + wh, wz); vertex(wx - ww, wy + wh, wz);
        }
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}


static float lcgNext(unsigned int& s)
{
    s = s * 1664525u + 1013904223u;
    return (float)(s & 0xFFFF) / 65535.0f;
}

static float rRange(unsigned int& s, float lo, float hi)
{
    return lo + lcgNext(s) * (hi - lo);
}


void drawDecorativeWorld()
{
    const float baseY = BUILDING_BOTTOM;
    const float buildingGap = 0.85f;
    const float arenaClearance = 7.0f;
    const int maxPlaceAttempts = 60;

    Color3 walls[8] = {
        {0.62f, 0.65f, 0.70f}, {0.82f, 0.76f, 0.62f},
        {0.42f, 0.55f, 0.78f}, {0.38f, 0.62f, 0.48f},
        {0.90f, 0.86f, 0.74f}, {0.72f, 0.38f, 0.28f},
        {0.50f, 0.52f, 0.60f}, {0.78f, 0.68f, 0.52f},
    };
    Color3 roofs[3] = {
        {0.22f, 0.22f, 0.25f},
        {0.40f, 0.40f, 0.42f},
        {0.60f, 0.62f, 0.65f},
    };
    Color3 winY = { 1.00f, 0.96f, 0.55f };

    
    unsigned int s = 0xCAFE1234u;

    struct Zone {
        float xLo, xHi;
        float zLo, zHi;
        float htMin, htMax;
        float hwMin, hwMax;
        float hdMin, hdMax;
        int   count;
    };

    Zone zones[] = {
        { -14.0f, -40.0f,  -75.0f,  35.0f,   6.0f, 18.0f,  1.5f, 2.5f,  1.4f, 2.2f,  55 },
        { -42.0f, -75.0f,  -75.0f,  35.0f,  14.0f, 30.0f,  2.0f, 3.5f,  1.8f, 3.0f,  45 },
        {  70.0f, 100.0f,  -75.0f,  35.0f,   6.0f, 20.0f,  1.5f, 2.6f,  1.4f, 2.3f,  55 },
        { 102.0f, 135.0f,  -75.0f,  35.0f,  16.0f, 34.0f,  2.2f, 3.8f,  2.0f, 3.2f,  40 },
        { -70.0f, 105.0f,  -80.0f, -58.0f,  14.0f, 36.0f,  2.5f, 4.5f,  2.5f, 4.5f,  50 },
        { -55.0f,  90.0f,   38.0f,  60.0f,  10.0f, 28.0f,  2.0f, 3.5f,  2.0f, 3.5f,  45 },
    };

    int nZones = 6;
    std::vector<BuildingFootprint> placed;
    placed.reserve(300);

    for (int z = 0; z < nZones; z++)
    {
        Zone& zn = zones[z];
        for (int i = 0; i < zn.count; i++)
        {
            for (int attempt = 0; attempt < maxPlaceAttempts; attempt++)
            {
                float xc = rRange(s, zn.xLo, zn.xHi);
                float zc = rRange(s, zn.zLo, zn.zHi);
                float hw = rRange(s, zn.hwMin, zn.hwMax);
                float hd = rRange(s, zn.hdMin, zn.hdMax);
                float ht = rRange(s, zn.htMin, zn.htMax);
                int wi = (int)(rRange(s, 0, 7.99f));
                int ri = (int)(rRange(s, 0, 2.99f));

                if (overlapsPlacedBuilding(placed, xc, zc, hw, hd, buildingGap))
                {
                    continue;
                }

                if (overlapsArena(xc, zc, hw, hd, arenaClearance))
                {
                    continue;
                }

                rememberBuilding(placed, xc, zc, hw, hd);
                drawBuilding(xc, zc, hw, hd,
                             baseY + ht, baseY,
                             walls[wi], roofs[ri], winY);
                break;
            }
        }
    }
}
