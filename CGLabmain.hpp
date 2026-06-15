/*
 TCG6223 Computer Graphics
 FIST, Multimedia University

 CGLabmain.hpp
*/

#ifndef YP_CGLABMAIN_HPP
#define YP_CGLABMAIN_HPP

#include <string>
#include <cmath>
#include <iostream>
#include <GL/glut.h>

using namespace std;

void myDisplayFunc(void);
void myReshapeFunc(int width, int height);
void myKeyboardFunc(unsigned char key, int x, int y);
void mySpecialFunc(int key, int x, int y);
void myMouseFunc(int button, int state, int x, int y);
void myMotionFunc(int x, int y);
void myTimerFunc(int value);

void myDataInit();
void myViewingInit();
void myInit();
void myWelcome();
void myLightingInit();

static GLfloat myblack[]   = { 0.0f, 0.0f, 0.0f, 1.0f };
static GLfloat mywhite[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat myred[]     = { 1.0f, 0.0f, 0.0f, 1.0f };
static GLfloat mygreen[]   = { 0.0f, 1.0f, 0.0f, 1.0f };
static GLfloat myblue[]    = { 0.0f, 0.0f, 1.0f, 1.0f };
static GLfloat mycyan[]    = { 0.0f, 1.0f, 1.0f, 1.0f };
static GLfloat mymagenta[] = { 1.0f, 0.0f, 1.0f, 1.0f };
static GLfloat myyellow[]  = { 1.0f, 1.0f, 0.0f, 1.0f };

static GLfloat mygray10Percent[] = { 0.1f, 0.1f, 0.1f, 1.0f };
static GLfloat mygray20Percent[] = { 0.2f, 0.2f, 0.2f, 1.0f };
static GLfloat mygray30Percent[] = { 0.3f, 0.3f, 0.3f, 1.0f };

struct MyWindow {
 string title;
 GLint posX, posY, width, height;
};

struct MyWorld {
 GLdouble rotateX, rotateY, rotateZ;
 GLdouble posX, posY, posZ;
 GLdouble scaleX, scaleY, scaleZ;
 void move(GLfloat xinc, GLfloat yinc, GLfloat zinc) { posX+=xinc; posY+=yinc; posZ+=zinc; }
 void rotate(GLfloat xinc, GLfloat yinc, GLfloat zinc) { rotateX+=xinc; rotateY+=yinc; rotateZ+=zinc; }
};

struct MyViewer {
 GLdouble eyeX, eyeY, eyeZ;
 GLdouble centerX, centerY, centerZ;
 GLdouble upX, upY, upZ;
 GLdouble zNear, zFar;
 GLdouble fieldOfView;
 GLdouble aspectRatio;
};

struct MySetting {
 GLdouble posInc, angleInc;
 int mouseX, mouseY;
 bool mouseRightMode, mouseLeftMode, shadingMode;
};

class MyAxis {
 private:
    GLfloat xlen, ylen, zlen;
    GLfloat xred, xgreen, xblue;
    GLfloat yred, ygreen, yblue;
    GLfloat zred, zgreen, zblue;
    GLint    linestipplerepeatfactor;
    GLfloat  linewidth;
    GLushort linestipplepattern;
    bool     shoulddraw;
 public:
    MyAxis() {
     setXColor(1.0f,0.0f,0.0f); setYColor(0.0f,1.0f,0.0f); setZColor(0.0f,0.0f,1.0f);
     setLength(20.0f,20.0f,20.0f); setLineStipple(1,0xffff); setLineWidth(1); shoulddraw=true;
    }
    void setLength(GLfloat xLen,GLfloat yLen,GLfloat zLen){ xlen=xLen; ylen=yLen; zlen=zLen; }
    void setXColor(GLfloat r,GLfloat g,GLfloat b){ xred=r; xgreen=g; xblue=b; }
    void setYColor(GLfloat r,GLfloat g,GLfloat b){ yred=r; ygreen=g; yblue=b; }
    void setZColor(GLfloat r,GLfloat g,GLfloat b){ zred=r; zgreen=g; zblue=b; }
    void setLineWidth(GLfloat lw){ linewidth=lw; }
    void setLineStipple(GLint rf,GLushort p){ linestipplerepeatfactor=rf; linestipplepattern=p; }
    void draw() {
     if (!shoulddraw) return;
     GLboolean lightingIsOn; glGetBooleanv(GL_LIGHTING,&lightingIsOn); glDisable(GL_LIGHTING);
     GLint oldrf, oldp; GLfloat oldlw; GLboolean stipOn;
     glGetIntegerv(GL_LINE_STIPPLE_REPEAT,&oldrf); glGetIntegerv(GL_LINE_STIPPLE_PATTERN,&oldp);
     glGetFloatv(GL_LINE_WIDTH,&oldlw); glGetBooleanv(GL_LINE_STIPPLE,&stipOn);
     glEnable(GL_LINE_STIPPLE); glLineWidth(linewidth); glLineStipple(linestipplerepeatfactor,linestipplepattern);
     glBegin(GL_LINES);
        glColor3f(xred,xgreen,xblue);
        glVertex3f(0,0,0); glVertex3f(xlen,0,0);
        glVertex3f(xlen,0,0); glVertex3f(0.9f*xlen,0,0.05f*xlen);
        glVertex3f(xlen,0,0); glVertex3f(0.9f*xlen,0,-0.05f*xlen);
        glColor3f(yred,ygreen,yblue);
        glVertex3f(0,0,0); glVertex3f(0,ylen,0);
        glVertex3f(0,ylen,0); glVertex3f(0.05f*ylen,0.9f*ylen,0);
        glVertex3f(0,ylen,0); glVertex3f(-0.05f*ylen,0.9f*ylen,0);
        glColor3f(zred,zgreen,zblue);
        glVertex3f(0,0,0); glVertex3f(0,0,zlen);
        glVertex3f(0,0,zlen); glVertex3f(0.05f*zlen,0,0.9f*zlen);
        glVertex3f(0,0,zlen); glVertex3f(-0.05f*zlen,0,0.9f*zlen);
     glEnd();
     glLineWidth(oldlw); glLineStipple(oldrf,oldp);
     if (stipOn==GL_FALSE) glDisable(GL_LINE_STIPPLE);
     if (lightingIsOn==GL_TRUE) glEnable(GL_LIGHTING);
    }
    void toggle(){ shoulddraw=!shoulddraw; }
    void enable(){ shoulddraw=true; }
    void disable(){ shoulddraw=false; }
};

#endif
