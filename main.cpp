#include <cstdlib> // included first

#ifdef __APPLE__
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
    #define HAPTIC
#endif

#define CONSTANT_UPDATE
//#define ALLOW_ADJUSTMENT


#include <iostream>
#include <string>
#include "chronoTimekeep.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "alignedImage.h"
#include "saveLoader.h"
using namespace std;

#ifdef HAPTIC
#include <windows.h>
#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>
#include <HLU/hlu.h>

/* Haptic device and rendering context handles. */
static HHD ghHD = HD_INVALID_HANDLE;
static HHLRC ghHLRC = 0;

/* Shape id for shape we will render haptically. */
HLuint gSurfaceShapeID;

#define CURSOR_SCALE_SIZE 60
static double gCursorScale;
static GLuint gCursorDisplayList = 0;

#endif

#define PRG_NAME "stereo_align"
#define MOVEMENT_RATE 0.004

alignedImage* leftImage;
alignedImage* rightImage;

const int FPS = 60;
const int fpsDispInterval = 100; //Rate to update FPS count
const int interval = 1000 / FPS; //Rate to actually redraw
Timer* fpsInterfalTimer = new Timer();
Timer* frameInterfalTimer = new Timer();

//FPS counting
double actualFPS = 0;
double displayedFPS = 0;
int frames = 0;

//Mouse tracking
int isDragging = 0;
int lastMouseX = 0;
int lastMouseY = 0;
int clickedButton = -1;

//Toggles
int stereoView = 1;
int hapticMode = 0;
int showHud = 1;

//Zoom and scrolling
double zoom = 1;
double xCam = 0;
double yCam = 0;

int usingHaptics = 0;

//Observations
vector<int> left_x_obs;
vector<int> left_y_obs;
vector<int> right_x_obs;
vector<int> right_y_obs;

//Haptic (or simulated mouse) coords
double obsX = 0;
double obsY = 0;

//Decide what image to observe
int observing = 0;


string getYesNo(int condition) {
    if (condition) return "Yes";
    return "No";
}

void calculateFPS() {
    frames++;
    double period = fpsInterfalTimer->timeSinceLastCall();
    if (period>1.0) {
        actualFPS = frames/period;
        fpsInterfalTimer->resetTime();
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

void cross(double x, double y, double arm) {
    x-=0.5;
    y-=0.5;
    x*=2;
    y*=2; //transform 0-1 to -1-1 space;
    glPushMatrix();
    glLoadIdentity();
    glBegin(GL_LINES);
    glVertex2d(x-arm,y); glVertex2d(x+arm,y);
    glVertex2d(x,y-arm); glVertex2d(x,y+arm);
    glEnd();
    glPopMatrix();
}

void drawCube(double x, double y, double w, double h) {
    glPushMatrix();
    glLoadIdentity();
    glTranslated(x, y, 0);
    glScaled(w,h,0);
    glBegin(GL_QUADS);
        glColor3f(0.0f, 0.1f, 0.2f);   

        glVertex2f(-0.5f, -0.5f);      // Bottom-left

        glVertex2f(0.5f, -0.5f);       // Bottom-right

        glVertex2f(0.5f, 0.5f);        // Top-right
        
        glVertex2f(-0.5f, 0.5f);        // Top-left
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

#ifdef HAPTIC
void drawHapticCursor()  //Taken from testTeapotHaptic Lab. Credit: Song Ho Ahn. Y. Hu, L. Ghaemi Dizaji
{
    static const double kCursorRadius = 0.5;
    static const double kCursorHeight = 1.5;
    static const int kCursorTess = 15;
    HLdouble proxyxform[16];

    GLUquadricObj* qobj = 0;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glPushMatrix();

    if (!gCursorDisplayList)
    {
        gCursorDisplayList = glGenLists(1);
        glNewList(gCursorDisplayList, GL_COMPILE);
        qobj = gluNewQuadric();

        gluCylinder(qobj, 0.0, kCursorRadius, kCursorHeight,
            kCursorTess, kCursorTess);
        glTranslated(0.0, 0.0, kCursorHeight);
        gluCylinder(qobj, kCursorRadius, 0.0, kCursorHeight / 5.0,
            kCursorTess, kCursorTess);

        gluDeleteQuadric(qobj);
        glEndList();
    }

    // Get the proxy transform in world coordinates for haptic device.
    hlGetDoublev(HL_PROXY_TRANSFORM, proxyxform);
    glMultMatrixd(proxyxform);

    // Apply the local cursor scale factor.
    glScaled(gCursorScale, gCursorScale, gCursorScale);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, 0.5, 1.0);

    glCallList(gCursorDisplayList);

    glPopMatrix();
    glPopAttrib();
}
#endif

void drawText(int width, int height) {
    if (!showHud) return;
    //Text
    glPushMatrix();
    gluOrtho2D(0, width, 0, height);

	double xSeperation = rightImage->getXAlign() - leftImage->getXAlign();
	double ySeperation = rightImage->getYAlign() - leftImage->getYAlign();

    string HUDTXT[] = {
        "ENSF545A5 Gavin+Kewei: Photogrammetry Obs Tool",
        "FPS: "+to_string(displayedFPS),
		"Seperation: " + to_string(xSeperation) + ", " + to_string(ySeperation),
		"Rotation: " + to_string(leftImage->getRotationAngle()) + ", " + to_string(rightImage->getRotationAngle()), 
#ifdef ALLOW_ADJUSTMENT
        "Adjustments enabled: Move (Mouse), Rotate (c,v)"
#else
        "Adjustments disabled (eye adjustments can be done with z, x)"
#endif
        "",
        "Hide HUD (h), Exit fullscreen (p), Quit (esc)",
        "Pan (w,s,a,d), Zoom (q,e)",
        "Window: "+to_string(width)+"x"+to_string(height),
        "Stereo Mode (space): "+getYesNo(stereoView),
        "Haptic Device Connected: " + getYesNo(usingHaptics),
        "Haptic Mode (1): "+to_string(hapticMode+1),
    };
    int HUDlength = 12;
    for (int i=0; i<HUDlength; i++) drawBitmapText(0,18*i,HUDTXT[i]);

    string obsText;

    if (observing==0) {
        obsText = "Observing Left Image";
    } else {
        obsText = "Observing Right Image";
    }

    drawBitmapText(0,height/2,obsText);

    glPopMatrix();
}

void display() {
    if (frameInterfalTimer->timeSinceLastCall()<interval/1000.0f) return; //this stops an odd glitch where display() is called twice, no need to render twice
    frameInterfalTimer->resetTime();

    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT); //clear screen

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);

    glMatrixMode(GL_MODELVIEW); //model view
    glLoadIdentity();

    glScaled(zoom,zoom,1);
    glTranslated(xCam,yCam,0);

    glEnable(GL_BLEND); //enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (stereoView) {
        glViewport(0,0,width/2,height);
        leftImage->draw();
        drawCube(0,-0.8,0.1,0.1);

        glViewport(width/2,0,width/2,height);
        rightImage->draw();
        drawCube(0,-0.8,0.1,0.1);

        //Draw mini viewports
        glViewport(0,0,width/4,height/2);
        drawCube(0,0,5,5);
        if (observing) {
            leftImage->draw();
            rightImage->draw();
        } else {
            rightImage->draw();
            leftImage->draw();
        }
        
        cross(obsX,obsY,0.5);

        glViewport(width/2,0,width/4,height/2);
        drawCube(0,0,5,5);
        if (observing) {
            leftImage->draw();
            rightImage->draw();
        } else {
            rightImage->draw();
            leftImage->draw();
        }
        cross(obsX,obsY,0.5);
    } else {
        glViewport(0,0,width,height);
        leftImage->draw(0.5);
        rightImage->draw(0.5);
    }

