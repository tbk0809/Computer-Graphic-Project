
    // Iron Man - right side
    glPushMatrix();
        glTranslatef(10, -5, -15);
        ironman.draw();
    glPopMatrix();

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
