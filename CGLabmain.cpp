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

 float dx = myvirtualworld.imX - myvirtualworld.caX;
 float dz = myvirtualworld.imZ - myvirtualworld.caZ;
 float distance = sqrt((dx * dx) + (dz * dz));

 switch (key) {
    case 'f': case 'F': xinc=-setting.posInc; break;
    case 'h': /* See fallback below */ break;
    case 'r': case 'R': yinc=-setting.posInc; break;
    case 'y': case 'Y': yinc= setting.posInc; break;
    case 't': case 'T': zinc=-setting.posInc; break;
    case 'g': case 'G': zinc= setting.posInc; break;

    case 'a': case 'A': myvirtualworld.imX -= setting.posInc; break;
    case 'd': case 'D': myvirtualworld.imX += setting.posInc; break;
    case 's': myvirtualworld.imZ -= setting.posInc; break;
    case 'S':
        if (!myvirtualworld.caAttackingShield && !myvirtualworld.caAttackingHammer && !myvirtualworld.caAttackingSuper && myvirtualworld.caHP > 0) {
            myvirtualworld.caAttackingSuper = true;
            myvirtualworld.caAttackTimer = 540;
            cout << "\n[Captain America] Casts Thunder Hammer Super!" << endl;
            if (distance < 18.0f) {
                myvirtualworld.imHP -= 30;
                cout << "-> THUNDER HIT! Iron Man HP: " << myvirtualworld.imHP << "/100" << endl;
                if (myvirtualworld.imHP <= 0) cout << "*** CAPTAIN AMERICA WINS! ***" << endl;
            } else { cout << "-> MISS! (Too far away)" << endl; }
        }
        break;
    case 'w': case 'W': myvirtualworld.imZ += setting.posInc; break;

    case 'j': case 'J': myvirtualworld.caX -= setting.posInc; break;
    case 'l': case 'L': myvirtualworld.caX += setting.posInc; break;
    case 'i': case 'I': myvirtualworld.caZ -= setting.posInc; break;
    case 'k': case 'K': myvirtualworld.caZ += setting.posInc; break;

    case '1': myvirtualworld.toggleCaptainHandAuto(); break;
    case '2': myvirtualworld.moveCaptainHands(-5.0f); break;
    case '3': myvirtualworld.moveCaptainHands( 5.0f); break;

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

    case ' ':
        if (!myvirtualworld.imAttacking && myvirtualworld.imHP > 0) {
            myvirtualworld.imAttacking   = true;
            myvirtualworld.imAttackTimer = 240;
            cout << "\n[Iron Man] Casts Repulsor Blast!" << endl;
            if (distance < 15.0f) {
                myvirtualworld.caHP -= 15;
                cout << "-> HIT! Captain America HP: " << myvirtualworld.caHP << "/100" << endl;
                if (myvirtualworld.caHP <= 0) cout << "*** IRON MAN WINS! ***" << endl;
            } else { cout << "-> MISS! (Too far away)" << endl; }
        }
        break;

    case 13: // Enter
        if (!myvirtualworld.caAttackingShield && !myvirtualworld.caAttackingHammer && !myvirtualworld.caAttackingSuper && myvirtualworld.caHP > 0) {
            myvirtualworld.caAttackingShield = true;
            myvirtualworld.caAttackTimer = 240;
            cout << "\n[Captain America] Casts Shield Strike!" << endl;
            if (distance < 15.0f) {
                myvirtualworld.imHP -= 15;
                cout << "-> HIT! Iron Man HP: " << myvirtualworld.imHP << "/100" << endl;
                if (myvirtualworld.imHP <= 0) cout << "*** CAPTAIN AMERICA WINS! ***" << endl;
            } else { cout << "-> MISS! (Too far away)" << endl; }
        }
        break;

    case 'H': // We handle the lowercase 'h' below, and capital 'H' here (Shift + h).
              // C++ Standard GLUT doesn't detect the 'Shift' key standalone as an event block!
        if (!myvirtualworld.caAttackingShield && !myvirtualworld.caAttackingHammer && !myvirtualworld.caAttackingSuper && myvirtualworld.caHP > 0) {
            myvirtualworld.caAttackingHammer = true;
            myvirtualworld.caAttackTimer = 240;
            cout << "\n[Captain America] Casts Hammer Strike!" << endl;
            if (distance < 15.0f) {
                myvirtualworld.imHP -= 15;
                cout << "-> HIT! Iron Man HP: " << myvirtualworld.imHP << "/100" << endl;
                if (myvirtualworld.imHP <= 0) cout << "*** CAPTAIN AMERICA WINS! ***" << endl;
            } else { cout << "-> MISS! (Too far away)" << endl; }
        }
        break;

    case 27: exit(1); break;
 }

 // Explicit check to catch lowercase 'h' since we couldn't hook it properly above because of world movement.
 if ((key == 'h' || key == 'H') && !myvirtualworld.caAttackingShield && !myvirtualworld.caAttackingHammer && !myvirtualworld.caAttackingSuper && myvirtualworld.caHP > 0) {
     myvirtualworld.caAttackingHammer = true;
     myvirtualworld.caAttackTimer = 240;
     cout << "\n[Captain America] Casts Hammer Strike!" << endl;
     if (distance < 15.0f) {
         myvirtualworld.imHP -= 15;
         cout << "-> HIT! Iron Man HP: " << myvirtualworld.imHP << "/100" << endl;
         if (myvirtualworld.imHP <= 0) cout << "*** CAPTAIN AMERICA WINS! ***" << endl;
     } else { cout << "-> MISS! (Too far away)" << endl; }
 } else if (key == 'h') {
     xinc = setting.posInc; // Fallback mapping for world move if HP dead or busy
 }

 world.move(xinc, yinc, zinc);
 if (cameraChanged) myViewingInit();
 glutPostRedisplay();
}

