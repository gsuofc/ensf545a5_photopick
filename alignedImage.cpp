#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include "alignedImage.h"

alignedImage::alignedImage() {

}
alignedImage::~alignedImage() {

}

void alignedImage::align(double x, double y) {
    xAlign = x;
    yAlign = y;
}

void alignedImage::alignAdd(double x, double y) {
    xAlign += x;
    yAlign += y;
}

void alignedImage::draw() {
    glPushMatrix();
    glLoadIdentity();
    glTranslated(xAlign, yAlign, 0);
    glBegin(GL_QUADS);
        glColor3f(1.0f, 0.0f, 0.0f);   // Red
        glVertex2f(-0.5f, -0.5f);      // Bottom-left

        glColor3f(0.0f, 1.0f, 0.0f);   // Green
        glVertex2f(0.5f, -0.5f);       // Bottom-right

        glColor3f(0.0f, 0.0f, 1.0f);   // Blue
        glVertex2f(0.5f, 0.5f);        // Top-right
        
        glColor3f(0.0f, 0.0f, 0.0f);   // Black
        glVertex2f(-0.5f, 0.5f);        // Top-left
    glEnd();
    glPopMatrix();
}