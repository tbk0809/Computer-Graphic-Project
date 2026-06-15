#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <GL/glut.h>

#include "CGLabmain.hpp"
#include "CGLab03.hpp"

using CGLab03::MyVirtualWorld;

MyVirtualWorld myvirtualworld;

using namespace std;

MyWindow   window;
MyWorld    world;
MyViewer   viewer;
MySetting  setting;
MyAxis     worldaxis;

void myDisplayFunc(void) {
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glPushMatrix();
    glTranslatef(world.posX, world.posY, world.posZ);
    glRotatef(world.rotateX, 1,0,0);
    glRotatef(world.rotateY, 0,1,0);
    glRotatef(world.rotateZ, 0,0,1);
    glScalef(world.scaleX, world.scaleY, world.scaleZ);
    worldaxis.draw();
    myvirtualworld.draw();
 glPopMatrix();
 glFlush();
 glutSwapBuffers();
 myvirtualworld.tickTime();
 glutPostRedisplay();
}

void myReshapeFunc(int width, int height) {
 window.width=width; window.height=height;
 glViewport(0,0,width,height);
}

void myKeyboardFunc(unsigned char key, int x, int y) {
 GLfloat xinc=0,yinc=0,zinc=0;
 switch (key) {
    case 'a': case 'A': xinc=-setting.posInc; break;
    case 'd': case 'D': xinc= setting.posInc; break;
    case 'q': case 'Q': yinc=-setting.posInc; break;
    case 'e': case 'E': yinc= setting.posInc; break;
    case 'w': case 'W': zinc=-setting.posInc; break;
    case 's': case 'S': zinc= setting.posInc; break;
    case 27: exit(1); break;
 }
 world.move(xinc,yinc,zinc);
 glutPostRedisplay();
}

void mySpecialFunc(int key, int x, int y) {
 switch (key) {
    case GLUT_KEY_DOWN : world.rotateX-=setting.angleInc; break;
    case GLUT_KEY_UP   : world.rotateX+=setting.angleInc; break;
    case GLUT_KEY_LEFT : world.rotateY-=setting.angleInc; break;
    case GLUT_KEY_RIGHT: world.rotateY+=setting.angleInc; break;
    case GLUT_KEY_HOME : myDataInit(); break;
    case GLUT_KEY_F1:
        setting.shadingMode=!setting.shadingMode;
        glPolygonMode(GL_FRONT_AND_BACK, setting.shadingMode?GL_FILL:GL_LINE);
        break;
    case GLUT_KEY_F2: worldaxis.toggle(); break;
    case GLUT_KEY_F3: {
        GLboolean on; glGetBooleanv(GL_LIGHTING,&on);
        if (on==GL_TRUE) glDisable(GL_LIGHTING); else glEnable(GL_LIGHTING);
        break;
    }
 }
 glutPostRedisplay();
}

void myMouseFunc(int button, int state, int x, int y) {
 y=window.height-y;
 switch (button) {
    case GLUT_RIGHT_BUTTON:
        if (state==GLUT_DOWN && !setting.mouseRightMode) { setting.mouseX=x; setting.mouseY=y; setting.mouseRightMode=true; }
        if (state==GLUT_UP   &&  setting.mouseRightMode) { setting.mouseRightMode=false; }
        break;
    case GLUT_LEFT_BUTTON:
        if (state==GLUT_DOWN && !setting.mouseLeftMode) { setting.mouseX=x; setting.mouseY=y; setting.mouseLeftMode=true; }
        if (state==GLUT_UP   &&  setting.mouseLeftMode) { setting.mouseLeftMode=false; }
        break;
 }
}

void myMotionFunc(int x, int y) {
 y=window.height-y;
 GLint xinc=x-setting.mouseX, yinc=y-setting.mouseY;
 if (setting.mouseRightMode) world.rotate(0,0,-xinc*0.5f);
 if (setting.mouseLeftMode)  world.rotate(-yinc*0.5f,xinc*0.5f,0);
 setting.mouseX=x; setting.mouseY=y;
 glutPostRedisplay();
}

