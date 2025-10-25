// GraphicsUtils.cpp : Defines the functions for the static library.
//
#include "pch.h" // Must be first
#include "GraphicsUtils.h" // Include your own header
#include <stdio.h> // For sprintf_s
#include <math.h>  // For floor, sqrt
#include <vector>  // For the collision grid

// --- Grid Constants Definitions ---
// These provide the concrete values for the 'extern' declarations in the header
const float GRID_SIZE = 40.0f;
const int   GRID_SEGMENTS = 40;
const float GRID_CELL_SIZE = GRID_SIZE / GRID_SEGMENTS;
const float GRID_HALF_SIZE = GRID_SIZE / 2.0f;

// --- Collision Grid Data ---
// Using vector for easier initialization. True means blocked.
std::vector<std::vector<bool>> g_collisionGrid(GRID_SEGMENTS, std::vector<bool>(GRID_SEGMENTS, false));

// --- Function Definitions ---

/**
 * @brief Draws X, Y, and Z axes.
 */
void drawAxes(float length) {
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
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
}

/**
 * @brief Draws a grid of lines on the XZ plane.
 */
void drawGrid(float size, int numSegments) {
    glDisable(GL_LIGHTING);
    glColor3f(0.4f, 0.4f, 0.4f); // Medium grey

    float halfSize = size / 2.0f;
    float segmentSize = size / numSegments;

    glBegin(GL_LINES);
    for (int i = 0; i <= numSegments; ++i) {
        float pos = -halfSize + (i * segmentSize);

        // Lines parallel to Z-axis
        glVertex3f(pos, 0.0f, -halfSize);
        glVertex3f(pos, 0.0f, halfSize);

        // Lines parallel to X-axis
        glVertex3f(-halfSize, 0.0f, pos);
        glVertex3f(halfSize, 0.0f, pos);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
}

/**
 * @brief Draws (X, Z) coordinate labels in the center of each grid square.
 */
void drawGridCoordinates(float size, int numSegments) {
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.5f); // Yellow text

    float halfSize = size / 2.0f;
    float segmentSize = size / numSegments;
    float textYOffset = 0.02f; // Slightly above the grid lines

    void* font = GLUT_BITMAP_HELVETICA_10; // Small font
    char textBuffer[16];

    for (int i = 0; i < numSegments; ++i) { // Z loop (rows)
        for (int j = 0; j < numSegments; ++j) { // X loop (columns)

            float cellX_BL = -halfSize + (j * segmentSize);
            float cellZ_BL = -halfSize + (i * segmentSize);
            float centerX = cellX_BL + (segmentSize / 2.0f);
            float centerZ = cellZ_BL + (segmentSize / 2.0f);

            int gridCoordX = (int)floor(centerX / segmentSize);
            int gridCoordZ = (int)floor(centerZ / segmentSize);

            // Use safer sprintf_s
            sprintf_s(textBuffer, sizeof(textBuffer), "(%d,%d)", gridCoordX, gridCoordZ);

            glRasterPos3f(centerX, textYOffset, centerZ);

            char* c = textBuffer;
            while (*c) {
                glutBitmapCharacter(font, *c);
                c++;
            }
        }
    }

    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f); // Reset color
}

/**
 * @brief Converts world X, Z coordinates to grid indices.
 */
bool worldToGrid(float worldX, float worldZ, int& gridX, int& gridZ) {
    float shiftedX = worldX + GRID_HALF_SIZE;
    float shiftedZ = worldZ + GRID_HALF_SIZE;

    gridX = static_cast<int>(floor(shiftedX / GRID_CELL_SIZE));
    gridZ = static_cast<int>(floor(shiftedZ / GRID_CELL_SIZE));

    // Check bounds
    return (gridX >= 0 && gridX < GRID_SEGMENTS && gridZ >= 0 && gridZ < GRID_SEGMENTS);
}

/**
 * @brief Checks if a target world position (X, Z) corresponds to a blocked grid cell.
 */
bool isGridPositionBlocked(float worldX, float worldZ) {
    int gridX, gridZ;
    if (worldToGrid(worldX, worldZ, gridX, gridZ)) {
        // Access using [row][column] which corresponds to [Z][X]
        return g_collisionGrid[gridZ][gridX];
    }
    return true; // Outside the defined grid is considered blocked
}

/**
 * @brief Marks a grid cell as blocked using integer grid coordinates.
 */
void addBlockGridBox(int gridX, int gridZ) {
    // Check if the grid coordinates are valid before accessing the vector
    if (gridZ >= 0 && gridZ < g_collisionGrid.size() && gridX >= 0 && gridX < g_collisionGrid[0].size()) {
        g_collisionGrid[gridZ][gridX] = true; // Mark as blocked
    }
    else {
        printf("Warning: Attempted to block invalid grid cell (%d, %d)\n", gridX, gridZ);
    }
}

/**
 * @brief Marks a grid cell as unblocked (walkable) using integer grid coordinates.
 */
void removeBlockGridBox(int gridX, int gridZ) {
    // Check if the grid coordinates are valid before accessing the vector
    if (gridZ >= 0 && gridZ < g_collisionGrid.size() && gridX >= 0 && gridX < g_collisionGrid[0].size()) {
        g_collisionGrid[gridZ][gridX] = false; // Mark as unblocked (walkable)
    }
    else {
        printf("Warning: Attempted to unblock invalid grid cell (%d, %d)\n", gridX, gridZ);
    }
}


void drawTexturedCube(float size, GLuint textureID) {
    if (textureID == 0) {
        // Fallback: Draw a simple pink cube if texture failed to load
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glColor3f(1.0f, 0.0f, 1.0f);
        glutSolidCube(size);
        glEnable(GL_LIGHTING);
        glColor3f(1.0f, 1.0f, 1.0f);
        return;
    }
    

    float halfSize = size / 2.0f;

    // Set material color to white (for GL_MODULATE)
    glColor3f(1.0f, 1.0f, 1.0f);

    // Enable texturing and bind the provided texture
    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Use modulate to see lighting
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBegin(GL_QUADS);
    // Front Face (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);

    // Back Face (-Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);

    // Top Face (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, halfSize, halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, halfSize, halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);

    // Bottom Face (-Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, -halfSize, -halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);

    // Right face (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(halfSize, -halfSize, -halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(halfSize, halfSize, -halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(halfSize, halfSize, halfSize);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(halfSize, -halfSize, halfSize);

    // Left Face (-X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfSize, -halfSize, -halfSize);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfSize, -halfSize, halfSize);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfSize, halfSize, halfSize);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfSize, halfSize, -halfSize);
    glEnd();

    // Cleanup
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

   
}