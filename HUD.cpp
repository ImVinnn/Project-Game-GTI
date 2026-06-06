#include "HUD.h"
#include <GL/glut.h>
#include <cstdio>

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while(*text)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text++);
}

void drawHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    char buf[64];
    sprintf(buf, "Time: %.1f", timeLeft);
    glColor3f(1,1,1);
    drawText(10, h - 30, buf);

    sprintf(buf, "Stars: %d/%d", score / 10, WIN_STAR_COUNT);
    drawText(10, h - 60, buf);

    if(gameState == GAMEOVER) {
        glColor3f(1, 0.2f, 0.2f);
        drawText(w/2 - 80, h/2, "GAME OVER! Press R to restart");
    }
    if(gameState == WIN) {
        glColor3f(0.2f, 1, 0.2f);
        drawText(w/2 - 60, h/2, "YOU WIN! Press R to restart");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