void mySpecialFunc(int key, int x, int y) {
 switch (key) {
    case GLUT_KEY_DOWN : world.rotateX -= setting.angleInc; break;
    case GLUT_KEY_UP   : world.rotateX += setting.angleInc; break;
    case GLUT_KEY_LEFT : world.rotateY -= setting.angleInc; break;
    case GLUT_KEY_RIGHT: world.rotateY += setting.angleInc; break;
    case GLUT_KEY_HOME : myDataInit(); break;
    case GLUT_KEY_F1:
        setting.shadingMode = !setting.shadingMode;
        glPolygonMode(GL_FRONT_AND_BACK, setting.shadingMode ? GL_FILL : GL_LINE);
        break;
    case GLUT_KEY_F2: worldaxis.toggle(); break;
    case GLUT_KEY_F3: {
        GLboolean on; glGetBooleanv(GL_LIGHTING, &on);
        if (on == GL_TRUE) glDisable(GL_LIGHTING);
        else               glEnable(GL_LIGHTING);
        break;
    }
    case GLUT_KEY_PAGE_UP:
        viewer.fieldOfView -= 2.0;
        if (viewer.fieldOfView < 20.0) viewer.fieldOfView = 20.0;
        myViewingInit();
        break;
    case GLUT_KEY_PAGE_DOWN:
        viewer.fieldOfView += 2.0;
        if (viewer.fieldOfView > 100.0) viewer.fieldOfView = 100.0;
        myViewingInit();
        break;
 }
 glutPostRedisplay();
}

void myMouseFunc(int button, int state, int x, int y) {
 y = window.height - y;
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
 y = window.height - y;
 GLint xinc = x - setting.mouseX, yinc = y - setting.mouseY;
 if (setting.mouseRightMode) world.rotate(0, 0, -xinc*0.5f);
 if (setting.mouseLeftMode)  world.rotate(-yinc*0.5f, xinc*0.5f, 0);
 setting.mouseX = x; setting.mouseY = y;
 glutPostRedisplay();
}

