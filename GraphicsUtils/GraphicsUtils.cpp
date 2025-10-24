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

void drawGrid(float size, int numSegments)
{
    // Disable lighting for the grid
    glDisable(GL_LIGHTING);

    // Set the grid color (e.g., a medium grey)
    glColor3f(0.4f, 0.4f, 0.4f);

    float halfSize = size / 2.0f;
    float segmentSize = size / numSegments;

    glBegin(GL_LINES);
    // Loop from -halfSize to +halfSize
    for (int i = 0; i <= numSegments; ++i) {
        float pos = -halfSize + (i * segmentSize);

        // Draw line parallel to the Z-axis (at a constant x)
        glVertex3f(pos, 0.0f, -halfSize);
        glVertex3f(pos, 0.0f, halfSize);

        // Draw line parallel to the X-axis (at a constant z)
        glVertex3f(-halfSize, 0.0f, pos);
        glVertex3f(halfSize, 0.0f, pos);
    }
    glEnd();

    // Re-enable lighting for the rest of the scene
    glEnable(GL_LIGHTING);

    // Reset color to white
    glColor3f(1.0f, 1.0f, 1.0f);
}
