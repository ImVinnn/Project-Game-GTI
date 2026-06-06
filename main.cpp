#include <GL/glut.h>
#include "GameLogic.h"
#include "Ball.h"

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1900, 1000);
    glutCreateWindow("RollingBalll Game");

    initGL();
    loadBallTexture("ballt.bmp"); 
    resetGame();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);

    lastTime = glutGet(GLUT_ELAPSED_TIME);
    lastSpawnTime = lastTime / 1000.0f;
    glutTimerFunc(16, timerCallback, 0);

    glutMainLoop();
    return 0;
}
