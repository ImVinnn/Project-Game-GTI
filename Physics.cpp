#include "Physics.h"
#include <cmath>

bool isInLowerHole(float x, float z) {
    float holeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;
    return std::fabs(x) <= LOWER_HOLE_HALF && std::fabs(z - holeCenterZ) <= LOWER_HOLE_HALF;
}

float getRampHeight(float x, float z) {
    if (z >= ARENA_HALF && z <= ARENA_HALF + RAMP_LENGTH && std::fabs(x) <= RAMP_HALF_WIDTH) {
        float t = (z - ARENA_HALF) / RAMP_LENGTH;
        float topY = -BALL_RADIUS;
        float bottomY = LOWER_Y - BALL_RADIUS;

        return topY + t * (bottomY - topY);
    }

    return -1e6f;
}

float getRamp2Height(float x, float z) {
    float ramp2MinX;
    float ramp2MaxX;
    float ramp2Z0;
    float ramp2Z1;

    getRamp2Layout(ramp2MinX, ramp2MaxX, ramp2Z0, ramp2Z1);

    if (z <= ramp2Z0 && z >= ramp2Z1 &&
        x >= ramp2MinX && x <= ramp2MaxX) {
        float t = (ramp2Z0 - z) / RAMP2_LENGTH;
        float topY = LOWER_Y - BALL_RADIUS;
        float bottomY = FOURTH_Y - BALL_RADIUS;

        return topY + t * (bottomY - topY);
    }

    return -1e6f;
}

float clamp01(float v) {
    if (v < 0.0f) return 0.0f;
    if (v > 1.0f) return 1.0f;
    return v;
}

float getSlopedHoleHeight(float x, float z,
                          float minX, float maxX,
                          float minZ, float maxZ,
                          float topY, float holeTopY,
                          float holeMinX, float holeMaxX,
                          float holeMinZ, float holeMaxZ) {
    float tx = 1.0f;
    float tz = 1.0f;

    if (x < holeMinX) {
        tx = (x - minX) / (holeMinX - minX);
    } else if (x > holeMaxX) {
        tx = (maxX - x) / (maxX - holeMaxX);
    }

    if (z < holeMinZ) {
        tz = (z - minZ) / (holeMinZ - minZ);
    } else if (z > holeMaxZ) {
        tz = (maxZ - z) / (maxZ - holeMaxZ);
    }

    tx = clamp01(tx);
    tz = clamp01(tz);

    float t = tx;
    if (tz < t) {
        t = tz;
    }

    return topY - (topY - holeTopY) * t;
}

bool isInFourthArenaHole(float x, float z,
                         float centerX, float centerZ) {
    float holeHalf = 0.45f;

    float holeCenters[4][2] = {
        {centerX + 2.00f, centerZ + 1.20f},
        {centerX + 1.20f, centerZ + 0.45f},
        {centerX - 0.60f, centerZ - 1.15f},
        {centerX + 2.10f, centerZ - 1.85f}
    };

    for (int i = 0; i < 4; i++) {
        if (x >= holeCenters[i][0] - holeHalf &&
            x <= holeCenters[i][0] + holeHalf &&
            z >= holeCenters[i][1] - holeHalf &&
            z <= holeCenters[i][1] + holeHalf) {
            return true;
        }
    }

    return false;
}

