#include "Ball.h"
#include "Lighting.h"
#include "Physics.h"
#include <GL/glut.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>

static GLuint ballTexID = 0;

static float rotMatrix[16] = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};

static const float CIRCUM = 2.0f * PI * BALL_RADIUS; 

static void mulMatrix(const float a[16], const float b[16], float out[16]) {
    for(int col = 0; col < 4; col++) {
        for(int row = 0; row < 4; row++) {
            out[col*4+row] = 0;
            for(int k = 0; k < 4; k++)
                out[col*4+row] += a[k*4+row] * b[col*4+k];
        }
    }
}

static void makeRotMatrix(float ax, float az, float rad, float out[16]) {
    float c = std::cos(rad);
    float s = std::sin(rad);
    float t = 1.0f - c;

    out[0]  = t*ax*ax + c;    out[4]  = 0;     out[8]  = t*ax*az + s*az;  out[12] = 0;
    out[1]  = s*az;            out[5]  = c;     out[9]  = -s*ax;           out[13] = 0;
    out[2]  = t*ax*az - s*az; out[6]  = s*ax;  out[10] = t*az*az + c;     out[14] = 0;
    out[3]  = 0;               out[7]  = 0;     out[11] = 0;               out[15] = 1;
}

void updateBallRotation(float vx, float vz) {
    float speed = std::sqrt(vx*vx + vz*vz);
    if(speed < 0.0001f) return;

    float axisX =  vz / speed;
    float axisZ = -vx / speed;

    float deltaRad = (speed / CIRCUM) * 2.0f * PI; 

    float frameMat[16];
    makeRotMatrix(axisX, axisZ, deltaRad, frameMat);

    float tmp[16];
    mulMatrix(frameMat, rotMatrix, tmp);
    for(int i = 0; i < 16; i++) rotMatrix[i] = tmp[i];

    
    float x0=rotMatrix[0], x1=rotMatrix[1], x2=rotMatrix[2];
    float y0=rotMatrix[4], y1=rotMatrix[5], y2=rotMatrix[6];

    float lenX = std::sqrt(x0*x0+x1*x1+x2*x2);
    if(lenX > 0) { x0/=lenX; x1/=lenX; x2/=lenX; }

    float dot = x0*y0+x1*y1+x2*y2;
    y0-=dot*x0; y1-=dot*x1; y2-=dot*x2;
    float lenY = std::sqrt(y0*y0+y1*y1+y2*y2);
    if(lenY > 0) { y0/=lenY; y1/=lenY; y2/=lenY; }

    float z0=x1*y2-x2*y1, z1=x2*y0-x0*y2, z2=x0*y1-x1*y0;

    rotMatrix[0]=x0; rotMatrix[1]=x1; rotMatrix[2]=x2;  rotMatrix[3]=0;
    rotMatrix[4]=y0; rotMatrix[5]=y1; rotMatrix[6]=y2;  rotMatrix[7]=0;
    rotMatrix[8]=z0; rotMatrix[9]=z1; rotMatrix[10]=z2; rotMatrix[11]=0;
    rotMatrix[12]=0; rotMatrix[13]=0; rotMatrix[14]=0;  rotMatrix[15]=1;
}

void loadBallTexture(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        printf("ERROR: Tidak bisa buka file tekstur: %s\n", filename);
        return;
    }

    unsigned char header[54];
    fread(header, 1, 54, f);

    int width      = *(int*)&header[18];
    int height     = *(int*)&header[22];
    int dataOffset = *(int*)&header[10];

    fseek(f, dataOffset, SEEK_SET);

    int size = width * height * 3;
    unsigned char* data = (unsigned char*)malloc(size);
    fread(data, 1, size, f);
    fclose(f);

    for (int i = 0; i < width * height; i++) {
        unsigned char tmp  = data[i*3];
        data[i*3]          = data[i*3+2];
        data[i*3+2]        = tmp;
    }

    glGenTextures(1, &ballTexID);
    glBindTexture(GL_TEXTURE_2D, ballTexID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, data);

    free(data);
    printf("Tekstur berhasil dimuat: %s (%dx%d)\n", filename, width, height);
}


static void glBallShadowProjection(float* l, float* e, float* n) {
    float d, c;
    float mat[16];

    d = n[0] * l[0] + n[1] * l[1] + n[2] * l[2];
    c = e[0] * n[0] + e[1] * n[1] + e[2] * n[2] - d;

    mat[0]  = l[0] * n[0] + c;
    mat[4]  = n[1] * l[0];
    mat[8]  = n[2] * l[0];
    mat[12] = -l[0] * c - l[0] * d;

    mat[1]  = n[0] * l[1];
    mat[5]  = l[1] * n[1] + c;
    mat[9]  = n[2] * l[1];
    mat[13] = -l[1] * c - l[1] * d;

    mat[2]  = n[0] * l[2];
    mat[6]  = n[1] * l[2];
    mat[10] = l[2] * n[2] + c;
    mat[14] = -l[2] * c - l[2] * d;

    mat[3]  = n[0];
    mat[7]  = n[1];
    mat[11] = n[2];
    mat[15] = -d;

    glMultMatrixf(mat);
}

void drawBallShadow() {
    float groundY = getGroundHeight(ballX, ballZ);
    if (groundY < -1e5f) return;

    float shadowY = groundY + 0.018f;
    float heightAbove = (ballY - BALL_RADIUS) - groundY;
    if (heightAbove < 0.0f) heightAbove = 0.0f;

    float alpha = 0.5f - heightAbove * 0.06f;
    if (alpha < 0.05f) return;
    if (alpha > 0.5f)  alpha = 0.5f;

    float lightPos[] = { SCENE_LIGHT_X, SCENE_LIGHT_Y, SCENE_LIGHT_Z };
    float planePoint[] = { 0.0f, shadowY, 0.0f };
    float planeNormal[] = { 0.0f, -40.0f, 0.0f };

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glColor4f(0.0f, 0.0f, 0.0f, alpha);

    glPushMatrix();
        glBallShadowProjection(lightPos, planePoint, planeNormal);
        glTranslatef(ballX, ballY, ballZ);
        GLUquadric* q = gluNewQuadric();
        gluSphere(q, BALL_RADIUS, 24, 16);
        gluDeleteQuadric(q);
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glPopAttrib();
}

void drawBall() {
    glPushMatrix();
        glTranslatef(ballX, ballY, ballZ);
        glMultMatrixf(rotMatrix);

        if (ballTexID != 0) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, ballTexID);
            glColor3f(1.0f, 1.0f, 1.0f);

            GLUquadric* q = gluNewQuadric();
            gluQuadricTexture(q, GL_TRUE);
            gluQuadricNormals(q, GLU_SMOOTH);
            gluSphere(q, BALL_RADIUS, 32, 32);
            gluDeleteQuadric(q);

            glDisable(GL_TEXTURE_2D);
        } else {
            glColor3f(0.2f, 0.6f, 1.0f);
            GLUquadric* q = gluNewQuadric();
            gluSphere(q, BALL_RADIUS, 24, 24);
            gluDeleteQuadric(q);
        }

    glPopMatrix();
}
