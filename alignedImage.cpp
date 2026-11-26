#include "alignedImage.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

alignedImage::alignedImage(unsigned int tid) {
    textureID = tid;
    xAlign = 0;
    yAlign = 0;
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

void alignedImage::drawWithoutRotate(float opacity) {
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//We want 2d image to be nearest
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //glLoadIdentity();
    glTranslated(xAlign, yAlign, 0);
    glBegin(GL_QUADS);
        glColor4f(1.0f, 1.0f, 1.0f, opacity);   

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

void alignedImage::draw(float opacity) {
    glPushMatrix();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//We want 3d image to be linear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //glLoadIdentity();
    glTranslated(xAlign, yAlign, 0);
	glRotated(rotationAngle, 0, 0, 1); // Rotate around Z-axis
    glBegin(GL_QUADS);
        glColor4f(1.0f, 1.0f, 1.0f, opacity);   

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