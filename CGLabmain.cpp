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
 viewer.aspectRatio=(height == 0) ? 1.0 : (GLdouble)width / (GLdouble)height;
 glViewport(0,0,width,height);
 myViewingInit();
}

void myKeyboardFunc(unsigned char key, int x, int y) {
 GLfloat xinc=0,yinc=0,zinc=0;
 bool cameraChanged = false;
 // Calculate distance between players for hit detection
 float dx = myvirtualworld.imX - myvirtualworld.caX;
 float dz = myvirtualworld.imZ - myvirtualworld.caZ;
 float distance = sqrt((dx * dx) + (dz * dz));
 switch (key) {
    // --- WORLD MOVEMENT (T, F, G, H, R, Y)
    case 'f': case 'F': xinc=-setting.posInc; break; // World Left
    case 'h': case 'H': xinc= setting.posInc; break; // World Right
    case 'r': case 'R': yinc=-setting.posInc; break; // World Down
    case 'y': case 'Y': yinc= setting.posInc; break; // World Up
    case 't': case 'T': zinc=-setting.posInc; break; // World Forward
    case 'g': case 'G': zinc= setting.posInc; break; // World Backward

    // Ironman Movement (W, A, S, D)
    case 'a': case 'A': myvirtualworld.imX -= setting.posInc; break; // Move Left
    case 'd': case 'D': myvirtualworld.imX += setting.posInc; break; // Move Right
    case 's': case 'S': myvirtualworld.imZ -= setting.posInc; break; // Move Forward
    case 'w': case 'W': myvirtualworld.imZ += setting.posInc; break; // Move Backward

    // Captain America Movement (I, J, K, L)
    case 'j': case 'J': myvirtualworld.caX -= setting.posInc; break; // Move Left
    case 'l': case 'L': myvirtualworld.caX += setting.posInc; break; // Move Right
    case 'i': case 'I': myvirtualworld.caZ -= setting.posInc; break; // Move Forward
    case 'k': case 'K': myvirtualworld.caZ += setting.posInc; break; // Move Backward

    // Captain America hand movement
    case '1': myvirtualworld.toggleCaptainHandAuto(); break;
    case '2': myvirtualworld.moveCaptainHands(-5.0f); break;
    case '3': myvirtualworld.moveCaptainHands( 5.0f); break;

    // Camera sight controls
    case 'b': case 'B': viewer.eyeX -= 2.0; cameraChanged = true; break;
    case 'v': case 'V': viewer.eyeX += 2.0; cameraChanged = true; break;
    case 'n': case 'N': viewer.eyeY -= 2.0; cameraChanged = true; break;
    case 'm': case 'M': viewer.eyeY += 2.0; cameraChanged = true; break;
    case 'z': case 'Z': viewer.eyeZ -= 2.0; cameraChanged = true; break;
    case 'x': case 'X': viewer.eyeZ += 2.0; cameraChanged = true; break;
    case ',': viewer.centerY -= 1.0; cameraChanged = true; break;
    case '.': viewer.centerY += 1.0; cameraChanged = true; break;
    case 'c': case 'C':
        viewer.eyeX = 0; viewer.eyeY = 20; viewer.eyeZ = 80;
        viewer.centerX = viewer.centerY = viewer.centerZ = 0;
        cameraChanged = true;
        break;

    // Ironman Skill 1
    case ' ': // Spacebar
        if (!myvirtualworld.imAttacking && myvirtualworld.imHP > 0) {
            myvirtualworld.imAttacking = true;
            myvirtualworld.imAttackTimer = 240;
            cout << "\n[Iron Man] Casts Repulsor Blast!" << endl;
