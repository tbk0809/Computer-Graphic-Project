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

        // Iron Man shake
        if (imAttacking && imAttackTimer <= 100 && imAttackTimer > 20) {
            float shakeX = sin(timenew * 0.8f) * 0.7f;
            float shakeY = cos(timenew * 0.9f) * 0.7f;
            glTranslatef(shakeX, shakeY, 0.0f);
        }
        // Captain America shake (Shield or Hammer)
        if ((caAttackingShield || caAttackingHammer || caAttackingSuper) && caAttackTimer <= 140 && caAttackTimer > 40) {
            float shakeX = sin(timenew * 1.5f) * 0.8f;
            float shakeY = cos(timenew * 1.6f) * 0.8f;
            glTranslatef(shakeX, shakeY, 0.0f);
        }

        // 2. Battle environment
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

        // 3. Iron Man animation
        float animImX = imX, animImY = imY, animImZ = imZ;
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

        // 4. Captain America body animation
        float animCaX = caX, animCaY = caY, animCaZ = caZ;
        float caTiltX = 0.0f, caRotY = 0.0f;

        bool caSkillActive = (caAttackingShield || caAttackingHammer || caAttackingSuper);

        if (caSkillActive) {
            if (caAttackingSuper && caAttackTimer > 180) {
                caRotY = -8.0f;
                caTiltX = 4.0f;
            } else if (caAttackTimer > 180) {
                float p = (240.0f - caAttackTimer) / 60.0f;
                caRotY = p * -18.0f;
                caTiltX = p * 6.0f;
            } else if (caAttackTimer > 40) {
                caRotY = -18.0f;
                caTiltX = -8.0f;
            } else {
                float p = caAttackTimer / 40.0f;
                caRotY = p * -18.0f;
                caTiltX = p * -8.0f;
            }
        }

        // 5. Iron Man world-space effect
        if (imAttacking) {
            glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            float coreX = animImX, coreY = animImY + 18.0f, coreZ = animImZ + 1.5f;
            float targetX = caX, targetY = caY + 16.0f, targetZ = caZ;

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
                    glTranslatef(coreX, coreY, coreZ);
                    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); glutSolidSphere(chargeSize * 0.4f, 16, 16);
                    glColor4f(0.0f, 0.8f, 1.0f, 0.6f); glutSolidSphere(chargeSize,        16, 16);
                glPopMatrix();
            } else if (imAttackTimer > 20) {
                float dx = targetX - coreX, dy = targetY - coreY, dz = targetZ - coreZ;
                float dist  = sqrt(dx*dx + dy*dy + dz*dz);
                float yaw   = atan2(dx, dz)  * 180.0f / 3.14159265f;
                float pitch = atan2(-dy, sqrt(dx*dx + dz*dz)) * 180.0f / 3.14159265f;

                float beamRadius = 1.5f;
                glPushMatrix();
                    glTranslatef(coreX, coreY, coreZ);
                    glRotatef(yaw, 0, 1, 0); glRotatef(pitch, 1, 0, 0);
                    GLUquadric* q = gluNewQuadric();
                    glColor4f(0.0f, 0.8f, 1.0f, 0.7f);
                    gluCylinder(q, beamRadius, beamRadius, dist, 16, 1);
                    gluDeleteQuadric(q);
                glPopMatrix();
            }
            glDisable(GL_BLEND); glEnable(GL_LIGHTING);
            imAttackTimer--;
            if (imAttackTimer <= 0) imAttacking = false;
        }

        // --- CUSTOM ORIGIN OFFSETS ---
        // The weapon files have their mesh centers far away from 0,0,0.
        // These local anchors let the props inherit Captain America's arm transforms.
        const float shieldCenterX =  9.1f,  shieldCenterY =  5.4f,  shieldCenterZ = -15.0f;
        const float hammerCenterX = -10.5f, hammerCenterY = 13.4f, hammerCenterZ = -15.0f;
        const float hammerGripX   = -10.5f, hammerGripY   = 10.8f, hammerGripZ   = 0.0f;

        const float shieldAttachX = -4.35f, shieldAttachY = 7.7f, shieldAttachZ = -0.95f;
        const float hammerAttachX =  -7.45f, hammerAttachY = 12.8f, hammerAttachZ = -0.75f;

        // 6a. Captain America Shield Flight Effect (right hand after the weapon swap)
        if (caAttackingShield) {
            if (caAttackTimer <= 180 && caAttackTimer > 40) {
                float startX = animCaX + 3.0f, startY = animCaY + 12.0f, startZ = animCaZ;
                float targetX = imX, targetY = imY + 18.0f, targetZ = imZ;
                float currentX = startX, currentY = startY, currentZ = startZ;

                if (caAttackTimer > 120) {
                    float p = (180.0f - caAttackTimer) / 60.0f;
                    currentX = startX + (targetX - startX) * p;
                    currentY = startY + (targetY - startY) * p + sin(p * 3.14159f) * 6.0f;
                    currentZ = startZ + (targetZ - startZ) * p;
                } else if (caAttackTimer > 100) {
                    currentX = targetX;
                    currentY = targetY;
                    currentZ = targetZ - 1.0f;
                } else {
                    float p = (100.0f - caAttackTimer) / 60.0f;
                    currentX = targetX + (startX - targetX) * p;
                    currentY = targetY + (startY - targetY) * p + sin(p * 3.14159f) * 4.0f;
                    currentZ = targetZ + (startZ - targetZ) * p;
                }

                glPushMatrix();
                    glTranslatef(currentX, currentY, currentZ);
                    glRotatef(caAttackTimer * 70.0f, 0.0f, 1.0f, 0.0f);
                    glRotatef(25.0f, 1.0f, 0.0f, 0.0f);
                    if (customWeaponsLoaded) {
                        glTranslatef(-shieldCenterX, -shieldCenterY, -shieldCenterZ);
                        customShield.draw();
                    } else {
                        glDisable(GL_TEXTURE_2D); glDisable(GL_LIGHTING);
                        glColor3f(0.8f, 0.1f, 0.1f);
                        glPushMatrix(); glScalef(2.5f, 2.5f, 0.2f); glutSolidSphere(1.0, 16, 16); glPopMatrix();
                        glEnable(GL_LIGHTING);
                    }
                glPopMatrix();
            }
            caAttackTimer--;
            if (caAttackTimer <= 0) caAttackingShield = false;
        }

        // 6b. Captain America Hammer Flight Effect (left hand after the weapon swap)
        if (caAttackingHammer || caAttackingSuper) {
            if (caAttackTimer <= 180 && caAttackTimer > 40) {
                float startX = animCaX - 3.0f, startY = animCaY + 12.0f, startZ = animCaZ;
                float targetX = imX, targetY = imY + 18.0f, targetZ = imZ;
                float currentX = startX, currentY = startY, currentZ = startZ;

                if (caAttackTimer > 120) {
                    float p = (180.0f - caAttackTimer) / 60.0f;
                    currentX = startX + (targetX - startX) * p;
                    currentY = startY + (targetY - startY) * p + sin(p * 3.14159f) * 7.0f;
                    currentZ = startZ + (targetZ - startZ) * p;
                } else if (caAttackTimer > 100) {
                    currentX = targetX;
                    currentY = targetY;
                    currentZ = targetZ - 1.0f;
                } else {
                    float p = (100.0f - caAttackTimer) / 60.0f;
                    currentX = targetX + (startX - targetX) * p;
                    currentY = targetY + (startY - targetY) * p + sin(p * 3.14159f) * 5.0f;
                    currentZ = targetZ + (startZ - targetZ) * p;
                }

                glPushMatrix();
                    glTranslatef(currentX, currentY, currentZ);
                    glRotatef(caAttackTimer * 75.0f, 1.0f, 0.0f, 0.0f);
                    glRotatef(25.0f, 0.0f, 1.0f, 0.0f);
                    if (customWeaponsLoaded) {
                        glTranslatef(-hammerCenterX, -hammerCenterY, -hammerCenterZ);
                        customHammer.draw();
                    } else {
                        glDisable(GL_TEXTURE_2D); glDisable(GL_LIGHTING);
                        glColor3f(0.7f, 0.7f, 0.75f);
                        glPushMatrix(); glScalef(1.8f, 1.2f, 1.2f); glutSolidCube(1.0f); glPopMatrix();
                        glEnable(GL_LIGHTING);
                    }
                glPopMatrix();

                if (caAttackingSuper && caAttackTimer <= 120 && caAttackTimer > 80) {
                    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
                    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                    glLineWidth(3.0f);
                    glColor4f(0.5f, 0.9f, 1.0f, 0.9f);
                    glBegin(GL_LINES);
                        glVertex3f(imX, imY + 28.0f, imZ); glVertex3f(imX - 2.0f, imY + 20.0f, imZ + 1.0f);
                        glVertex3f(imX - 2.0f, imY + 20.0f, imZ + 1.0f); glVertex3f(imX + 1.5f, imY + 13.0f, imZ - 1.0f);
                        glVertex3f(imX + 1.5f, imY + 13.0f, imZ - 1.0f); glVertex3f(imX, imY + 6.0f, imZ);
                        glVertex3f(imX + 3.0f, imY + 24.0f, imZ - 2.0f); glVertex3f(imX - 1.0f, imY + 16.0f, imZ + 2.0f);
                    glEnd();
                    glLineWidth(1.0f);
                    glDisable(GL_BLEND); glEnable(GL_LIGHTING);
                }
            }
            caAttackTimer--;
            if (caAttackTimer <= 0) { caAttackingHammer = false; caAttackingSuper = false; }
        }

        // 7. Draw Characters
        // --- Iron Man ---
        glPushMatrix();
            glTranslatef(animImX, animImY, animImZ);
            if (imAttacking) glRotatef(imTiltX, 1.0f, 0.0f, 0.0f);
            ironman.draw();
        glPopMatrix();

        // --- Captain America ---
        glPushMatrix();
            glTranslatef(animCaX, animCaY, animCaZ);
            glRotatef(180, 0, 0, 1);
            glRotatef(180, 1, 0, 0);

            if (caAttackingShield || caAttackingHammer || caAttackingSuper) {
                glRotatef(caTiltX, 1.0f, 0.0f, 0.0f);
                glRotatef(caRotY,  0.0f, 1.0f, 0.0f);
            }

            if (splitCaptainLoaded) {
                captainBody.draw();

                float leftArmPitch = 0.0f;
                float rightArmPitch = 0.0f;
                if (caAttackingHammer || caAttackingSuper) {
                    if (caAttackingSuper && caAttackTimer > 480) {
                        leftArmPitch = -90.0f * (540.0f - caAttackTimer) / 60.0f;
                    } else if (caAttackTimer > 180) {
                        leftArmPitch = -90.0f;
                    } else if (caAttackTimer > 40) {
                        leftArmPitch = -90.0f;
                    } else {
                        leftArmPitch = -90.0f * caAttackTimer / 40.0f;
                    }
                }
                if (caAttackingShield) {
                    if (caAttackTimer > 180) {
                        rightArmPitch = -90.0f * (240.0f - caAttackTimer) / 60.0f;
                    } else if (caAttackTimer > 40) {
                        rightArmPitch = -90.0f;
                    } else {
                        rightArmPitch = -90.0f * caAttackTimer / 40.0f;
                    }
                }

                // ==========================================
                // LEFT HAND (Holds Hammer after swap)
                // ==========================================
                glPushMatrix();
                    glTranslatef(-3.0f, 19.6f, -0.5f);
                    if (caAttackingHammer || caAttackingSuper) glRotatef(leftArmPitch, 1.0f, 0.0f, 0.0f);
                    else glRotatef(captainHandAngle, 0.0f, 0.0f, 1.0f);
                    glTranslatef(3.0f, -19.6f, 0.5f);

                    captainLeftHand.draw();

                    bool hammerFlying = ((caAttackingHammer || caAttackingSuper) && caAttackTimer <= 180 && caAttackTimer > 40);
                    if (customWeaponsLoaded && !hammerFlying) {
                        glPushMatrix();
                            glTranslatef(hammerAttachX, hammerAttachY, hammerAttachZ);
                            glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
                            glRotatef(4.0f, 0.0f, 0.0f, 1.0f);
                            glScalef(0.95f, 0.95f, 0.95f);
                            glTranslatef(-hammerGripX, -hammerGripY, -hammerGripZ);
                            customHammer.draw();
                        glPopMatrix();
                    }
                    if (caAttackingSuper && caAttackTimer > 180) {
                        glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
                        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                        glLineWidth(2.5f);
                        glColor4f(0.4f, 0.9f, 1.0f, 0.85f);
                        glBegin(GL_LINES);
                            glVertex3f(hammerAttachX, hammerAttachY + 7.0f, hammerAttachZ); glVertex3f(hammerAttachX - 1.2f, hammerAttachY + 3.5f, hammerAttachZ + 0.8f);
                            glVertex3f(hammerAttachX - 1.2f, hammerAttachY + 3.5f, hammerAttachZ + 0.8f); glVertex3f(hammerAttachX + 1.0f, hammerAttachY + 1.0f, hammerAttachZ - 0.5f);
                            glVertex3f(hammerAttachX + 1.7f, hammerAttachY + 5.5f, hammerAttachZ - 0.7f); glVertex3f(hammerAttachX - 0.7f, hammerAttachY + 2.0f, hammerAttachZ + 0.4f);
                        glEnd();
                        glLineWidth(1.0f);
                        glDisable(GL_BLEND); glEnable(GL_LIGHTING);
                    }
                glPopMatrix();

                // ==========================================
                // RIGHT HAND (Holds Shield after swap)
                // ==========================================
                glPushMatrix();
                    glTranslatef(3.0f, 19.6f, -0.5f);
                    if (caAttackingShield) glRotatef(rightArmPitch, 1.0f, 0.0f, 0.0f);
                    else glRotatef(-captainHandAngle, 0.0f, 0.0f, 1.0f);
                    glTranslatef(-3.0f, -19.6f, 0.5f);

                    captainRightHand.draw();

                    bool shieldFlying = (caAttackingShield && caAttackTimer <= 180 && caAttackTimer > 40);
                    if (customWeaponsLoaded && !shieldFlying) {
                        glPushMatrix();
                            glTranslatef(shieldAttachX, shieldAttachY, shieldAttachZ);
                            glRotatef(88.0f, 0.0f, 1.0f, 0.0f);
                            glRotatef(-45.0f, 9.0f, 0.0f, 1.0f);
                            glScalef(0.92f, 0.92f, 0.92f);
                            glTranslatef(-shieldCenterX, -shieldCenterY, -shieldCenterZ);
                            customShield.draw();
                        glPopMatrix();
                    }
                glPopMatrix();

            } else {
                captainamerica.draw();
            }
        glPopMatrix();

    glPopMatrix();
    drawHUD();
}
