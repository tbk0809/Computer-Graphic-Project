/*
 TCG6223 Computer Graphics
 Combined CPP: Iron Man + Captain America
 (C++03 compatible)
*/

#include <GL/glut.h>
#include <string>
#include <fstream>
#include <iostream>
#include <cmath>
#define STB_IMAGE_IMPLEMENTATION
#include "CGLab03.hpp"

using namespace std;
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

        // --- ADD THIS NEW SHAKE FOR MISSILES ---
        if (imAttackingMissiles && imAttackTimer <= 60 && imAttackTimer > 0) {
            float shakeX = sin(timenew * 1.5f) * 1.0f;
            float shakeY = cos(timenew * 1.6f) * 1.0f;
            glTranslatef(shakeX, shakeY, 0.0f);
        }

        // Captain America shake (Shield or Hammer)
        if ((caDefendingAnim || caAttackingHammer || caAttackingSuper) && caAttackTimer <= 140 && caAttackTimer > 40) {
            float shakeX = sin(timenew * 1.5f) * 0.8f;
            float shakeY = cos(timenew * 1.6f) * 0.8f;
            glTranslatef(shakeX, shakeY, 0.0f);
        }

        // 2. Battle environment (Grass Arena)
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

        if (imAttacking || imAttackingMissiles) {
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

        bool caSkillActive = (caDefendingAnim || caAttackingHammer || caAttackingSuper);

        if (caSkillActive) {
            if (caDefendingAnim) {
                // Smoothly lean into the block over the first 20 frames
                if (caAttackTimer > 40) {
                    float p = (60.0f - caAttackTimer) / 20.0f;
                    caRotY = p * 25.0f;   // Turn side-on
                    caTiltX = p * -10.0f; // Lean forward
                } else {
                    caRotY = 25.0f;
                    caTiltX = -10.0f;
                }
            } else if (caAttackingSuper && caAttackTimer > 180) {
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

        // 5a. Iron Man world-space effect
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

            imAttackTimer -= 0.5f;

            // ==========================================
            // DELAYED HP DEDUCTION
            // Deducts exactly at the frame the laser hits
            // ==========================================
            if (imAttackTimer == 100.0f) {
                if (caIsDefending) {
                    cout << "-> BLOCKED! Captain America takes NO damage! HP: " << caHP << "/100" << endl;
                    caIsDefending = false;
                    caDefendingAnim = false; // NEW: Forces his arm to drop back down!
                } else {
                    caHP -= 15;
                    cout << "-> LASER HIT! Captain America HP: " << caHP << "/100" << endl;
                }
                if (caHP <= 0) cout << "*** IRON MAN WINS! ***" << endl;
            }

            if (imAttackTimer <= 0) { imAttacking = false; if (caHP > 0) currentTurn = 2; }
        }

        // ==============================================================
        // 5b. Iron Man world-space effect (Homing Micro-Missiles)
        // ==============================================================
        if (imAttackingMissiles) {
            glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            float coreX = animImX, coreY = animImY + 16.0f, coreZ = animImZ;
            float targetX = caX, targetY = caY + 8.0f, targetZ = caZ;

            if (imAttackTimer > 160) {
                // PHASE 1: Missiles deploy and orbit around Iron Man
                float deployP = (240.0f - imAttackTimer) / 80.0f;
                for(int i=0; i<8; i++) {
                    float angle = i * 45.0f * 3.14159f / 180.0f + (timenew * 0.005f);
                    float radius = deployP * 8.0f;
                    float mx = coreX + cos(angle) * radius;
                    float mz = coreZ + sin(angle) * radius;
                    float my = coreY + sin(timenew * 0.01f + i) * 2.0f;

                    glPushMatrix();
                        glTranslatef(mx, my, mz);
                        // Point the rockets UP while they orbit and charge
                        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
                        glRotatef(timenew * 0.5f, 0.0f, 0.0f, 1.0f); // Spin them slightly

                        GLUquadric* q = gluNewQuadric();

                        // 1. Rocket Body (Metallic)
                        glColor4f(0.7f, 0.7f, 0.7f, deployP);
                        gluCylinder(q, 0.25f, 0.25f, 1.2f, 8, 1);

                        // 2. Rocket Nose Cone (Red)
                        glPushMatrix();
                            glTranslatef(0.0f, 0.0f, 1.2f);
                            glColor4f(0.8f, 0.1f, 0.1f, deployP);
                            glutSolidCone(0.25f, 0.6f, 8, 1);
                        glPopMatrix();

                        // 3. Rocket Fins (Red)
                        glDisable(GL_CULL_FACE); // Turn off culling so fins are visible from both sides
                        glColor4f(0.8f, 0.1f, 0.1f, deployP);
                        glBegin(GL_TRIANGLES);
                            glVertex3f( 0.0f,  0.25f, 0.2f); glVertex3f( 0.0f,  0.25f, 0.8f); glVertex3f( 0.0f,  0.7f, 0.0f); // Top
                            glVertex3f( 0.0f, -0.25f, 0.2f); glVertex3f( 0.0f, -0.25f, 0.8f); glVertex3f( 0.0f, -0.7f, 0.0f); // Bottom
                            glVertex3f(-0.25f,  0.0f, 0.2f); glVertex3f(-0.25f,  0.0f, 0.8f); glVertex3f(-0.7f,  0.0f, 0.0f); // Left
                            glVertex3f( 0.25f,  0.0f, 0.2f); glVertex3f( 0.25f,  0.0f, 0.8f); glVertex3f( 0.7f,  0.0f, 0.0f); // Right
                        glEnd();
                        glEnable(GL_CULL_FACE);

                        // Engine glow charging
                        glColor4f(1.0f, 0.5f, 0.0f, deployP);
                        glutSolidSphere(0.3f, 8, 8);

                        gluDeleteQuadric(q);
                    glPopMatrix();
                }
            } else if (imAttackTimer > 60) {
                // PHASE 2: Missiles fly in an arc toward Captain America
                float flightP = (160.0f - imAttackTimer) / 100.0f;

                for(int i=0; i<8; i++) {
                    float startAngle = i * 45.0f * 3.14159f / 180.0f;
                    float arcSpread = sin(flightP * 3.14159f);
                    float spreadX = cos(startAngle + timenew*0.01f) * 15.0f * arcSpread;
                    float spreadZ = sin(startAngle + timenew*0.01f) * 15.0f * arcSpread;
                    float arcY = arcSpread * 20.0f;

                    float mx = coreX + (targetX - coreX) * flightP + spreadX;
                    float my = coreY + (targetY - coreY) * flightP + arcY;
                    float mz = coreZ + (targetZ - coreZ) * flightP + spreadZ;

                    // Calculate the NEXT position to find the velocity direction (Aiming the nose)
                    float nextP = flightP + 0.01f;
                    float nextArc = sin(nextP * 3.14159f);
                    float nx = coreX + (targetX - coreX) * nextP + (cos(startAngle + (timenew+10)*0.01f) * 15.0f * nextArc);
                    float ny = coreY + (targetY - coreY) * nextP + (nextArc * 20.0f);
                    float nz = coreZ + (targetZ - coreZ) * nextP + (sin(startAngle + (timenew+10)*0.01f) * 15.0f * nextArc);

                    float dx = nx - mx, dy = ny - my, dz = nz - mz;
                    float yaw = atan2(dx, dz) * 180.0f / 3.14159f;
                    float pitch = atan2(-dy, sqrt(dx*dx + dz*dz)) * 180.0f / 3.14159f;

                    glPushMatrix();
                        glTranslatef(mx, my, mz);
                        glRotatef(yaw, 0, 1, 0);   // Steer left/right
                        glRotatef(pitch, 1, 0, 0); // Steer up/down
                        glRotatef(timenew * 1.5f, 0.0f, 0.0f, 1.0f); // Make the rocket spin like a bullet!

                        GLUquadric* q = gluNewQuadric();

                        // 1. Rocket Body
                        glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
                        gluCylinder(q, 0.25f, 0.25f, 1.2f, 8, 1);

                        // 2. Nose Cone
                        glPushMatrix();
                            glTranslatef(0.0f, 0.0f, 1.2f);
                            glColor4f(1.0f, 0.1f, 0.1f, 1.0f);
                            glutSolidCone(0.25f, 0.6f, 8, 1);
                        glPopMatrix();

                        // 3. Fins
                        glDisable(GL_CULL_FACE);
                        glColor4f(1.0f, 0.1f, 0.1f, 1.0f);
                        glBegin(GL_TRIANGLES);
                            glVertex3f( 0.0f,  0.25f, 0.2f); glVertex3f( 0.0f,  0.25f, 0.8f); glVertex3f( 0.0f,  0.7f, 0.0f);
                            glVertex3f( 0.0f, -0.25f, 0.2f); glVertex3f( 0.0f, -0.25f, 0.8f); glVertex3f( 0.0f, -0.7f, 0.0f);
                            glVertex3f(-0.25f,  0.0f, 0.2f); glVertex3f(-0.25f,  0.0f, 0.8f); glVertex3f(-0.7f,  0.0f, 0.0f);
                            glVertex3f( 0.25f,  0.0f, 0.2f); glVertex3f( 0.25f,  0.0f, 0.8f); glVertex3f( 0.7f,  0.0f, 0.0f);
                        glEnd();
                        glEnable(GL_CULL_FACE);

                        // Exhaust fire
                        glColor4f(1.0f, 1.0f, 0.5f, 1.0f);
                        glutSolidSphere(0.4f, 8, 8);

                        gluDeleteQuadric(q);
                    glPopMatrix();

                    // Rocket exhaust trails (smoke/fire left behind)
                    glLineWidth(3.0f);
                    glBegin(GL_LINE_STRIP);
                    for (float past = 0.0f; past <= 0.15f; past += 0.03f) {
                        float oldP = flightP - past;
                        if (oldP < 0.0f) continue;
                        float oldArc = sin(oldP * 3.14159f);
                        float oldX = coreX + (targetX - coreX) * oldP + (cos(startAngle + (timenew - past*100)*0.01f) * 15.0f * oldArc);
                        float oldY = coreY + (targetY - coreY) * oldP + (oldArc * 20.0f);
                        float oldZ = coreZ + (targetZ - coreZ) * oldP + (sin(startAngle + (timenew - past*100)*0.01f) * 15.0f * oldArc);
                        glColor4f(1.0f, 0.5f - (past*3.0f), 0.0f, 1.0f - (past*6.0f));
                        glVertex3f(oldX, oldY, oldZ);
                    }
                    glEnd();
                    glLineWidth(1.0f);
                }
            } else {
                // PHASE 3: Explosions on Captain America

                // FIX: Make the depth buffer read-only so explosions don't block characters!
                glDepthMask(GL_FALSE);

                for(int i = 0; i < 8; i++) {
                    glPushMatrix();
                        float offsetX = sin(timenew * (i+1)) * 5.0f;
                        float offsetY = cos(timenew * (i+1)) * 5.0f;
                        float offsetZ = sin(timenew * (i+2)) * 5.0f;
                        glTranslatef(targetX + offsetX, targetY + offsetY, targetZ + offsetZ);

                        float explosionSize = (60.0f - imAttackTimer) * 0.4f;

                        // Outer Orange Explosion
                        glColor4f(1.0f, 0.3f, 0.0f, 1.0f - (explosionSize/15.0f));
                        glutSolidSphere(explosionSize, 16, 16);

                        // Inner Yellow Core
                        glColor4f(1.0f, 1.0f, 0.5f, 1.0f - (explosionSize/12.0f));
                        glutSolidSphere(explosionSize * 0.5f, 16, 16);
                    glPopMatrix();
                }

                // FIX: Turn depth writing back on so the rest of the scene renders normally!
                glDepthMask(GL_TRUE);
            }
            glDisable(GL_BLEND); glEnable(GL_LIGHTING);

            imAttackTimer -= 0.5f;

            if (imAttackTimer == 60.0f) {
                if (caIsDefending) {
                    caHP -= 5;
                    cout << "-> BLOCKED (Partial Damage)! Captain America HP: " << caHP << "/100" << endl;
                    caIsDefending = false;
                    caDefendingAnim = false;
                } else {
                    caHP -= 20;
                    cout << "-> MISSILE BOMBARDMENT HIT! Captain America HP: " << caHP << "/100" << endl;
                }
                if (caHP <= 0) cout << "*** IRON MAN WINS! ***" << endl;
            }

            if (imAttackTimer <= 0) { imAttackingMissiles = false; if (caHP > 0) currentTurn = 2; }
        }

        // 5c. Iron Man Defend Animation
        if (imDefendingAnim) {
            imAttackTimer -= 1.0f;
            if (imAttackTimer <= 0) {
                imDefendingAnim = false;
                if (caHP > 0) currentTurn = 2; // Pass turn to Captain America after deploying
            }
        }

        // --- CUSTOM ORIGIN OFFSETS ---
        const float shieldCenterX =  9.1f,  shieldCenterY =  5.4f,  shieldCenterZ = -15.0f;
        const float hammerCenterX = -10.5f, hammerCenterY = 13.4f, hammerCenterZ = -5.0f;
        const float hammerGripX   = -10.5f, hammerGripY   = 10.8f, hammerGripZ   = 0.0f;

        const float shieldAttachX = -4.35f, shieldAttachY = 7.7f, shieldAttachZ = -0.95f;
        const float hammerAttachX =  -7.45f, hammerAttachY = 12.8f, hammerAttachZ = -0.75f;

        // 6a. Captain America Defend Animation
        if (caDefendingAnim) {
            // Instantly pass the turn, but LEAVE caDefendingAnim = true so he holds the pose!
            if (imHP > 0) currentTurn = 1;

            // We use caAttackTimer just for the 20-frame lifting animation, but cap it so it stops counting.
            if (caAttackTimer > 40) caAttackTimer -= 1.0f;
        }

        // 6b. Captain America Hammer Flight Effect
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

                    // Realistic Lightning: multiple lines appear and disappear dynamically
                    glLineWidth(3.0f);
                    glBegin(GL_LINES);
                    int numBolts = 4 + (rand() % 4); // 4 to 7 main lightning bolts per frame
                    for (int i = 0; i < numBolts; i++) {
                        float r = 0.4f + (rand() % 30) / 100.0f; // 0.4 to 0.69
                        float g = 0.8f + (rand() % 20) / 100.0f; // 0.8 to 0.99
                        float b = 1.0f;
                        float a = 0.6f + (rand() % 40) / 100.0f; // 0.6 to 0.99
                        glColor4f(r, g, b, a);
                        
                        // Top starting point
                        float topX = imX + (rand() % 100 - 50) * 0.1f;
                        float topZ = imZ + (rand() % 100 - 50) * 0.1f;
                        float topY = imY + 40.0f + (rand() % 15);
                        
                        // Target point on Iron Man's body
                        float botX = imX + (rand() % 40 - 20) * 0.1f;
                        float botZ = imZ + (rand() % 40 - 20) * 0.1f;
                        float botY = imY + 5.0f + (rand() % 10);
                        
                        float curX = topX, curY = topY, curZ = topZ;
                        
                        int segments = 10 + (rand() % 6); // 10 to 15 segments per bolt
                        for (int s = 0; s < segments; s++) {
                            float t = (float)(s + 1) / segments;
                            
                            // Interpolate and add noise
                            float nextX = topX + (botX - topX) * t + (rand() % 60 - 30) * 0.1f;
                            float nextZ = topZ + (botZ - topZ) * t + (rand() % 60 - 30) * 0.1f;
                            float nextY = topY + (botY - topY) * t;
                            
                            glVertex3f(curX, curY, curZ);
                            glVertex3f(nextX, nextY, nextZ);
                            
                            // Branching
                            if (rand() % 3 == 0) {
                                float branchX = nextX + (rand() % 80 - 40) * 0.1f;
                                float branchZ = nextZ + (rand() % 80 - 40) * 0.1f;
                                float branchY = nextY - (rand() % 40) * 0.1f;
                                glVertex3f(nextX, nextY, nextZ);
                                glVertex3f(branchX, branchY, branchZ);
                            }
                            
                            curX = nextX;
                            curY = nextY;
                            curZ = nextZ;
                        }
                    }
                    glEnd();

                    if (caAttackTimer <= 104 && caAttackTimer > 96) {
                        float flicker = 0.75f + (rand() % 25) / 100.0f;
                        glColor4f(0.2f, 0.4f, 1.0f, flicker);
                        glPushMatrix();
                            glTranslatef(imX, imY + 7.0f, imZ);
                            glutSolidSphere(6.0f, 24, 12);
                        glPopMatrix();

                        glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
                        glPushMatrix();
                            glTranslatef(imX, imY + 7.0f, imZ);
                            glutSolidSphere(2.5f + (rand() % 10) / 10.0f, 16, 12);
                        glPopMatrix();

                        glLineWidth(6.0f);
                        glColor4f(0.1f, 0.3f, 1.0f, 0.95f);
                        glBegin(GL_LINES);
                            glVertex3f(imX - 12.0f, imY + 7.0f, imZ); glVertex3f(imX + 12.0f, imY + 7.0f, imZ);
                            glVertex3f(imX, imY - 3.0f, imZ); glVertex3f(imX, imY + 18.0f, imZ);
                            glVertex3f(imX - 8.0f, imY + 1.0f, imZ + 5.0f); glVertex3f(imX + 8.0f, imY + 15.0f, imZ - 5.0f);
                            glVertex3f(imX - 8.0f, imY + 15.0f, imZ - 5.0f); glVertex3f(imX + 8.0f, imY + 1.0f, imZ + 5.0f);

                            glVertex3f(imX - 10.0f, imY + 7.0f, imZ + 9.0f); glVertex3f(imX + 10.0f, imY + 7.0f, imZ - 9.0f);
                            glVertex3f(imX - 10.0f, imY + 7.0f, imZ - 9.0f); glVertex3f(imX + 10.0f, imY + 7.0f, imZ + 9.0f);
                            glVertex3f(imX - 4.0f, imY - 2.0f, imZ - 4.0f); glVertex3f(imX + 4.0f, imY + 16.0f, imZ + 4.0f);
                            glVertex3f(imX + 4.0f, imY - 2.0f, imZ - 4.0f); glVertex3f(imX - 4.0f, imY + 16.0f, imZ + 4.0f);
                        glEnd();

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

            // ==========================================
            // DELAYED HP DEDUCTION
            // Deducts exactly at the frame the hammer/lightning hits
            // ==========================================
            if (caAttackingHammer && caAttackTimer == 120.0f) {
                if (imIsDefending) {
                    imHP -= 5;
                    cout << "-> BLOCKED (Partial Damage)! Iron Man HP: " << imHP << "/100" << endl;
                    imIsDefending = false;
                } else {
                    imHP -= 15;
                    cout << "-> HAMMER HIT! Iron Man HP: " << imHP << "/100" << endl;
                }
                if (imHP <= 0) cout << "*** CAPTAIN AMERICA WINS! ***" << endl;
            }
            if (caAttackingSuper && caAttackTimer == 120.0f) {
                if (imIsDefending) {
                    imHP -= 10;
                    cout << "-> SHIELD OVERLOADED (Reduced Damage)! Iron Man HP: " << imHP << "/100" << endl;
                    imIsDefending = false;
                } else {
                    imHP -= 30;
                    cout << "-> THUNDER HIT! Iron Man HP: " << imHP << "/100" << endl;
                }
                if (imHP <= 0) cout << "*** CAPTAIN AMERICA WINS! ***" << endl;
            }

            if (caAttackTimer <= 0) { caAttackingHammer = false; caAttackingSuper = false; if (imHP > 0) currentTurn = 1; }
        }

        // 6c. Captain America Super Activation Lightning (Realistic procedural bolts)
        if (caAttackingSuper && caAttackTimer > 180) {
            glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
            glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            // Realistic Lightning: multiple bolts from sky to Captain America
            glLineWidth(3.0f);
            glBegin(GL_LINES);
            int numBolts = 4 + (rand() % 4); // 4 to 7 main lightning bolts per frame
            for (int i = 0; i < numBolts; i++) {
                float r = 0.4f + (rand() % 30) / 100.0f; // 0.4 to 0.69
                float g = 0.8f + (rand() % 20) / 100.0f; // 0.8 to 0.99
                float b = 1.0f;
                float a = 0.6f + (rand() % 40) / 100.0f; // 0.6 to 0.99
                glColor4f(r, g, b, a);

                // Top starting point (sky above Captain America)
                float topX = animCaX + (rand() % 100 - 50) * 0.1f;
                float topZ = animCaZ + (rand() % 100 - 50) * 0.1f;
                float topY = animCaY + 40.0f + (rand() % 15);

                // Target point on Captain America's body
                float botX = animCaX + (rand() % 40 - 20) * 0.1f;
                float botZ = animCaZ + (rand() % 40 - 20) * 0.1f;
                float botY = animCaY + 5.0f + (rand() % 10);

                float curX = topX, curY = topY, curZ = topZ;

                int segments = 10 + (rand() % 6); // 10 to 15 segments per bolt
                for (int s = 0; s < segments; s++) {
                    float t = (float)(s + 1) / segments;

                    // Interpolate and add noise
                    float nextX = topX + (botX - topX) * t + (rand() % 60 - 30) * 0.1f;
                    float nextZ = topZ + (botZ - topZ) * t + (rand() % 60 - 30) * 0.1f;
                    float nextY = topY + (botY - topY) * t;

                    glVertex3f(curX, curY, curZ);
                    glVertex3f(nextX, nextY, nextZ);

                    // Branching
                    if (rand() % 3 == 0) {
                        float branchX = nextX + (rand() % 80 - 40) * 0.1f;
                        float branchZ = nextZ + (rand() % 80 - 40) * 0.1f;
                        float branchY = nextY - (rand() % 40) * 0.1f;
                        glVertex3f(nextX, nextY, nextZ);
                        glVertex3f(branchX, branchY, branchZ);
                    }

                    curX = nextX;
                    curY = nextY;
                    curZ = nextZ;
                }
            }
            glEnd();

            // Flickering flash sphere and energy burst on Captain America
            if ((int)caAttackTimer % 8 > 3) {
                float flicker = 0.75f + (rand() % 25) / 100.0f;
                glColor4f(0.2f, 0.4f, 1.0f, flicker);
                glPushMatrix();
                    glTranslatef(animCaX, animCaY + 7.0f, animCaZ);
                    glutSolidSphere(6.0f, 24, 12);
                glPopMatrix();

                glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
                glPushMatrix();
                    glTranslatef(animCaX, animCaY + 7.0f, animCaZ);
                    glutSolidSphere(2.5f + (rand() % 10) / 10.0f, 16, 12);
                glPopMatrix();

                // Energy cross lines
                glLineWidth(6.0f);
                glColor4f(0.1f, 0.3f, 1.0f, 0.95f);
                glBegin(GL_LINES);
                    glVertex3f(animCaX - 12.0f, animCaY + 7.0f, animCaZ); glVertex3f(animCaX + 12.0f, animCaY + 7.0f, animCaZ);
                    glVertex3f(animCaX, animCaY - 3.0f, animCaZ); glVertex3f(animCaX, animCaY + 18.0f, animCaZ);
                    glVertex3f(animCaX - 8.0f, animCaY + 1.0f, animCaZ + 5.0f); glVertex3f(animCaX + 8.0f, animCaY + 15.0f, animCaZ - 5.0f);
                    glVertex3f(animCaX - 8.0f, animCaY + 15.0f, animCaZ - 5.0f); glVertex3f(animCaX + 8.0f, animCaY + 1.0f, animCaZ + 5.0f);

                    glVertex3f(animCaX - 10.0f, animCaY + 7.0f, animCaZ + 9.0f); glVertex3f(animCaX + 10.0f, animCaY + 7.0f, animCaZ - 9.0f);
                    glVertex3f(animCaX - 10.0f, animCaY + 7.0f, animCaZ - 9.0f); glVertex3f(animCaX + 10.0f, animCaY + 7.0f, animCaZ + 9.0f);
                    glVertex3f(animCaX - 4.0f, animCaY - 2.0f, animCaZ - 4.0f); glVertex3f(animCaX + 4.0f, animCaY + 16.0f, animCaZ + 4.0f);
                    glVertex3f(animCaX + 4.0f, animCaY - 2.0f, animCaZ - 4.0f); glVertex3f(animCaX - 4.0f, animCaY + 16.0f, animCaZ + 4.0f);
                glEnd();

                // Energy ring around Captain America
                glLineWidth(2.5f);
                glColor4f(0.3f, 0.6f, 1.0f, 0.6f);
                glBegin(GL_LINE_LOOP);
                    for (int i = 0; i < 16; i++) {
                        float ang = i * (360.0f / 16) * 3.14159265f / 180.0f;
                        glVertex3f(animCaX + cos(ang) * 9.0f, animCaY + 7.1f, animCaZ + sin(ang) * 9.0f);
                    }
                glEnd();
            }

            glLineWidth(1.0f);
            glDisable(GL_BLEND); glEnable(GL_LIGHTING);
        }

        // 7. Draw Characters
        // --- Iron Man ---
        glPushMatrix();
            glTranslatef(animImX, animImY, animImZ);
            if (imAttacking || imAttackingMissiles) glRotatef(imTiltX, 1.0f, 0.0f, 0.0f);
            ironman.draw();

            // NEW: Draw the Forcefield if Defending
            if (imIsDefending || imDefendingAnim) {
                glTranslatef(0.0f, 12.0f, 0.0f); // Center shield on chest
                glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
                glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE);

                float pulse = (sin(timenew * 0.01f) + 1.0f) * 0.5f;
                // Transparent inner glow
                glColor4f(0.0f, 0.5f + 0.3f*pulse, 1.0f, 0.2f + 0.1f*pulse);
                glutSolidSphere(12.0f, 16, 16);
                // Bright wireframe shell
                glColor4f(0.6f, 0.8f, 1.0f, 0.8f);
                glutWireSphere(12.2f, 16, 16);

                glDisable(GL_BLEND); glEnable(GL_LIGHTING);
            }
        glPopMatrix();

        // --- Captain America ---
        glPushMatrix();
            glTranslatef(animCaX, animCaY, animCaZ);
            glRotatef(180, 0, 0, 1);
            glRotatef(180, 1, 0, 0);

            if (caDefendingAnim || caAttackingHammer || caAttackingSuper) {
                glRotatef(caTiltX, 1.0f, 0.0f, 0.0f);
                glRotatef(caRotY,  0.0f, 1.0f, 0.0f);
            }

            if (splitCaptainLoaded) {
                captainBody.draw();

               float leftArmPitch = 0.0f;
                float rightArmPitch = 0.0f;
                float rightArmYaw = 0.0f;
                float rightArmRoll = 0.0f; // NEW: To tilt the shield face forward

                // Math for Shield Block
                if (caDefendingAnim) {
                    // Pull the arm further across the chest (-75 Yaw) and tilt the shield (-25 Roll)
                    if (caAttackTimer > 40) {
                        float p = (60.0f - caAttackTimer) / 20.0f;
                        rightArmPitch = p * -75.0f;
                        rightArmYaw   = p * -75.0f;
                        rightArmRoll  = p * -25.0f;
                    } else {
                        rightArmPitch = -75.0f;
                        rightArmYaw   = -75.0f;
                        rightArmRoll  = -25.0f;
                    }
                }

                // Math for Hammer/Super Attack (THIS WAS ACCIDENTALLY DELETED!)
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

                // ==========================================
                // LEFT HAND (Holds Hammer after swap)
                // ==========================================
                glPushMatrix();
                    glTranslatef(-1.95f, 18.8f, -0.35f);
                    if (caAttackingHammer || caAttackingSuper) glRotatef(leftArmPitch, 1.0f, 0.0f, 0.0f);
                    else glRotatef(captainHandAngle, 0.0f, 0.0f, 1.0f);
                    glTranslatef(2.35f, -18.8f, 0.35f);

                    // Shift the left arm slightly inwards to close the gap with the body
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
                // RIGHT HAND (Holds Shield to block)
                // ==========================================
                glPushMatrix();
                    glTranslatef(1.85f, 18.8f, -0.35f);
                    if (caDefendingAnim) {
                        glRotatef(rightArmYaw, 0.0f, 1.0f, 0.0f);   // Swing deep across chest
                        glRotatef(rightArmPitch, 1.0f, 0.0f, 0.0f); // Lift arm up
                        glRotatef(rightArmRoll, 0.0f, 0.0f, 1.0f);  // Tilt shield to face enemy
                    }
                    else glRotatef(-captainHandAngle, 0.0f, 0.0f, 1.0f);
                    glTranslatef(-2.35f, -18.8f, 0.35f);

                    captainRightHand.draw();

                    // Always draw the custom shield in his hand
                    if (customWeaponsLoaded) {
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

void MyVirtualWorld::drawSkybox(float camX, float camY, float camZ)
{
    glDisable(GL_LIGHTING);
    glDepthMask(GL_FALSE);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glPushMatrix();
    glTranslatef(camX, camY, camZ);
    float s = 250.0f;
    glColor3f(1.0f, 1.0f, 1.0f);

    float u0 = 0.002f, u1 = 0.998f;
    float v0 = 0.002f, v1 = 0.998f;

    if (skyboxTextureIDs[4]) {
        glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[4]);
        glBegin(GL_QUADS);
            glTexCoord2f(u0, v0); glVertex3f(-s, -s, -s);
            glTexCoord2f(u1, v0); glVertex3f( s, -s, -s);
            glTexCoord2f(u1, v1); glVertex3f( s,  s, -s);
            glTexCoord2f(u0, v1); glVertex3f(-s,  s, -s);
        glEnd();
    }
    if (skyboxTextureIDs[5]) {
        glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[5]);
        glBegin(GL_QUADS);
            glTexCoord2f(u0, v0); glVertex3f( s, -s,  s);
            glTexCoord2f(u1, v0); glVertex3f(-s, -s,  s);
            glTexCoord2f(u1, v1); glVertex3f(-s,  s,  s);
            glTexCoord2f(u0, v1); glVertex3f( s,  s,  s);
        glEnd();
    }
    if (skyboxTextureIDs[1]) {
        glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[1]);
        glBegin(GL_QUADS);
            glTexCoord2f(u0, v0); glVertex3f(-s, -s,  s);
            glTexCoord2f(u1, v0); glVertex3f(-s, -s, -s);
            glTexCoord2f(u1, v1); glVertex3f(-s,  s, -s);
            glTexCoord2f(u0, v1); glVertex3f(-s,  s,  s);
        glEnd();
    }
    if (skyboxTextureIDs[0]) {
        glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[0]);
        glBegin(GL_QUADS);
            glTexCoord2f(u0, v0); glVertex3f( s, -s, -s);
            glTexCoord2f(u1, v0); glVertex3f( s, -s,  s);
            glTexCoord2f(u1, v1); glVertex3f( s,  s,  s);
            glTexCoord2f(u0, v1); glVertex3f( s,  s, -s);
        glEnd();
    }
    if (skyboxTextureIDs[2]) {
        glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[2]);
        glBegin(GL_QUADS);
            glTexCoord2f(u0, v0); glVertex3f(-s,  s, -s);
            glTexCoord2f(u1, v0); glVertex3f( s,  s, -s);
            glTexCoord2f(u1, v1); glVertex3f( s,  s,  s);
            glTexCoord2f(u0, v1); glVertex3f(-s,  s,  s);
        glEnd();
    }
    if (skyboxTextureIDs[3]) {
        glBindTexture(GL_TEXTURE_2D, skyboxTextureIDs[3]);
        glBegin(GL_QUADS);
            glTexCoord2f(u0, v1); glVertex3f(-s, -s, -s);
            glTexCoord2f(u1, v1); glVertex3f( s, -s, -s);
            glTexCoord2f(u1, v0); glVertex3f( s, -s,  s);
            glTexCoord2f(u0, v0); glVertex3f(-s, -s,  s);
        glEnd();
    }

    glPopMatrix();

    glDepthMask(GL_TRUE);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}
