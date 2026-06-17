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
            if (grassTextureID) {
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, grassTextureID);
                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
                glColor3f(1.0f, 1.0f, 1.0f);
            } else {
                glColor3f(0.2f, 0.8f, 0.2f);
            }
            
            GLfloat grassColor[] = {0.8f, 0.8f, 0.8f, 1.0f};
            glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, grassColor);

            GLfloat xmin = -100.0f, xmax = 100.0f;
            GLfloat zmin = -100.0f, zmax = 100.0f;
            GLfloat xdim = xmax - xmin;
            GLfloat zdim = zmax - zmin;
            GLfloat texSMax = 20.0f;
            GLfloat texTMax = 20.0f;
            GLfloat quadSizeX = 5.0f;
            GLfloat quadSizeZ = 5.0f;
            GLfloat texScaleX = texSMax/xdim;
            GLfloat texScaleZ = texTMax/zdim;

            for (GLfloat z1 = zmin; z1 < zmax; z1 += quadSizeZ) {
                GLfloat z2 = z1 + quadSizeZ;
                GLfloat t1 = texScaleZ * (z1 - zmin);
                GLfloat t2 = texScaleZ * (z2 - zmin);
                for (GLfloat x1 = xmin; x1 < xmax; x1 += quadSizeX) {
                    GLfloat x2 = x1 + quadSizeX;
                    GLfloat s1 = texScaleX * (x1 - xmin);
                    GLfloat s2 = texScaleX * (x2 - xmin);

                    glBegin(GL_QUADS);
                    glNormal3f(0.0f, 1.0f, 0.0f);
                    if (grassTextureID) glTexCoord2f(s1, t1);
                    glVertex3f(x1, -5.0f, z1);
                    if (grassTextureID) glTexCoord2f(s1, t2);
                    glVertex3f(x1, -5.0f, z2);
                    if (grassTextureID) glTexCoord2f(s2, t2);
                    glVertex3f(x2, -5.0f, z2);
                    if (grassTextureID) glTexCoord2f(s2, t1);
                    glVertex3f(x2, -5.0f, z1);
                    glEnd();
                }
            }

            if (grassTextureID) glDisable(GL_TEXTURE_2D);
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

            // 【修改】减缓倒计时，延长钢铁侠动画时间
            imAttackTimer -= 0.5f;
            if (imAttackTimer <= 0) imAttacking = false;
        }

        // --- CUSTOM ORIGIN OFFSETS ---
        // The weapon files have their mesh centers far away from 0,0,0.
        // These local anchors let the props inherit Captain America's arm transforms.
        const float shieldCenterX =  9.1f,  shieldCenterY =  5.4f,  shieldCenterZ = -15.0f;
        const float hammerCenterX = -10.5f, hammerCenterY = 13.4f, hammerCenterZ = -5.0f;
        const float hammerGripX   = -10.5f, hammerGripY   = 10.8f, hammerGripZ   = 0.0f;

        const float shieldAttachX = -4.35f, shieldAttachY = 7.7f, shieldAttachZ = -0.95f;
        const float hammerAttachX =  -7.45f, hammerAttachY = 12.8f, hammerAttachZ = -0.75f;

        // 6a. Captain America Shield Flight Effect (right hand after the weapon swap)
        if (caAttackingShield) {
            if (caAttackTimer <= 180 && caAttackTimer > 40) {
                float startX = animCaX + 3.0f, startY = animCaY + 28.0f, startZ = animCaZ;
                float targetX = imX, targetY = imY + 28.0f, targetZ = imZ;
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
                    glPushMatrix();
                        glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
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
                glPopMatrix();
            }


            caAttackTimer -= 0.5f;
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
                    glPushMatrix();
                        glRotatef(caAttackTimer * 32.0f, 1.0f, 0.0f, 0.0f);
                        glRotatef(14.0f, 0.0f, 1.0f, 0.0f);
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
                glPopMatrix();

                if (caAttackingSuper && caAttackTimer <= 120 && caAttackTimer > 80) {
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Small per-frame jitter so the bolt crackles instead of staying static
    float jx = sin(timenew * 3.1f) * 0.6f;
    float jz = cos(timenew * 2.7f) * 0.6f;

    glLineWidth(3.0f);
    glColor4f(0.5f, 0.9f, 1.0f, 0.9f);
    glBegin(GL_LINES);
        // --- Main trunk (existing) ---
        glVertex3f(imX - 1.0f + jx, imY + 30.0f, imZ - 0.5f + jz); glVertex3f(imX + 2.3f, imY + 25.5f, imZ + 1.6f);
        glVertex3f(imX + 2.3f, imY + 25.5f, imZ + 1.6f); glVertex3f(imX - 2.8f, imY + 21.0f, imZ - 1.2f);
        glVertex3f(imX - 2.8f, imY + 21.0f, imZ - 1.2f); glVertex3f(imX + 1.6f, imY + 17.0f, imZ + 0.9f);
        glVertex3f(imX + 1.6f, imY + 17.0f, imZ + 0.9f); glVertex3f(imX - 1.9f, imY + 12.5f, imZ - 1.5f);
        glVertex3f(imX - 1.9f, imY + 12.5f, imZ - 1.5f); glVertex3f(imX + 0.2f, imY + 7.0f, imZ + 0.2f);

        // --- Existing mid forks ---
        glVertex3f(imX + 2.3f, imY + 25.5f, imZ + 1.6f); glVertex3f(imX + 5.0f, imY + 21.5f, imZ - 1.8f);
        glVertex3f(imX - 2.8f, imY + 21.0f, imZ - 1.2f); glVertex3f(imX - 5.2f, imY + 17.5f, imZ + 2.1f);
        glVertex3f(imX + 1.6f, imY + 17.0f, imZ + 0.9f); glVertex3f(imX + 4.4f, imY + 13.0f, imZ - 1.1f);

        // --- Existing large outer branches ---
        glVertex3f(imX + 3.0f, imY + 45.0f, imZ + 2.0f); glVertex3f(imX + 9.0f, imY + 35.0f, imZ - 3.0f);
        glVertex3f(imX + 9.0f, imY + 35.0f, imZ - 3.0f); glVertex3f(imX + 12.0f, imY + 22.0f, imZ - 1.0f);
        glVertex3f(imX + 12.0f, imY + 22.0f, imZ - 1.0f); glVertex3f(imX + 15.0f, imY + 8.0f, imZ + 2.0f);

        glVertex3f(imX - 3.5f, imY + 30.0f, imZ - 2.5f); glVertex3f(imX - 10.0f, imY + 22.0f, imZ + 4.0f);
        glVertex3f(imX - 10.0f, imY + 22.0f, imZ + 4.0f); glVertex3f(imX - 14.0f, imY + 10.0f, imZ + 2.0f);

        // --- Existing inner crackles ---
        glVertex3f(imX + 2.0f, imY + 18.0f, imZ + 1.5f); glVertex3f(imX + 7.0f, imY + 12.0f, imZ - 2.0f);
        glVertex3f(imX - 1.9f, imY + 10.0f, imZ - 1.5f); glVertex3f(imX - 6.0f, imY + 4.0f, imZ + 1.0f);

        // ===== NEW: tertiary mini-forks off the big outer branches =====
        glVertex3f(imX + 9.0f, imY + 35.0f, imZ - 3.0f); glVertex3f(imX + 13.5f, imY + 30.0f, imZ + 1.5f);
        glVertex3f(imX + 12.0f, imY + 22.0f, imZ - 1.0f); glVertex3f(imX + 17.0f, imY + 18.0f, imZ - 3.5f);
        glVertex3f(imX - 10.0f, imY + 22.0f, imZ + 4.0f); glVertex3f(imX - 15.0f, imY + 18.0f, imZ - 1.0f);
        glVertex3f(imX - 14.0f, imY + 10.0f, imZ + 2.0f); glVertex3f(imX - 17.5f, imY + 4.0f, imZ + 5.0f);

        // ===== NEW: extra branch shooting toward the camera/forward axis =====
        glVertex3f(imX + 0.5f, imY + 38.0f, imZ + 3.0f); glVertex3f(imX + 4.5f, imY + 28.0f, imZ + 8.0f);
        glVertex3f(imX + 4.5f, imY + 28.0f, imZ + 8.0f); glVertex3f(imX + 2.0f, imY + 16.0f, imZ + 6.5f);

        glVertex3f(imX - 0.8f, imY + 33.0f, imZ - 4.0f); glVertex3f(imX - 4.8f, imY + 24.0f, imZ - 9.0f);
        glVertex3f(imX - 4.8f, imY + 24.0f, imZ - 9.0f); glVertex3f(imX - 1.5f, imY + 14.0f, imZ - 6.0f);

        // ===== NEW: dense fine crackle near impact point (lower body) =====
        glVertex3f(imX + 0.2f, imY + 7.0f, imZ + 0.2f); glVertex3f(imX + 3.5f, imY + 4.5f, imZ - 1.5f);
        glVertex3f(imX + 0.2f, imY + 7.0f, imZ + 0.2f); glVertex3f(imX - 3.0f, imY + 3.5f, imZ + 2.0f);
        glVertex3f(imX - 1.9f, imY + 10.0f, imZ - 1.5f); glVertex3f(imX + 1.0f, imY + 6.0f, imZ - 4.0f);
        glVertex3f(imX + 4.4f, imY + 13.0f, imZ - 1.1f); glVertex3f(imX + 7.5f, imY + 9.0f, imZ + 1.0f);
    glEnd();

    if (caAttackTimer <= 104 && caAttackTimer > 96) {
        // Flickering brightness on the core flash for a punchier hit
        float flicker = 0.75f + (rand() % 25) / 100.0f;
        glColor4f(0.2f, 0.4f, 1.0f, flicker);
        glPushMatrix();
            glTranslatef(imX, imY + 7.0f, imZ);
            glutSolidSphere(6.0f, 24, 12);
        glPopMatrix();
        // Bright white-hot core inside the blue flash
        glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
        glPushMatrix();
            glTranslatef(imX, imY + 7.0f, imZ);
            glutSolidSphere(2.5f + (rand() % 10) / 10.0f, 16, 12);
        glPopMatrix();

        glLineWidth(6.0f);
        glColor4f(0.1f, 0.3f, 1.0f, 0.95f);
        glBegin(GL_LINES);
            // --- Existing 4-ray starburst ---
            glVertex3f(imX - 12.0f, imY + 7.0f, imZ); glVertex3f(imX + 12.0f, imY + 7.0f, imZ);
            glVertex3f(imX, imY - 3.0f, imZ); glVertex3f(imX, imY + 18.0f, imZ);
            glVertex3f(imX - 8.0f, imY + 1.0f, imZ + 5.0f); glVertex3f(imX + 8.0f, imY + 15.0f, imZ - 5.0f);
            glVertex3f(imX - 8.0f, imY + 15.0f, imZ - 5.0f); glVertex3f(imX + 8.0f, imY + 1.0f, imZ + 5.0f);

            // ===== NEW: extra rays to turn the 4-point star into an 8-point burst =====
            glVertex3f(imX - 10.0f, imY + 7.0f, imZ + 9.0f); glVertex3f(imX + 10.0f, imY + 7.0f, imZ - 9.0f);
            glVertex3f(imX - 10.0f, imY + 7.0f, imZ - 9.0f); glVertex3f(imX + 10.0f, imY + 7.0f, imZ + 9.0f);
            glVertex3f(imX - 4.0f, imY - 2.0f, imZ - 4.0f); glVertex3f(imX + 4.0f, imY + 16.0f, imZ + 4.0f);
            glVertex3f(imX + 4.0f, imY - 2.0f, imZ - 4.0f); glVertex3f(imX - 4.0f, imY + 16.0f, imZ + 4.0f);
        glEnd();

        // ===== NEW: thin ground ring scorch using a short line loop =====
        glLineWidth(2.5f);
        glColor4f(0.3f, 0.6f, 1.0f, 0.6f);
        glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 16; i++) {
                float ang = i * (360.0f / 16) * 3.14159265f / 180.0f;
                glVertex3f(imX + cos(ang) * 9.0f, imY + 7.1f, imZ + sin(ang) * 9.0f);
            }
        glEnd();
    }
    glLineWidth(1.0f);
    glDisable(GL_BLEND); glEnable(GL_LIGHTING);
}
            }


            caAttackTimer -= 0.25f;
            if (caAttackTimer <= 0) { caAttackingHammer = false; caAttackingSuper = false; }
        }


        // 6c. Captain America Super Activation Lightning (Calling down the Thunder)
        if (caAttackingSuper && caAttackTimer > 180) {
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    float jx = sin(timenew * 4.0f) * 0.4f;
    float jz = cos(timenew * 3.4f) * 0.4f;

    glLineWidth(4.0f);
    glColor4f(0.1f, 0.3f, 1.0f, 1.0f);
    glBegin(GL_LINES);
        // --- Existing main descending bolt ---
        glVertex3f(animCaX - 2.0f + jx, animCaY + 80.0f, animCaZ - 1.0f + jz); glVertex3f(animCaX + 3.0f, animCaY + 55.0f, animCaZ + 2.0f);
        glVertex3f(animCaX + 3.0f, animCaY + 55.0f, animCaZ + 2.0f); glVertex3f(animCaX - 3.5f, animCaY + 35.0f, animCaZ - 2.5f);
        glVertex3f(animCaX - 3.5f, animCaY + 35.0f, animCaZ - 2.5f); glVertex3f(animCaX - 1.0f, animCaY + 22.0f, animCaZ + 0.5f);

        // --- Existing secondary bolts ---
        glVertex3f(animCaX + 3.0f, animCaY + 55.0f, animCaZ + 2.0f); glVertex3f(animCaX + 10.0f, animCaY + 40.0f, animCaZ - 4.0f);
        glVertex3f(animCaX + 10.0f, animCaY + 40.0f, animCaZ - 4.0f); glVertex3f(animCaX + 5.0f, animCaY + 25.0f, animCaZ - 1.0f);

        glVertex3f(animCaX - 3.5f, animCaY + 35.0f, animCaZ - 2.5f); glVertex3f(animCaX - 12.0f, animCaY + 25.0f, animCaZ + 3.0f);
        glVertex3f(animCaX - 12.0f, animCaY + 25.0f, animCaZ + 3.0f); glVertex3f(animCaX - 8.0f, animCaY + 12.0f, animCaZ + 2.0f);

        // --- Existing swirl around body ---
        glVertex3f(animCaX - 6.0f, animCaY + 2.0f, animCaZ + 5.0f); glVertex3f(animCaX + 4.0f, animCaY + 8.0f, animCaZ - 3.0f);
        glVertex3f(animCaX + 7.0f, animCaY + 4.0f, animCaZ - 6.0f); glVertex3f(animCaX - 5.0f, animCaY + 14.0f, animCaZ + 4.0f);

        // ===== NEW: a second sky bolt striking from a different angle =====
        glVertex3f(animCaX + 6.0f, animCaY + 85.0f, animCaZ + 4.0f); glVertex3f(animCaX + 2.0f, animCaY + 60.0f, animCaZ - 1.0f);
        glVertex3f(animCaX + 2.0f, animCaY + 60.0f, animCaZ - 1.0f); glVertex3f(animCaX + 6.5f, animCaY + 38.0f, animCaZ + 3.0f);
        glVertex3f(animCaX + 6.5f, animCaY + 38.0f, animCaZ + 3.0f); glVertex3f(animCaX + 2.5f, animCaY + 20.0f, animCaZ - 0.5f);

        // ===== NEW: tertiary forks off the main trunk for density =====
        glVertex3f(animCaX + 3.0f, animCaY + 55.0f, animCaZ + 2.0f); glVertex3f(animCaX - 4.0f, animCaY + 48.0f, animCaZ + 6.0f);
        glVertex3f(animCaX - 3.5f, animCaY + 35.0f, animCaZ - 2.5f); glVertex3f(animCaX + 4.0f, animCaY + 28.0f, animCaZ - 6.0f);
        glVertex3f(animCaX + 10.0f, animCaY + 40.0f, animCaZ - 4.0f); glVertex3f(animCaX + 14.0f, animCaY + 30.0f, animCaZ + 1.0f);
        glVertex3f(animCaX - 12.0f, animCaY + 25.0f, animCaZ + 3.0f); glVertex3f(animCaX - 16.0f, animCaY + 15.0f, animCaZ - 1.0f);

        // ===== NEW: tighter, faster crackle right against his body =====
        glVertex3f(animCaX - 3.0f, animCaY + 6.0f, animCaZ - 4.0f); glVertex3f(animCaX + 2.0f, animCaY + 16.0f, animCaZ + 5.0f);
        glVertex3f(animCaX + 5.0f, animCaY + 10.0f, animCaZ + 2.0f); glVertex3f(animCaX - 4.0f, animCaY + 2.0f, animCaZ - 5.0f);
        glVertex3f(animCaX - 1.0f, animCaY + 18.0f, animCaZ - 3.0f); glVertex3f(animCaX + 6.0f, animCaY + 6.0f, animCaZ + 4.0f);
    glEnd();

    // Flashing ground aura while charging (existing, slightly denser)
    if ((int)caAttackTimer % 8 > 3) {
        glLineWidth(6.0f);
        glColor4f(0.2f, 0.4f, 1.0f, 0.85f);
        glBegin(GL_LINES);
            glVertex3f(animCaX - 14.0f, animCaY + 0.5f, animCaZ); glVertex3f(animCaX + 14.0f, animCaY + 0.5f, animCaZ);
            glVertex3f(animCaX, animCaY + 0.5f, animCaZ - 14.0f); glVertex3f(animCaX, animCaY + 0.5f, animCaZ + 14.0f);
            glVertex3f(animCaX - 10.0f, animCaY + 0.5f, animCaZ + 10.0f); glVertex3f(animCaX + 10.0f, animCaY + 0.5f, animCaZ - 10.0f);
            glVertex3f(animCaX - 10.0f, animCaY + 0.5f, animCaZ - 10.0f); glVertex3f(animCaX + 10.0f, animCaY + 0.5f, animCaZ + 10.0f);

            // ===== NEW: extra ground arcs for a denser aura ring =====
            glVertex3f(animCaX - 12.0f, animCaY + 0.5f, animCaZ + 6.0f); glVertex3f(animCaX + 12.0f, animCaY + 0.5f, animCaZ - 6.0f);
            glVertex3f(animCaX - 6.0f, animCaY + 0.5f, animCaZ + 12.0f); glVertex3f(animCaX + 6.0f, animCaY + 0.5f, animCaZ - 12.0f);
        glEnd();
    }

    glLineWidth(1.0f);
    glDisable(GL_BLEND); glEnable(GL_LIGHTING);
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
                    glTranslatef(-2.35f, 18.8f, -0.35f);
                    if (caAttackingHammer || caAttackingSuper) glRotatef(leftArmPitch, 1.0f, 0.0f, 0.0f);
                    else glRotatef(captainHandAngle, 0.0f, 0.0f, 1.0f);
                    glTranslatef(2.35f, -18.8f, 0.35f);

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
                    glTranslatef(2.35f, 18.8f, -0.35f);
                    if (caAttackingShield) glRotatef(rightArmPitch, 1.0f, 0.0f, 0.0f);
                    else glRotatef(-captainHandAngle, 0.0f, 0.0f, 1.0f);
                    glTranslatef(-2.35f, -18.8f, 0.35f);

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


