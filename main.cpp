#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#define CONSTANT_UPDATE

#include <cstdlib>
#include <iostream>
#include <time.h>
#include <string>
#include "alignedImage.h"
using namespace std;

#define PRG_NAME "stereo_align"

alignedImage* leftImage = new alignedImage();

const int FPS = 60;
const int fpsInterval = 1000;

const int interval = 1000 / FPS;
double actualFps = 0;
double displayedFPS = 0;
int frameCount = 0;

void drawBitmapText(float x, float y, const std::string& text) {
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    y = height-y-18;
    glColor3f(1.0f, 1.0f, 1.0f); 
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void display() {
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT);

    leftImage->draw();

    //Text
    glPushMatrix();
    gluOrtho2D(0, width, 0, height);
    drawBitmapText(0,0,"FPS: "+to_string(displayedFPS));
    drawBitmapText(0,18,"Window: "+to_string(width)+"x"+to_string(height));
    glPopMatrix();

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;
        case 'w':
            leftImage->alignAdd(0,0.1);
            break;
        case 's':
            leftImage->alignAdd(0,-0.1);
            break;
        case 'a':
            leftImage->alignAdd(-0.1,0);
            break;
        case 'd':
            leftImage->alignAdd(0.1,0);
            break;
    }
    frameCount++;
    glutPostRedisplay();
}

//In case we decide for a constant FPS update
void timer(int value) {
    frameCount++;
    glutPostRedisplay(); 
    glutTimerFunc(interval, timer, 0); 
}

void fps_timer(int value) {
    displayedFPS = frameCount*1000/fpsInterval;
    frameCount = 0;
    glutTimerFunc(fpsInterval, fps_timer, 0); 
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
    glutCreateWindow(PRG_NAME);
    glutFullScreen();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

#ifdef CONSTANT_UPDATE
    glutTimerFunc(0, timer, 0);
#endif

    glutTimerFunc(0, fps_timer, 0);

    glutMainLoop();
    return 0;
}
