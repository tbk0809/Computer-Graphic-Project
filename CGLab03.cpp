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

void MyVirtualWorld::draw()
{

    // 1. Camera shake effect
    glPushMatrix();

        // Iron Man Shake
        if (imAttacking && imAttackTimer <= 100 && imAttackTimer > 20) {
            float shakeX = sin(timenew * 0.8f) * 0.7f;
            float shakeY = cos(timenew * 0.9f) * 0.7f;
            glTranslatef(shakeX, shakeY, 0.0f);
        }
        // Captain America Shake
        if (caAttacking && caAttackTimer <= 140 && caAttackTimer > 40) {
            float shakeX = sin(timenew * 1.5f) * 0.8f;
            float shakeY = cos(timenew * 1.6f) * 0.8f;
            glTranslatef(shakeX, shakeY, 0.0f);
        }

        // 2. Battle environment
        glPushMatrix();
            glDisable(GL_TEXTURE_2D);

            GLfloat grassColor[] = {0.0f, 1.0f, 1.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grassColor);
            glColor3f(0.2f, 0.8f, 0.2f); // Classic Green

            glBegin(GL_QUADS);
                glNormal3f(0.0f, 1.0f, 0.0f);
                glVertex3f(-100.0f, -5.0f,  100.0f);
                glVertex3f( 100.0f, -5.0f,  100.0f);
                glVertex3f( 100.0f, -5.0f, -100.0f);
                glVertex3f(-100.0f, -5.0f, -100.0f);
            glEnd();
        glPopMatrix();

        // 3. Animation for iron man
        float animImX = imX; float animImY = imY; float animImZ = imZ;
        float imTiltX = 0.0f;

        if (imAttacking) {
            if (imAttackTimer > 200) {
                float p = (240.0f - imAttackTimer) / 40.0f;
                animImY += p * 15.0f; imTiltX = p * -10.0f;
            } else if (imAttackTimer > 100) {
                float p = (200.0f - imAttackTimer) / 100.0f;
                animImY += 15.0f + sin(imAttackTimer * 0.05f) * 0.5f;
                imTiltX = -10.0f + (p * 25.0f);
            } else if (imAttackTimer > 20) {
                animImY += 15.0f; imTiltX = -20.0f;
                float p = (100.0f - imAttackTimer) / 80.0f;
                animImZ += p * 4.0f;
            } else {
                float p = imAttackTimer / 20.0f;
                animImY += p * 15.0f; imTiltX = p * -20.0f; animImZ += p * 4.0f;
            }
        }

        // 4. Animation for Captain America
        float animCaX = caX; float animCaY = caY; float animCaZ = caZ;
        float caTiltX = 0.0f; float caRotY = 0.0f;

        if (caAttacking) {
            if (caAttackTimer > 180) {
                // Wind up
                float p = (240.0f - caAttackTimer) / 60.0f;
                caRotY = p * -30.0f;
                caTiltX = p * 10.0f;
                animCaZ -= p * 2.0f;
            } else if (caAttackTimer > 140) {
                // Throw
                float p = (180.0f - caAttackTimer) / 40.0f;
                caRotY = -30.0f;
                caTiltX = 10.0f - (p * 30.0f);
            } else if (caAttackTimer > 40) {
                // Hold Pose
                caRotY = -30.0f;
                caTiltX = -20.0f;
            } else {
                // Catch & Recoil
                float p = caAttackTimer / 40.0f;
                caRotY = p * -30.0f;
                caTiltX = (p * -20.0f) + (sin(p * 3.14159f) * 15.0f);
                animCaZ -= sin(p * 3.14159f) * 4.0f;
            }
        }

        // 5. Ironman World Space Effect
        if (imAttacking) {
            glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            float coreX = animImX; float coreY = animImY + 18.0f; float coreZ = animImZ + 1.5f;
            float targetX = caX; float targetY = caY + 16.0f; float targetZ = caZ;

            if (imAttackTimer > 200) {
                glPushMatrix();
                    glTranslatef(animImX, animImY + 2.0f, animImZ + 1.0f);
                    glColor4f(0.0f, 0.8f, 1.0f, 0.8f);
                    glutSolidSphere(2.0f + (rand()%10)/10.0f, 16, 16);
                glPopMatrix();
            } else if (imAttackTimer > 100) {
                float p = (200.0f - imAttackTimer) / 100.0f;
                float chargeSize = p * 3.0f;

                glPushMatrix();
                    glTranslatef(animImX, -4.8f, animImZ);
                    glRotatef(timenew * 0.2f, 0, 1, 0);
                    glColor4f(0.0f, 0.6f, 1.0f, 0.5f);
                    glutWireTorus(0.5, 8.0f + (p * 2.0f), 4, 32);
                glPopMatrix();

                glLineWidth(2.0f); glBegin(GL_LINES);
                for(int i = 0; i < 16; i++) {
                    float angle = (i * 22.5f) * 3.14159f / 180.0f;
                    float dist = 12.0f * (1.0f - p) + 2.0f;
                    float px = cos(angle + timenew * 0.005f) * dist;
                    float py = sin(angle + timenew * 0.005f) * dist;
                    glColor4f(0.0f, 0.8f, 1.0f, 0.0f); glVertex3f(coreX + px*1.5, coreY + py*1.5, coreZ - dist);
                    glColor4f(1.0f, 1.0f, 1.0f, 0.9f); glVertex3f(coreX + px, coreY + py, coreZ - dist*0.5f);
                }
                glEnd(); glLineWidth(1.0f);

                glPushMatrix();
                    glTranslatef(coreX, coreY, coreZ);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); glutSolidSphere(chargeSize * 0.4f, 16, 16);
                    glColor4f(0.0f, 0.8f, 1.0f, 0.6f); glutSolidSphere(chargeSize, 16, 16);
                    glRotatef(imAttackTimer * 25.0f, 0, 1, 0); glRotatef(imAttackTimer * 15.0f, 1, 0, 0);
                    glColor4f(0.0f, 0.5f, 1.0f, 0.8f); glutWireSphere(chargeSize * 1.2f, 10, 10);
                glPopMatrix();

            } else if (imAttackTimer > 20) {
                float dx = targetX - coreX; float dy = targetY - coreY; float dz = targetZ - coreZ;
                float dist = sqrt(dx*dx + dy*dy + dz*dz);
                float yaw = atan2(dx, dz) * 180.0f / 3.14159265f;
                float pitch = atan2(-dy, sqrt(dx*dx + dz*dz)) * 180.0f / 3.14159265f;

                float beamPulse = (sin(timenew * 0.05f) * 0.5f) + 0.5f;
                float beamRadius = 1.5f + (beamPulse * 0.6f);

                glPushMatrix();
                    glTranslatef(coreX, coreY, coreZ);
                    glRotatef(yaw, 0, 1, 0); glRotatef(pitch, 1, 0, 0);
                    GLUquadric* q = gluNewQuadric();
                    glColor4f(0.0f, 0.3f, 1.0f, 0.3f); gluCylinder(q, beamRadius * 2.5f, beamRadius * 2.0f, dist, 16, 1);
                    glColor4f(0.0f, 0.8f, 1.0f, 0.7f); gluCylinder(q, beamRadius, beamRadius, dist, 16, 1);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); gluCylinder(q, beamRadius * 0.3f, beamRadius * 0.3f, dist, 16, 1);

                    float travelSpeed = fmod((240.0f - imAttackTimer) * 2.0f, dist);
                    glPushMatrix();
                        glTranslatef(0.0f, 0.0f, travelSpeed);
                        glColor4f(0.5f, 0.9f, 1.0f, 0.9f); glutSolidTorus(0.3f, beamRadius * 1.5f, 8, 16);
                    glPopMatrix();
                    gluDeleteQuadric(q);
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(coreX, coreY, coreZ);
                    glColor4f(0.8f, 0.9f, 1.0f, 0.9f); glutSolidSphere(beamRadius * 2.0f, 16, 16);
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(targetX, targetY, targetZ);
                    float impactSize = 3.0f + beamPulse * 2.0f;
                    glColor4f(0.0f, 0.6f, 1.0f, 0.8f); glutSolidSphere(impactSize, 16, 16);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); glutSolidSphere(impactSize * 0.5f, 16, 16);

                    glLineWidth(4.0f); glBegin(GL_LINES);
                    for(int i = 0; i < 8; i++) {
                        float angle = (i * 45.0f + imAttackTimer * 15.0f) * 3.14159f / 180.0f;
                        float sparkX = cos(angle) * (impactSize * 2.0f);
                        float sparkY = sin(angle) * (impactSize * 2.0f);
                        float sparkZ = sin(angle * 2.0f) * (impactSize * 1.5f);
                        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); glVertex3f(0.0f, 0.0f, 0.0f);
                        glColor4f(0.0f, 0.8f, 1.0f, 0.0f); glVertex3f(sparkX, sparkY, sparkZ);
                    }
                    glEnd(); glLineWidth(1.0f);
                glPopMatrix();
            }

            glDisable(GL_BLEND); glEnable(GL_LIGHTING);
            imAttackTimer--;
            if (imAttackTimer <= 0) imAttacking = false;
        }

        // 6. Captain America world space effect
        if (caAttacking) {
            float startX = animCaX - 2.0f; float startY = animCaY + 12.0f; float startZ = animCaZ;
            float targetX = imX; float targetY = imY + 18.0f; float targetZ = imZ;

            float currentX = startX; float currentY = startY; float currentZ = startZ;
            float arcOffset = 0.0f;

            if (caAttackTimer > 180) {
                // Wind up
                float glow = (sin(timenew * 0.05f) + 1.0f) * 0.5f;
                glPushMatrix();
                    glTranslatef(currentX, currentY, currentZ);
                    glDisable(GL_LIGHTING); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    glColor4f(0.0f, 0.5f, 1.0f, 0.3f + glow * 0.5f);
                    glutSolidSphere(4.0f, 16, 16);
                    glDisable(GL_BLEND); glEnable(GL_LIGHTING);
                glPopMatrix();

            } else if (caAttackTimer > 140) {
                // Throw
                float p = (180.0f - caAttackTimer) / 40.0f;
                arcOffset = sin(p * 3.14159f) * 12.0f;
                currentX = startX + (targetX - startX) * p + arcOffset;
                currentY = startY + (targetY - startY) * p + (arcOffset * 0.3f);
                currentZ = startZ + (targetZ - startZ) * p;

            } else if (caAttackTimer > 40) {
                // Continuous repeating shockwaves
                currentX = targetX; currentY = targetY; currentZ = targetZ - 1.0f;

                glPushMatrix();
                    glTranslatef(currentX, currentY, currentZ);
                    glDisable(GL_LIGHTING); glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

                    // Wave 1
                    float wave1Size = fmod((140.0f - caAttackTimer) * 1.2f, 40.0f);
                    glColor4f(0.2f, 0.6f, 1.0f, 1.0f - (wave1Size / 40.0f));
                    glutSolidTorus(0.5f, wave1Size, 16, 32);

                    // Wave 2
                    float wave2Size = fmod((140.0f - caAttackTimer) * 1.2f + 20.0f, 40.0f);
                    glColor4f(0.2f, 0.6f, 1.0f, 1.0f - (wave2Size / 40.0f));
                    glutSolidTorus(0.5f, wave2Size, 16, 32);

                    glColor4f(1.0f, 1.0f, 1.0f, 0.8f);
                    glutSolidSphere(4.0f, 16, 16);

                    glDisable(GL_BLEND); glEnable(GL_LIGHTING);
                glPopMatrix();

            } else {
                // Return
                float p = caAttackTimer / 40.0f;
                arcOffset = sin(p * 3.14159f) * 12.0f;
                currentX = startX + (targetX - startX) * p - arcOffset;
                currentY = startY + (targetY - startY) * p + (arcOffset * 0.3f);
                currentZ = startZ + (targetZ - startZ) * p;
            }

            // Draw the flying
            glPushMatrix();
                glDisable(GL_TEXTURE_2D); glDisable(GL_LIGHTING);
                glTranslatef(currentX, currentY, currentZ);

                // Spin extremely fast when thrown
                if (caAttackTimer <= 180 && caAttackTimer > 40) glRotatef(caAttackTimer * 65.0f, 0.0f, 1.0f, 0.0f);
                else glRotatef(-30.0f, 0.0f, 1.0f, 0.0f);

                glRotatef(25.0f, 1.0f, 0.0f, 0.0f);

                glColor3f(0.8f, 0.1f, 0.1f);
                glPushMatrix(); glScalef(2.5f, 2.5f, 0.2f); glutSolidSphere(1.0, 16, 16); glPopMatrix();
                glColor3f(0.9f, 0.9f, 0.9f);
                glPushMatrix(); glScalef(1.8f, 1.8f, 0.25f); glutSolidSphere(1.0, 16, 16); glPopMatrix();
                glColor3f(0.1f, 0.3f, 0.8f);
                glPushMatrix(); glScalef(1.0f, 1.0f, 0.3f); glutSolidSphere(1.0, 16, 16); glPopMatrix();

                if (caAttackTimer <= 180 && caAttackTimer > 40) {
                    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    glColor4f(0.0f, 0.5f, 1.0f, 0.6f);
                    glutSolidTorus(0.3f, 2.7f, 16, 32);
                    glDisable(GL_BLEND);
                }

                glEnable(GL_LIGHTING);
            glPopMatrix();

            caAttackTimer--;
            if (caAttackTimer <= 0) caAttacking = false;
        }

        // 7. Draw Characters
        // Iron Man
        glPushMatrix();
            glTranslatef(animImX, animImY, animImZ);
            if (imAttacking) glRotatef(imTiltX, 1.0f, 0.0f, 0.0f);
            ironman.draw();
        glPopMatrix();

        // Captain America
        glPushMatrix();
            glTranslatef(animCaX, animCaY, animCaZ);
            glRotatef(180, 0, 0, 1);
            glRotatef(180, 1, 0, 0);

            if (caAttacking) {
                glRotatef(caTiltX, 1.0f, 0.0f, 0.0f);
                glRotatef(caRotY, 0.0f, 1.0f, 0.0f);
            }

            if (splitCaptainLoaded) {
                captainBody.draw();

                glPushMatrix();
                    glTranslatef(-3.0f, 19.6f, -0.5f);
                    glRotatef(captainHandAngle, 0.0f, 0.0f, 1.0f);
                    glTranslatef(3.0f, -19.6f, 0.5f);
                    captainLeftHand.draw();
                glPopMatrix();

                glPushMatrix();
                    glTranslatef(3.0f, 19.6f, -0.5f);
                    glRotatef(-captainHandAngle, 0.0f, 0.0f, 1.0f);
                    glTranslatef(-3.0f, -19.6f, 0.5f);
                    captainRightHand.draw();
                glPopMatrix();
            } else {
                captainamerica.draw();
            }
        glPopMatrix();

    glPopMatrix();

    drawHUD();
}