#ifdef HAPTIC
    drawHapticCursor();
#endif

    glDisable(GL_BLEND);

    glMatrixMode(GL_MODELVIEW); //model view
    glLoadIdentity();
    if (stereoView) {
        glViewport(0,0,width/5,height);
        drawText(width/2, height);

        glViewport(width/2,0,width/5,height);
        drawText(width/2, height);
    } else {
        drawText(width, height);
    }

    glutSwapBuffers();
    calculateFPS();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
#ifdef ALLOW_ADJUSTMENT
			saveToFile(leftImage->getXAlign(), leftImage->getYAlign(), rightImage->getXAlign(), rightImage->getYAlign(), leftImage->getRotationAngle(), rightImage->getRotationAngle());
#endif
            std::exit(0);
            break;
        case 's':
            yCam+=0.25/zoom;
            break;
        case 'w':
            yCam+=-0.25/zoom;
            break;
        case 'd':
            xCam-=0.25/zoom;
            break;
        case 'a':
            xCam+=0.25/zoom;
            break;
        case 'q':
            zoom/=2;
            break;
        case 'e':
            zoom*=2;
            break;
        case ' ':
            stereoView=!stereoView;
            break;
        case '1':
            hapticMode=!hapticMode;
            break;
        case 'p':
            glutReshapeWindow(300, 300);
            break;
        case 'h':
            showHud=!showHud;
            break;
        case 'z':
            leftImage->alignAdd(0.001, 0);
            rightImage->alignAdd(-0.001, 0);
            break;
        case 'x':
            leftImage->alignAdd(-0.001, 0);
            rightImage->alignAdd(0.001, 0);
            break;
#ifdef ALLOW_ADJUSTMENT
        case 'c':
			leftImage->rotateAdd(0.1);
			break;
        case 'v':
            leftImage->rotateAdd(-0.1);
            break;
#endif
    }
    glutPostRedisplay();
}

void mouseCallback(int button, int state, int x, int y) {
#ifdef ALLOW_ADJUSTMENT
    if (state == GLUT_DOWN) {
        isDragging = 1;
        clickedButton = button; 
        lastMouseX = x;
        lastMouseY = y;
    } else if (state == GLUT_UP) {
        isDragging = 0;
        clickedButton = -1;
    }
#endif
}

