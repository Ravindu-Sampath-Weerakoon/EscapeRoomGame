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


/**
 * @brief Draws (X, Z) coordinate labels in the center of each grid square.
 */
void drawGridCoordinates(float size, int numSegments) {
    // Disable lighting for the text
    glDisable(GL_LIGHTING);
    // Set text color (e.g., yellow for visibility)
    glColor3f(1.0f, 1.0f, 0.5f);

    float halfSize = size / 2.0f;
    float segmentSize = size / numSegments;
    float textYOffset = 0.02f; // Slightly above the grid lines

    // Choose a font (smaller is better for grid labels)
    void* font = GLUT_BITMAP_HELVETICA_10; // Or GLUT_BITMAP_8_BY_13

    char textBuffer[16]; // Buffer to hold the coordinate string like "(-12,34)"

    // Loop through each grid cell
    for (int i = 0; i < numSegments; ++i) { // Loop for Z direction
        for (int j = 0; j < numSegments; ++j) { // Loop for X direction

            // Calculate the coordinates of the *bottom-left* corner of the cell
            float cellX_BL = -halfSize + (j * segmentSize);
            float cellZ_BL = -halfSize + (i * segmentSize);

            // Calculate the center coordinates of the cell
            float centerX = cellX_BL + (segmentSize / 2.0f);
            float centerZ = cellZ_BL + (segmentSize / 2.0f);

            // Determine the integer grid coordinates
            // Use floor to correctly handle negative coordinates
            int gridCoordX = (int)floor(centerX / segmentSize);
            int gridCoordZ = (int)floor(centerZ / segmentSize);

            // Format the coordinate string using the safer sprintf_s
            sprintf_s(textBuffer, sizeof(textBuffer), "(%d,%d)", gridCoordX, gridCoordZ);

            // Set the 3D position for the text rasterization
            glRasterPos3f(centerX, textYOffset, centerZ);

            // Draw the string character by character
            char* c = textBuffer;
            while (*c) {
                glutBitmapCharacter(font, *c);
                c++;
            }
        }
    }

    // Re-enable lighting
    glEnable(GL_LIGHTING);
    // Reset color
    glColor3f(1.0f, 1.0f, 1.0f);
}
