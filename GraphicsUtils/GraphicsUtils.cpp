// GraphicsUtils.cpp : Defines the functions for the static library.
//
#include "pch.h"

/**
 * @brief Draws X, Y, and Z axes.
 */
void drawAxes(float length) {
    // We don't need to include <glut.h> because
    // it's already in "pch.h"

    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);

    // X-axis in Red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-length, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);

    // Y-axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -length, 0.0f);
    glVertex3f(0.0f, length, 0.0f);

    // Z-axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -length);
    glVertex3f(0.0f, 0.0f, length);

    glEnd();

    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);


}
