
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
