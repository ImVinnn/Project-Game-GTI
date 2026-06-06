

#include <GL/glut.h>

static const float SCENE_LIGHT_X = -45.0f;
static const float SCENE_LIGHT_Y =  32.0f;
static const float SCENE_LIGHT_Z = -35.0f;

inline void applySceneLight()
{
    GLfloat lightPos[] = { SCENE_LIGHT_X, SCENE_LIGHT_Y, SCENE_LIGHT_Z, 1.0f };
    GLfloat ambient[]  = { 0.28f, 0.28f, 0.30f, 1.0f };
    GLfloat diffuse[]  = { 1.00f, 0.96f, 0.88f, 1.0f };
    GLfloat specular[] = { 0.35f, 0.32f, 0.28f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}
inline void buildSceneShadowMatrix(float mat[16], float planeY)
{
    const float a = 0.0f, b = 1.0f, c = 0.0f, pd = -planeY;
    const float lx = SCENE_LIGHT_X;
    const float ly = SCENE_LIGHT_Y;
    const float lz = SCENE_LIGHT_Z;
    const float dot = a * lx + b * ly + c * lz + pd;

    mat[0]  = dot - lx * a;    mat[4]  = 0.0f - lx * b;    mat[8]  = 0.0f - lx * c;    mat[12] = 0.0f - lx * pd;
    mat[1]  = 0.0f - ly * a;   mat[5]  = dot  - ly * b;    mat[9]  = 0.0f - ly * c;    mat[13] = 0.0f - ly * pd;
    mat[2]  = 0.0f - lz * a;   mat[6]  = 0.0f - lz * b;    mat[10] = dot  - lz * c;    mat[14] = 0.0f - lz * pd;
    mat[3]  = 0.0f - 1.0f * a; mat[7]  = 0.0f - 1.0f * b;  mat[11] = 0.0f - 1.0f * c;  mat[15] = dot  - 1.0f * pd;
}