void myDataInit() {
 window.title="TCG6223 Computer Graphics";
 window.posX=100; window.posY=100; window.width=800; window.height=500;
 world.rotateX=world.rotateY=world.rotateZ=0;
 world.posX=world.posY=world.posZ=0;
 world.scaleX=world.scaleY=world.scaleZ=1;

 viewer.eyeX=0; viewer.eyeY=20; viewer.eyeZ=80;
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
 gluPerspective(viewer.fieldOfView, viewer.aspectRatio, viewer.zNear, viewer.zFar);
 glMatrixMode(GL_MODELVIEW); glLoadIdentity();
 gluLookAt(viewer.eyeX, viewer.eyeY, viewer.eyeZ,
           viewer.centerX, viewer.centerY, viewer.centerZ,
           viewer.upX, viewer.upY, viewer.upZ);
}

void myLightingInit() {
 static GLfloat ambient[]  = {0,0,0,1}, diffuse[]  = {1,1,1,1};
 static GLfloat specular[] = {1,1,1,1}, specref[]  = {1,1,1,1};
 static GLfloat position[] = {10,10,10,1};
 short shininess = 128;
 glDisable(GL_LIGHTING);
 glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
 glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
 glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
 glLightfv(GL_LIGHT0, GL_POSITION, position);
 glEnable(GL_LIGHT0);
 glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
 glEnable(GL_COLOR_MATERIAL);
 glMaterialfv(GL_FRONT, GL_SPECULAR,  specref);
 glMateriali (GL_FRONT, GL_SHININESS, shininess);
 glEnable(GL_NORMALIZE);
}

void myInit() {
 myDataInit();
 glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
 glutInitWindowPosition(window.posX, window.posY);
 glutInitWindowSize(window.width, window.height);
 glutCreateWindow(window.title.c_str());
 glutDisplayFunc(myDisplayFunc);
 glutReshapeFunc(myReshapeFunc);
 glutKeyboardFunc(myKeyboardFunc);
 glutSpecialFunc(mySpecialFunc);
 glutMotionFunc(myMotionFunc);
 glutMouseFunc(myMouseFunc);
 glPointSize(4);
 glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
 glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
 glFrontFace(GL_CCW); glShadeModel(GL_SMOOTH);
 glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
 glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
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
 cout<<"| Characters:                                                   |\n";
 cout<<"|   Iron Man    -> W/A/S/D to move, SPACE to attack            |\n";
 cout<<"|   Captain Am. -> I/J/K/L to move, ENTER for Shield Attack    |\n";
 cout<<"|                  H to throw Hammer, S for Hammer Super        |\n";
 cout<<"|                                                               |\n";
 cout<<"| World:                                                        |\n";
 cout<<"|   T/G/F/H/R/Y             => move world                      |\n";
 cout<<"|   Arrow keys              => rotate world                     |\n";
 cout<<"|   HOME                    => restore defaults                 |\n";
 cout<<"|   ESC                     => exit                             |\n";
 cout<<"|                                                               |\n";
 cout<<"| Captain America Hands (split model only):                     |\n";
 cout<<"|   1  => toggle auto-swing on/off (swings during attack only)  |\n";
 cout<<"|   2  => manual arm angle -5 deg (when not attacking)          |\n";
 cout<<"|   3  => manual arm angle +5 deg (when not attacking)          |\n";
 cout<<"|                                                               |\n";
 cout<<"| Camera:                                                       |\n";
 cout<<"|   B/V => camera left/right  N/M => camera down/up            |\n";
 cout<<"|   Z/X => zoom in/out        C   => reset camera               |\n";
 cout<<"|   ,/. => look down/up   PgUp/Dn => FOV narrow/wide           |\n";
 cout<<"|                                                               |\n";
 cout<<"| Toggles:                                                      |\n";
 cout<<"|   F1 => wireframe  F2 => axes  F3 => lighting                |\n";
 cout<<"|   Mouse drag (L/R) => rotate world                            |\n";
 cout<<"*****************************************************************\n";
 cout<<"|                      H A V E   F U N  !!!                    |\n";
 cout<<"*****************************************************************\n";
}

int main(int argc, char **argv) {
 glutInit(&argc, argv);
 myWelcome();
 myInit();
 glutMainLoop();
}
