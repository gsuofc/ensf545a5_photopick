#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#define CONSTANT_UPDATE

#include <cstdlib>
#include <iostream>
#include <string>
#include "chronoTimekeep.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "alignedImage.h"
using namespace std;

#define PRG_NAME "stereo_align"
#define MOVEMENT_RATE 0.004

alignedImage* leftImage;
alignedImage* rightImage;

const int FPS = 60;
const int fpsDispInterval = 100; //Rate to update FPS count
const int interval = 1000 / FPS; //Rate to actually redraw

//FPS counting
double actualFPS = 0;
double displayedFPS = 0;
int frames = 0;

void calculateFPS() {
    frames++;
    double period = timeSinceLastCall();
    if (period>1.0) {
        actualFPS = frames/period;
        resetTime();
        frames=0;
    }
}

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

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    leftImage->draw();
    rightImage->draw();

    glDisable(GL_BLEND);

    //Text
    glPushMatrix();
    gluOrtho2D(0, width, 0, height);
    drawBitmapText(0,0,"FPS: "+to_string(displayedFPS));
    drawBitmapText(0,18,"Window: "+to_string(width)+"x"+to_string(height));
    glPopMatrix();

    glutSwapBuffers();
    calculateFPS();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;
        case 'w':
            leftImage->alignAdd(0,MOVEMENT_RATE);
            break;
        case 's':
            leftImage->alignAdd(0,-MOVEMENT_RATE);
            break;
        case 'a':
            leftImage->alignAdd(-MOVEMENT_RATE,0);
            break;
        case 'd':
            leftImage->alignAdd(MOVEMENT_RATE,0);
            break;
    }
    glutPostRedisplay();
}

//In case we decide for a constant FPS update
void timer(int value) {
    glutTimerFunc(interval, timer, 0); 
    glutPostRedisplay(); 
}

void idleFunc() {
    glutPostRedisplay(); 
}

void fps_timer(int value) {
    displayedFPS = actualFPS;
    glutTimerFunc(fpsDispInterval, fps_timer, 0); 
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
    //glutTimerFunc(0, timer, 0);
    glutIdleFunc(idleFunc);
#endif

    glutTimerFunc(0, fps_timer, 0);

    int widthL, heightL, nrChannelsL;
    unsigned char *dataL = stbi_load("images/8MGY17062_034.jpg", &widthL, &heightL, &nrChannelsL, 0); 
    int widthR, heightR, nrChannelsR;
    unsigned char *dataR = stbi_load("images/8MGY17062_033.jpg", &widthR, &heightR, &nrChannelsR, 0); 

    if (!dataL || !dataR) {
        cout << "Failed to load textures!" << endl;
        exit(0);
    }

    //Setup objects
    unsigned int leftImageTID;
    glGenTextures(1, &leftImageTID);
    glBindTexture(GL_TEXTURE_2D, leftImageTID);
    leftImage = new alignedImage(leftImageTID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthL, heightL, 0, GL_RGB, GL_UNSIGNED_BYTE, dataL);
    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int rightImageTID;
    glGenTextures(1, &rightImageTID);
    glBindTexture(GL_TEXTURE_2D, rightImageTID);
    rightImage = new alignedImage(rightImageTID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthR, heightR, 0, GL_RGB, GL_UNSIGNED_BYTE, dataR);
    glGenerateMipmap(GL_TEXTURE_2D);

    glutMainLoop();
    return 0;
}