float getGroundHeight(float x, float z) {
    
    if (std::fabs(x) <= ARENA_HALF && std::fabs(z) <= ARENA_HALF) {
        return -BALL_RADIUS;
    }

    
    float rampY = getRampHeight(x, z);
    if (rampY > -1e5f) {
        return rampY;
    }

    
    if (std::fabs(x) <= LOWER_ARENA_HALF &&
        z >= ARENA_HALF + RAMP_LENGTH &&
        z <= ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF * 2.0f) {

        float holeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;

        if (std::fabs(x) <= LOWER_HOLE_HALF &&
            std::fabs(z - holeCenterZ) <= LOWER_HOLE_HALF) {
            return -1e6f;
        }

        return LOWER_Y - BALL_RADIUS;
    }

    
    float bridgeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;
    float bridgeMinZ = bridgeCenterZ - BRIDGE2_HALF_WIDTH;
    float bridgeMaxZ = bridgeCenterZ + BRIDGE2_HALF_WIDTH;

    if (x >= LOWER_ARENA_HALF &&
        x <= LOWER_ARENA_HALF + BRIDGE2_LENGTH &&
        z >= bridgeMinZ &&
        z <= bridgeMaxZ) {
        return LOWER_Y - BALL_RADIUS;
    }

    
    float thirdMinX = LOWER_ARENA_HALF + BRIDGE2_LENGTH;
    float thirdMaxX = thirdMinX + THIRD_ARENA_HALF * 2.0f;
    float thirdMinZ = bridgeCenterZ - THIRD_ARENA_HALF;
    float thirdMaxZ = bridgeCenterZ + THIRD_ARENA_HALF;
    float thirdY = LOWER_Y - BALL_RADIUS;

    if (x >= thirdMinX && x <= thirdMaxX &&
        z >= thirdMinZ && z <= thirdMaxZ) {

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

        if (x >= thirdHoleMinX && x <= thirdHoleMaxX &&
            z >= thirdHoleMinZ && z <= thirdHoleMaxZ) {
            return -1e6f;
        }

        return getSlopedHoleHeight(x, z,
                                   thirdMinX, thirdMaxX,
                                   thirdMinZ, thirdMaxZ,
                                   thirdY, holeTopY,
                                   thirdHoleMinX, thirdHoleMaxX,
                                   thirdHoleMinZ, thirdHoleMaxZ);
    }

    
    float ramp2Y = getRamp2Height(x, z);
    if (ramp2Y > -1e5f) {
        return ramp2Y;
    }

    
    float thirdCenterX = thirdMinX + THIRD_ARENA_HALF;
    float fourthMinX;
    float fourthMaxX;
    float fourthMinZ;
    float fourthMaxZ;

    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);

    float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;
    float fourthCenterX = (fourthMinX + fourthMaxX) * 0.5f;

    if (x >= fourthMinX && x <= fourthMaxX &&
        z >= fourthMinZ && z <= fourthMaxZ) {
        if (isInFourthArenaHole(x, z, fourthCenterX, fourthCenterZ)) {
            return -1e6f;
        }

        return FOURTH_Y - BALL_RADIUS;
    }

    
    float bridge3MinX = fourthMaxX;
    float bridge3MaxX = fourthMaxX + BRIDGE3_LENGTH;
    float bridge3MinZ = fourthCenterZ - BRIDGE3_HALF_WIDTH;
    float bridge3MaxZ = fourthCenterZ + BRIDGE3_HALF_WIDTH;

    if (x >= bridge3MinX && x <= bridge3MaxX &&
        z >= bridge3MinZ && z <= bridge3MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    
    float fifthHalf = FIFTH_ARENA_HALF;
    float fifthMinX = bridge3MaxX;
    float fifthMaxX = fifthMinX + fifthHalf * 2.0f;
    float fifthMinZ = fourthCenterZ - fifthHalf;
    float fifthMaxZ = fourthCenterZ + fifthHalf;

    if (x >= fifthMinX && x <= fifthMaxX &&
        z >= fifthMinZ && z <= fifthMaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    
    float fifthCenterX = (fifthMinX + fifthMaxX) * 0.5f;
    float bridge4MinX = fifthCenterX - BRIDGE4_HALF_WIDTH;
    float bridge4MaxX = fifthCenterX + BRIDGE4_HALF_WIDTH;
    float bridge4MaxZ = fifthMinZ;
    float bridge4MinZ = fifthMinZ - BRIDGE4_LENGTH;

    if (x >= bridge4MinX && x <= bridge4MaxX &&
        z >= bridge4MinZ && z <= bridge4MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    
    float sixthMinX = fifthMinX;
    float sixthMaxX = fifthMaxX;
    float sixthMaxZ = bridge4MinZ;
    float sixthMinZ = bridge4MinZ - SIXTH_ARENA_LENGTH;

    if (x >= sixthMinX && x <= sixthMaxX &&
        z >= sixthMinZ && z <= sixthMaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    
    float sixthCenterX = (sixthMinX + sixthMaxX) * 0.5f;
    float bridge5MinX = sixthCenterX - BRIDGE5_HALF_WIDTH;
    float bridge5MaxX = sixthCenterX + BRIDGE5_HALF_WIDTH;
    float bridge5MaxZ = sixthMinZ;
    float bridge5MinZ = sixthMinZ - BRIDGE5_LENGTH;

    if (x >= bridge5MinX && x <= bridge5MaxX &&
        z >= bridge5MinZ && z <= bridge5MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    
    float seventhMinX = sixthMinX;
    float seventhMaxX = sixthMaxX;
    float seventhMaxZ = bridge5MinZ;
    float seventhMinZ = bridge5MinZ - SEVENTH_ARENA_LENGTH;

    if (x >= seventhMinX && x <= seventhMaxX &&
        z >= seventhMinZ && z <= seventhMaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    
    float seventhCenterZ = (seventhMinZ + seventhMaxZ) * 0.5f;

    float bridge6Seg1MaxX = seventhMinX;
    float bridge6Seg1MinX = bridge6Seg1MaxX - BRIDGE6_SEG1_LENGTH;
    float bridge6Seg1CenterZ = seventhCenterZ;
    float bridge6Seg1MinZ = bridge6Seg1CenterZ - BRIDGE6_HALF_WIDTH;
    float bridge6Seg1MaxZ = bridge6Seg1CenterZ + BRIDGE6_HALF_WIDTH;

    if (x >= bridge6Seg1MinX && x <= bridge6Seg1MaxX &&
        z >= bridge6Seg1MinZ && z <= bridge6Seg1MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    float bridge6Seg2CenterX = bridge6Seg1MinX;
    float bridge6Seg2MinX = bridge6Seg2CenterX - BRIDGE6_HALF_WIDTH;
    float bridge6Seg2MaxX = bridge6Seg2CenterX + BRIDGE6_HALF_WIDTH;
    float bridge6Seg2MinZ = bridge6Seg1CenterZ;
    float bridge6Seg2MaxZ = bridge6Seg1CenterZ + BRIDGE6_SEG2_LENGTH;

    if (x >= bridge6Seg2MinX && x <= bridge6Seg2MaxX &&
        z >= bridge6Seg2MinZ && z <= bridge6Seg2MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    float bridge6Seg3MaxX = bridge6Seg2CenterX;
    float bridge6Seg3MinX = bridge6Seg3MaxX - BRIDGE6_SEG3_LENGTH;
    float bridge6Seg3CenterZ = bridge6Seg2MaxZ;
    float bridge6Seg3MinZ = bridge6Seg3CenterZ - BRIDGE6_HALF_WIDTH;
    float bridge6Seg3MaxZ = bridge6Seg3CenterZ + BRIDGE6_HALF_WIDTH;

    if (x >= bridge6Seg3MinX && x <= bridge6Seg3MaxX &&
        z >= bridge6Seg3MinZ && z <= bridge6Seg3MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    float bridge6Seg4CenterX = bridge6Seg3MinX;
    float bridge6Seg4MinX = bridge6Seg4CenterX - BRIDGE6_HALF_WIDTH;
    float bridge6Seg4MaxX = bridge6Seg4CenterX + BRIDGE6_HALF_WIDTH;
    float bridge6Seg4MinZ = bridge6Seg3CenterZ - BRIDGE6_SEG4_LENGTH;
    float bridge6Seg4MaxZ = bridge6Seg3CenterZ;

    if (x >= bridge6Seg4MinX && x <= bridge6Seg4MaxX &&
        z >= bridge6Seg4MinZ && z <= bridge6Seg4MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    float bridge6Seg5MaxX = bridge6Seg4CenterX;
    float bridge6Seg5MinX = bridge6Seg5MaxX - BRIDGE6_SEG5_LENGTH;
    float bridge6Seg5CenterZ = bridge6Seg4MinZ;
    float bridge6Seg5MinZ = bridge6Seg5CenterZ - BRIDGE6_HALF_WIDTH;
    float bridge6Seg5MaxZ = bridge6Seg5CenterZ + BRIDGE6_HALF_WIDTH;

    if (x >= bridge6Seg5MinX && x <= bridge6Seg5MaxX &&
        z >= bridge6Seg5MinZ && z <= bridge6Seg5MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    float bridge6Seg6CenterX = bridge6Seg5MinX;
    float bridge6Seg6MinX = bridge6Seg6CenterX - BRIDGE6_HALF_WIDTH;
    float bridge6Seg6MaxX = bridge6Seg6CenterX + BRIDGE6_HALF_WIDTH;
    float bridge6Seg6MinZ = bridge6Seg5CenterZ;
    float bridge6Seg6MaxZ = bridge6Seg5CenterZ + BRIDGE6_SEG6_LENGTH;

    if (x >= bridge6Seg6MinX && x <= bridge6Seg6MaxX &&
        z >= bridge6Seg6MinZ && z <= bridge6Seg6MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    float bridge6Seg7MaxX = bridge6Seg6CenterX;
    float bridge6Seg7MinX = bridge6Seg7MaxX - BRIDGE6_SEG7_LENGTH;
    float bridge6Seg7CenterZ = bridge6Seg6MaxZ;
    float bridge6Seg7MinZ = bridge6Seg7CenterZ - BRIDGE6_HALF_WIDTH;
    float bridge6Seg7MaxZ = bridge6Seg7CenterZ + BRIDGE6_HALF_WIDTH;

    if (x >= bridge6Seg7MinX && x <= bridge6Seg7MaxX &&
        z >= bridge6Seg7MinZ && z <= bridge6Seg7MaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    
    float eighthMaxX = bridge6Seg7MinX;
    float eighthMinX = eighthMaxX - EIGHTH_ARENA_HALF * 2.0f;
    float eighthCenterZ = bridge6Seg7CenterZ - EIGHTH_ARENA_FORWARD_OFFSET;
    float eighthMinZ = eighthCenterZ - EIGHTH_ARENA_HALF;
    float eighthMaxZ = eighthCenterZ + EIGHTH_ARENA_HALF;

    if (x >= eighthMinX && x <= eighthMaxX &&
        z >= eighthMinZ && z <= eighthMaxZ) {
        return FOURTH_Y - BALL_RADIUS;
    }

    return -1e6f;
}


void getSlopeForce(float x, float z, float &outFX, float &outFZ) {
    float eps = 0.05f;

    float h0 = getGroundHeight(x, z);
    float hX = getGroundHeight(x + eps, z);
    float hZ = getGroundHeight(x, z + eps);

    if (h0 < -1e5f || hX < -1e5f || hZ < -1e5f) {
        outFX = 0.0f;
        outFZ = 0.0f;
        return;
    }

    float dydx = (hX - h0) / eps;
    float dydz = (hZ - h0) / eps;

    float slopeFactor = GRAVITY * 0.5f;

    outFX = -dydx * slopeFactor;
    outFZ = -dydz * slopeFactor;
}

static void resolveSolidBlock(float minX, float maxX,
                              float minZ, float maxZ,
                              float topY,
                              float &x, float &z,
                              float y,
                              float &vx, float &vz) {
    float ballBottom = y - BALL_RADIUS;
    float ballTop = y + BALL_RADIUS;
    float topLipTolerance = BALL_RADIUS * 0.80f;

    if (ballBottom >= topY - topLipTolerance) {
        return;
    }

    if (ballTop < BUILDING_BOTTOM) {
        return;
    }

    float bx0 = minX - BALL_RADIUS;
    float bx1 = maxX + BALL_RADIUS;
    float bz0 = minZ - BALL_RADIUS;
    float bz1 = maxZ + BALL_RADIUS;

    if (!(x > bx0 && x < bx1 && z > bz0 && z < bz1)) {
        return;
    }

    bool outsideX = x < minX || x > maxX;
    bool outsideZ = z < minZ || z > maxZ;

    if (!outsideX && !outsideZ) {
        return;
    }

    float dLeft  = x - bx0;
    float dRight = bx1 - x;
    float dFront = z - bz0;
    float dBack  = bz1 - z;

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
        x = bx0;
        vx = -std::fabs(vx) * 0.35f;
    } else if (side == 1) {
        x = bx1;
        vx = std::fabs(vx) * 0.35f;
    } else if (side == 2) {
        z = bz0;
        vz = -std::fabs(vz) * 0.35f;
    } else {
        z = bz1;
        vz = std::fabs(vz) * 0.35f;
    }
}

void resolveWorldSolidCollision(float &x, float &z, float y, float &vx, float &vz) {
    float mainY = -BALL_RADIUS;
    bool onRamp1Connector =
        std::fabs(x) <= RAMP_HALF_WIDTH + BALL_RADIUS &&
        z >= ARENA_HALF - BALL_RADIUS &&
        z <= ARENA_HALF + RAMP_LENGTH + BALL_RADIUS &&
        y >= (LOWER_Y - BALL_RADIUS);

    if (!onRamp1Connector) {
        resolveSolidBlock(-ARENA_HALF, ARENA_HALF,
                          -ARENA_HALF, ARENA_HALF,
                          mainY,
                          x, z, y, vx, vz);
    }

    float lowerZ0 = ARENA_HALF + RAMP_LENGTH;
    float lowerZ1 = lowerZ0 + LOWER_ARENA_HALF * 2.0f;
    float lowerY = LOWER_Y - BALL_RADIUS;
    resolveSolidBlock(-LOWER_ARENA_HALF, LOWER_ARENA_HALF,
                      lowerZ0, lowerZ1,
                      lowerY,
                      x, z, y, vx, vz);

    float bridgeCenterZ = ARENA_HALF + RAMP_LENGTH + LOWER_ARENA_HALF;
    resolveSolidBlock(LOWER_ARENA_HALF, LOWER_ARENA_HALF + BRIDGE2_LENGTH,
                      bridgeCenterZ - BRIDGE2_HALF_WIDTH,
                      bridgeCenterZ + BRIDGE2_HALF_WIDTH,
                      lowerY,
                      x, z, y, vx, vz);

    float thirdMinX = LOWER_ARENA_HALF + BRIDGE2_LENGTH;
    float thirdMaxX = thirdMinX + THIRD_ARENA_HALF * 2.0f;
    float thirdMinZ = bridgeCenterZ - THIRD_ARENA_HALF;
    float thirdMaxZ = bridgeCenterZ + THIRD_ARENA_HALF;
    float thirdCenterX = thirdMinX + THIRD_ARENA_HALF;
    float thirdY = LOWER_Y - BALL_RADIUS;
    float ramp2MinX;
    float ramp2MaxX;
    float ramp2Z0;
    float ramp2Z1;
    getRamp2Layout(ramp2MinX, ramp2MaxX, ramp2Z0, ramp2Z1);

    bool onRamp2Connector =
        x >= ramp2MinX - BALL_RADIUS &&
        x <= ramp2MaxX + BALL_RADIUS &&
        z >= ramp2Z1 - BALL_RADIUS &&
        z <= ramp2Z0 + BALL_RADIUS &&
        y >= (FOURTH_Y - BALL_RADIUS);

    if (!onRamp2Connector) {
        resolveSolidBlock(thirdMinX, thirdMaxX,
                          thirdMinZ, thirdMaxZ,
                          thirdY,
                          x, z, y, vx, vz);
    }

    float fourthMinX;
    float fourthMaxX;
    float fourthMinZ;
    float fourthMaxZ;
    getFourthArenaLayout(fourthMinX, fourthMaxX, fourthMinZ, fourthMaxZ);
    float fourthY = FOURTH_Y - BALL_RADIUS;
    if (!onRamp2Connector) {
        resolveSolidBlock(fourthMinX, fourthMaxX,
                          fourthMinZ, fourthMaxZ,
                          fourthY,
                          x, z, y, vx, vz);
    }

    float fourthCenterZ = (fourthMinZ + fourthMaxZ) * 0.5f;
    float bridge3MinX = fourthMaxX;
    float bridge3MaxX = fourthMaxX + BRIDGE3_LENGTH;
    float bridge3MinZ = fourthCenterZ - BRIDGE3_HALF_WIDTH;
    float bridge3MaxZ = fourthCenterZ + BRIDGE3_HALF_WIDTH;
    resolveSolidBlock(bridge3MinX, bridge3MaxX,
                      bridge3MinZ, bridge3MaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float fifthHalf = FIFTH_ARENA_HALF;
    float fifthMinX = bridge3MaxX;
    float fifthMaxX = fifthMinX + fifthHalf * 2.0f;
    float fifthMinZ = fourthCenterZ - fifthHalf;
    float fifthMaxZ = fourthCenterZ + fifthHalf;
    resolveSolidBlock(fifthMinX, fifthMaxX,
                      fifthMinZ, fifthMaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float fifthCenterX = (fifthMinX + fifthMaxX) * 0.5f;
    float bridge4MinX = fifthCenterX - BRIDGE4_HALF_WIDTH;
    float bridge4MaxX = fifthCenterX + BRIDGE4_HALF_WIDTH;
    float bridge4MaxZ = fifthMinZ;
    float bridge4MinZ = fifthMinZ - BRIDGE4_LENGTH;
    resolveSolidBlock(bridge4MinX, bridge4MaxX,
                      bridge4MinZ, bridge4MaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float sixthMinX = fifthMinX;
    float sixthMaxX = fifthMaxX;
    float sixthMaxZ = bridge4MinZ;
    float sixthMinZ = bridge4MinZ - SIXTH_ARENA_LENGTH;
    resolveSolidBlock(sixthMinX, sixthMaxX,
                      sixthMinZ, sixthMaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float sixthCenterX = (sixthMinX + sixthMaxX) * 0.5f;
    float bridge5MinX = sixthCenterX - BRIDGE5_HALF_WIDTH;
    float bridge5MaxX = sixthCenterX + BRIDGE5_HALF_WIDTH;
    float bridge5MaxZ = sixthMinZ;
    float bridge5MinZ = sixthMinZ - BRIDGE5_LENGTH;
    resolveSolidBlock(bridge5MinX, bridge5MaxX,
                      bridge5MinZ, bridge5MaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float seventhMinX = sixthMinX;
    float seventhMaxX = sixthMaxX;
    float seventhMaxZ = bridge5MinZ;
    float seventhMinZ = bridge5MinZ - SEVENTH_ARENA_LENGTH;
    resolveSolidBlock(seventhMinX, seventhMaxX,
                      seventhMinZ, seventhMaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float seventhCenterZ = (seventhMinZ + seventhMaxZ) * 0.5f;
    float bridge6Seg1MaxX = seventhMinX;
    float bridge6Seg1MinX = bridge6Seg1MaxX - BRIDGE6_SEG1_LENGTH;
    float bridge6Seg1CenterZ = seventhCenterZ;
    resolveSolidBlock(bridge6Seg1MinX, bridge6Seg1MaxX,
                      bridge6Seg1CenterZ - BRIDGE6_HALF_WIDTH,
                      bridge6Seg1CenterZ + BRIDGE6_HALF_WIDTH,
                      fourthY,
                      x, z, y, vx, vz);

    float bridge6Seg2CenterX = bridge6Seg1MinX;
    float bridge6Seg2MinX = bridge6Seg2CenterX - BRIDGE6_HALF_WIDTH;
    float bridge6Seg2MaxX = bridge6Seg2CenterX + BRIDGE6_HALF_WIDTH;
    float bridge6Seg2MinZ = bridge6Seg1CenterZ;
    float bridge6Seg2MaxZ = bridge6Seg1CenterZ + BRIDGE6_SEG2_LENGTH;
    resolveSolidBlock(bridge6Seg2MinX, bridge6Seg2MaxX,
                      bridge6Seg2MinZ, bridge6Seg2MaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float bridge6Seg3MaxX = bridge6Seg2CenterX;
    float bridge6Seg3MinX = bridge6Seg3MaxX - BRIDGE6_SEG3_LENGTH;
    float bridge6Seg3CenterZ = bridge6Seg2MaxZ;
    resolveSolidBlock(bridge6Seg3MinX, bridge6Seg3MaxX,
                      bridge6Seg3CenterZ - BRIDGE6_HALF_WIDTH,
                      bridge6Seg3CenterZ + BRIDGE6_HALF_WIDTH,
                      fourthY,
                      x, z, y, vx, vz);

    float bridge6Seg4CenterX = bridge6Seg3MinX;
    float bridge6Seg4MinX = bridge6Seg4CenterX - BRIDGE6_HALF_WIDTH;
    float bridge6Seg4MaxX = bridge6Seg4CenterX + BRIDGE6_HALF_WIDTH;
    float bridge6Seg4MinZ = bridge6Seg3CenterZ - BRIDGE6_SEG4_LENGTH;
    float bridge6Seg4MaxZ = bridge6Seg3CenterZ;
    resolveSolidBlock(bridge6Seg4MinX, bridge6Seg4MaxX,
                      bridge6Seg4MinZ, bridge6Seg4MaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float bridge6Seg5MaxX = bridge6Seg4CenterX;
    float bridge6Seg5MinX = bridge6Seg5MaxX - BRIDGE6_SEG5_LENGTH;
    float bridge6Seg5CenterZ = bridge6Seg4MinZ;
    resolveSolidBlock(bridge6Seg5MinX, bridge6Seg5MaxX,
                      bridge6Seg5CenterZ - BRIDGE6_HALF_WIDTH,
                      bridge6Seg5CenterZ + BRIDGE6_HALF_WIDTH,
                      fourthY,
                      x, z, y, vx, vz);

    float bridge6Seg6CenterX = bridge6Seg5MinX;
    float bridge6Seg6MinX = bridge6Seg6CenterX - BRIDGE6_HALF_WIDTH;
    float bridge6Seg6MaxX = bridge6Seg6CenterX + BRIDGE6_HALF_WIDTH;
    float bridge6Seg6MinZ = bridge6Seg5CenterZ;
    float bridge6Seg6MaxZ = bridge6Seg5CenterZ + BRIDGE6_SEG6_LENGTH;
    resolveSolidBlock(bridge6Seg6MinX, bridge6Seg6MaxX,
                      bridge6Seg6MinZ, bridge6Seg6MaxZ,
                      fourthY,
                      x, z, y, vx, vz);

    float bridge6Seg7MaxX = bridge6Seg6CenterX;
    float bridge6Seg7MinX = bridge6Seg7MaxX - BRIDGE6_SEG7_LENGTH;
    float bridge6Seg7CenterZ = bridge6Seg6MaxZ;
    resolveSolidBlock(bridge6Seg7MinX, bridge6Seg7MaxX,
                      bridge6Seg7CenterZ - BRIDGE6_HALF_WIDTH,
                      bridge6Seg7CenterZ + BRIDGE6_HALF_WIDTH,
                      fourthY,
                      x, z, y, vx, vz);

    float eighthMaxX = bridge6Seg7MinX;
    float eighthMinX = eighthMaxX - EIGHTH_ARENA_HALF * 2.0f;
    float eighthCenterZ = bridge6Seg7CenterZ - EIGHTH_ARENA_FORWARD_OFFSET;
    resolveSolidBlock(eighthMinX, eighthMaxX,
                      eighthCenterZ - EIGHTH_ARENA_HALF,
                      eighthCenterZ + EIGHTH_ARENA_HALF,
                      fourthY,
                      x, z, y, vx, vz);
}