void myDataInit() {
 window.title="TCG6223 Computer Graphics";
 window.posX=100; window.posY=100; window.width=800; window.height=500;
 world.rotateX=world.rotateY=world.rotateZ=0;
 world.posX=world.posY=world.posZ=0;
 world.scaleX=world.scaleY=world.scaleZ=1;
 viewer.eyeX=0; viewer.eyeY=0; viewer.eyeZ=40;
 viewer.centerX=viewer.centerY=viewer.centerZ=0;
 viewer.upX=0; viewer.upY=1; viewer.upZ=0;
 viewer.zNear=0.1; viewer.zFar=500;
 viewer.fieldOfView=60;
 viewer.aspectRatio=(GLdouble)window.width/window.height;
 setting.posInc=1.0; setting.angleInc=2.0;
 setting.mouseX=setting.mouseY=0;
 setting.mouseRightMode=setting.mouseLeftMode=false;
 setting.shadingMode=true;
}

void myViewingInit() {
 glMatrixMode(GL_PROJECTION); glLoadIdentity();
 gluPerspective(viewer.fieldOfView,viewer.aspectRatio,viewer.zNear,viewer.zFar);
 glMatrixMode(GL_MODELVIEW); glLoadIdentity();
 gluLookAt(viewer.eyeX,viewer.eyeY,viewer.eyeZ, viewer.centerX,viewer.centerY,viewer.centerZ, viewer.upX,viewer.upY,viewer.upZ);
}

void myLightingInit() {
 static GLfloat ambient[] ={0,0,0,1}, diffuse[]={1,1,1,1}, specular[]={1,1,1,1}, specref[]={1,1,1,1}, position[]={10,10,10,1};
 short shininess=128;
 glDisable(GL_LIGHTING);
 glLightfv(GL_LIGHT0,GL_AMBIENT,ambient); glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
 glLightfv(GL_LIGHT0,GL_SPECULAR,specular); glLightfv(GL_LIGHT0,GL_POSITION,position);
 glEnable(GL_LIGHT0);
 glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE); glEnable(GL_COLOR_MATERIAL);
 glMaterialfv(GL_FRONT,GL_SPECULAR,specref); glMateriali(GL_FRONT,GL_SHININESS,shininess);
 glEnable(GL_NORMALIZE);
}

void myInit() {
 myDataInit();
 glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
 glutInitWindowPosition(window.posX,window.posY);
 glutInitWindowSize(window.width,window.height);
 glutCreateWindow(window.title.c_str());
 glutDisplayFunc(myDisplayFunc); glutReshapeFunc(myReshapeFunc);
 glutKeyboardFunc(myKeyboardFunc); glutSpecialFunc(mySpecialFunc);
 glutMotionFunc(myMotionFunc); glutMouseFunc(myMouseFunc);
 glPointSize(4);
 glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
 glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
 glFrontFace(GL_CCW); glShadeModel(GL_SMOOTH);
 glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
 glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
 glEnable(GL_CULL_FACE);
 myViewingInit();
 myLightingInit();
 myvirtualworld.init();
}

void myWelcome() {
 cout<<"*****************************************************************\n";
 cout<<"*                   TCG6223 Computer Graphics                   *\n";
 cout<<"*                  FIST, Multimedia University                  *\n";
 cout<<"*****************************************************************\n";
 cout<<"| Press:                                                        |\n";
 cout<<"|   <a>,<d>,<w>,<s>,<q>,<e> => move world                       |\n";
 cout<<"|   <arrows>                => rotate world                     |\n";
 cout<<"|   HOME                    => restore defaults                 |\n";
 cout<<"|   ESC                     => exit                             |\n";
 cout<<"|                                                               |\n";
 cout<<"|   F1                      => toggle shading / wire-frame mode |\n";
 cout<<"|   F2                      => toggle rendering of axes         |\n";
 cout<<"|   F3                      => toggle lighting on / off         |\n";
 cout<<"|                                                               |\n";
 cout<<"| Mouse (Left Drag or Right Drag) => rotate world               |\n";
 cout<<"|                                                               |\n";
 cout<<"*****************************************************************\n";
 cout<<"|                      H A V E   F U N  !!!                     |\n";
 cout<<"*****************************************************************\n";
}

int main(int argc, char **argv) {
 glutInit(&argc, argv);
 myWelcome();
 myInit();
 glutMainLoop();
}