void passiveMotion(int x, int y) {
    if (!usingHaptics) {
        int width = glutGet(GLUT_WINDOW_WIDTH);
        int height = glutGet(GLUT_WINDOW_HEIGHT);

        obsX = x*1.0/width;
        obsY = 1.0-(y*1.0/height);
    }
}

void motionCallback(int x, int y) {
    int width = glutGet(GLUT_WINDOW_WIDTH);
    if (stereoView) width/=2;
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    if (isDragging) {
        int deltaX = x - lastMouseX;
        int deltaY = y - lastMouseY;

        double moveX = deltaX*1.0/width*2/zoom;
        double moveY = -deltaY*1.0/height*2/zoom;


        if (clickedButton == GLUT_LEFT_BUTTON) {
            leftImage->alignAdd(moveX,moveY);
        } else if (clickedButton == GLUT_RIGHT_BUTTON) {
            rightImage->alignAdd(moveX,moveY);
        }

        lastMouseX = x;
        lastMouseY = y;

        glutPostRedisplay(); 
    }
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

#ifdef HAPTIC
void HLCALLBACK touchShapeCallback(HLenum event, HLuint object, HLenum thread,
    HLcache* cache, void* userdata)
{

}

void drawSceneHaptics()
{
    // Start haptic frame.  (Must do this before rendering any haptic shapes.)
    hlBeginFrame();

    // Set material properties for the shapes to be drawn.
    hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.7f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.1f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.0f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 0.0f);
    if (hapticMode) {
        hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.7f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.1f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.0f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 0.0f);
    } else {
        hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.7f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.1f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 1.0f);
        hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 1.0f);
	}

    // Start a new haptic shape.  Use the feedback buffer to capture OpenGL 
    // geometry for haptic rendering.
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gSurfaceShapeID);
    glPushMatrix();
    leftImage->draw();
    glPopMatrix();
    hlEndShape();

    // End the haptic frame.
    hlEndFrame();

    // Call any event callbacks that have been triggered.
    hlCheckEvents();

}

#endif

void initHL() { //Taken from testTeapotHaptic Lab. Credit: Song Ho Ahn. Y. Hu, L. Ghaemi Dizaji
#ifdef HAPTIC
    HDErrorInfo error;

    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        return;
    }

	usingHaptics = 1;


    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC);

    // Enable optimization of the viewing parameters when rendering
    // geometry for OpenHaptics.
    hlEnable(HL_HAPTIC_CAMERA_VIEW);

    // Generate id's for the teapot shape and the sphere shape.
    gSurfaceShapeID = hlGenShapes(1);

    // Setup event callbacks.
    hlAddEventCallback(HL_EVENT_TOUCH, HL_OBJECT_ANY, HL_CLIENT_THREAD,
        &touchShapeCallback, NULL);


    hlTouchableFace(HL_FRONT); // define force feedback from front faces of teapot
#endif
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    
    glutCreateWindow(PRG_NAME);
    glutFullScreen();

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    glutMouseFunc(mouseCallback);
    glutMotionFunc(motionCallback);
    glutPassiveMotionFunc(passiveMotion);

#ifdef CONSTANT_UPDATE
    glutTimerFunc(0, timer, 0);
    //glutIdleFunc(idleFunc);
#endif

    glutTimerFunc(0, fps_timer, 0);

    initHL();

    int widthL, heightL, nrChannelsL;
    unsigned char *dataL = stbi_load("images/dec_1_027_d.jpg", &widthL, &heightL, &nrChannelsL, 0); 
    int widthR, heightR, nrChannelsR;
    unsigned char *dataR = stbi_load("images/dec_1_028_d.jpg", &widthR, &heightR, &nrChannelsR, 0); 

    if (!dataL || !dataR) {
        cout << "Failed to load textures!" << endl;
        std::exit(0);
    }

    //Setup objects
    unsigned int leftImageTID;
    glGenTextures(1, &leftImageTID);
    glBindTexture(GL_TEXTURE_2D, leftImageTID);
    leftImage = new alignedImage(leftImageTID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthL, heightL, 0, GL_RGB, GL_UNSIGNED_BYTE, dataL);
    //glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int rightImageTID;
    glGenTextures(1, &rightImageTID);
    glBindTexture(GL_TEXTURE_2D, rightImageTID);
    rightImage = new alignedImage(rightImageTID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, widthR, heightR, 0, GL_RGB, GL_UNSIGNED_BYTE, dataR);
    //glGenerateMipmap(GL_TEXTURE_2D);

	double leftXAlign, leftYAlign, rightXAlign, rightYAlign;
	leftXAlign = leftYAlign = rightXAlign = rightYAlign = 0;
	double leftRotationAngle = 0, rightRotationAngle = 0;

    loadFromFile(leftXAlign, leftYAlign, rightXAlign, rightYAlign, leftRotationAngle, rightRotationAngle);
	leftImage->align(leftXAlign, leftYAlign);
	rightImage->align(rightXAlign, rightYAlign);
	leftImage->rotate(leftRotationAngle);
	rightImage->rotate(rightRotationAngle);

    glutMainLoop();
    return 0;
}
