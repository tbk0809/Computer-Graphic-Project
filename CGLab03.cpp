/*
 TCG6223 Computer Graphics
 Combined CPP: Iron Man + Captain America
 (C++03 compatible)
*/

#include <GL/glut.h>
#include <string>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "CGLab03.hpp"

using namespace CGLab03;

// === COMBINED SCENE RENDER ===
void MyVirtualWorld::draw()
{
    glPushMatrix();
        glDisable(GL_TEXTURE_2D);

        GLfloat grassColor[] = {0.0f, 1.0f, 1.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grassColor);
        glColor3f(0.2f, 0.8f, 0.2f);

        glBegin(GL_QUADS);
            glNormal3f(0.0f, 1.0f, 0.0f);
            glVertex3f(-100.0f, -5.0f,  100.0f);
            glVertex3f( 100.0f, -5.0f,  100.0f);
            glVertex3f( 100.0f, -5.0f, -100.0f);
            glVertex3f(-100.0f, -5.0f, -100.0f);
        glEnd();
    glPopMatrix();

    // Iron Man - right side
    glPushMatrix();
        glTranslatef(10, -5, -15);
        ironman.draw();
    glPopMatrix();

    // Captain America - left side, facing Iron Man
    glPushMatrix();
        glTranslatef(-5, -5, 15);
        glRotatef(180, 0, 0, 1);
        glRotatef(180, 1, 0, 0);

        captainBody.draw();

        glPushMatrix();
            glTranslatef(-3.6f, 23.5f, -0.6f);
            glRotatef(captainHandAngle, 0, 0, 1);
            glTranslatef(3.6f, -23.5f, 0.6f);
            captainLeftHand.draw();
        glPopMatrix();

        glPushMatrix();
            glTranslatef(3.6f, 23.5f, -0.6f);
            glRotatef(-captainHandAngle, 0, 0, 1);
            glTranslatef(-3.6f, -23.5f, 0.6f);
            captainRightHand.draw();
        glPopMatrix();
    glPopMatrix();
}
