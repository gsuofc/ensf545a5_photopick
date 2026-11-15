#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
#endif

#include "alignedImage.h"

alignedImage::alignedImage(unsigned int tid) {
    textureID = tid;
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

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glLoadIdentity();
    glTranslated(xAlign, yAlign, 0);
    glBegin(GL_QUADS);
        glColor4f(1.0f, 1.0f, 1.0f, 0.3f);   // make kind of transparent

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-0.5f, -0.5f);      // Bottom-left

        glTexCoord2f(1.0f, 0.0f); 
        glVertex2f(0.5f, -0.5f);       // Bottom-right

        glTexCoord2f(1.0f, 1.0f); 
        glVertex2f(0.5f, 0.5f);        // Top-right
        
        glTexCoord2f(0.0f, 1.0f); 
        glVertex2f(-0.5f, 0.5f);        // Top-left
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}