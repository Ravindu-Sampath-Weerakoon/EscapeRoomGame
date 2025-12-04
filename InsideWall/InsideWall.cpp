#include "pch.h" // Must be first
#include "InsideWall.h"
#include "GraphicsUtils.h" 
#include <math.h>
#include <stdio.h>

InsideWall::InsideWall(float height)
    : m_height(height), m_textureID(0), m_displayListID(0) {
}

void InsideWall::addWall(float startX, float startZ, float endX, float endZ, float thickness) {
    WallSegment w;
    w.startX = startX;
    w.startZ = startZ;
    w.endX = endX;
    w.endZ = endZ;
    w.thickness = thickness;
    w.height = m_height;
    m_walls.push_back(w);

    // ==========================================================
    // FIXED COLLISION LOGIC: Create a Thicker Barrier
    // ==========================================================

    // 1. Calculate the actual boundaries of the wall (Bounding Box)
    // We expand the area by half the thickness in all directions.
    float halfThick = thickness / 2.0f;

    // Find min and max for the LINE part
    float lineMinX = (startX < endX) ? startX : endX;
    float lineMaxX = (startX < endX) ? endX : startX;
    float lineMinZ = (startZ < endZ) ? startZ : endZ;
    float lineMaxZ = (startZ < endZ) ? endZ : startZ;

    // Expand by thickness (so the collision is wide enough)
    // We add a tiny extra bit (+0.1f) to ensure we catch the border grid cells
    float boxMinX = lineMinX - halfThick;
    float boxMaxX = lineMaxX + halfThick;
    float boxMinZ = lineMinZ - halfThick;
    float boxMaxZ = lineMaxZ + halfThick;

    // 2. Loop through this "Rectangle" on the floor
    // We step by 0.5f to ensure every grid cell inside the wall is caught
    for (float x = boxMinX; x <= boxMaxX; x += 0.5f) {
        for (float z = boxMinZ; z <= boxMaxZ; z += 0.5f) {

            int gridX, gridZ;
            // If this point is on the grid, BLOCK IT
            if (worldToGrid(x, z, gridX, gridZ)) {
                addBlockGridBox(gridX, gridZ);
            }
        }
    }

    printf("Added Wall Collision: X[%.1f to %.1f] Z[%.1f to %.1f]\n", boxMinX, boxMaxX, boxMinZ, boxMaxZ);
}

void InsideWall::build(GLuint textureID) {
    m_textureID = textureID;

    if (m_displayListID != 0) glDeleteLists(m_displayListID, 1);
    m_displayListID = glGenLists(1);

    glNewList(m_displayListID, GL_COMPILE);

    // Bind Texture
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glColor3f(1.0f, 1.0f, 1.0f);

    for (const auto& wall : m_walls) {
        // Visual Drawing Logic (Unchanged)
        float centerX = (wall.startX + wall.endX) / 2.0f;
        float centerZ = (wall.startZ + wall.endZ) / 2.0f;

        float width = fabs(wall.endX - wall.startX);
        float depth = fabs(wall.endZ - wall.startZ);

        // Add thickness visually
        if (width > depth) { depth = wall.thickness; }
        else { width = wall.thickness; }

        glPushMatrix();
        glTranslatef(centerX, wall.height / 2.0f, centerZ);
        glScalef(width, wall.height, depth);

        // Simple textured box 
        float s = 0.5f;
        glBegin(GL_QUADS);
        // Front
        glNormal3f(0, 0, 1); glTexCoord2f(0, 0); glVertex3f(-s, -s, s); glTexCoord2f(1, 0); glVertex3f(s, -s, s); glTexCoord2f(1, 1); glVertex3f(s, s, s); glTexCoord2f(0, 1); glVertex3f(-s, s, s);
        // Back
        glNormal3f(0, 0, -1); glTexCoord2f(0, 0); glVertex3f(s, -s, -s); glTexCoord2f(1, 0); glVertex3f(-s, -s, -s); glTexCoord2f(1, 1); glVertex3f(-s, s, -s); glTexCoord2f(0, 1); glVertex3f(s, s, -s);
        // Left
        glNormal3f(-1, 0, 0); glTexCoord2f(0, 0); glVertex3f(-s, -s, -s); glTexCoord2f(1, 0); glVertex3f(-s, -s, s); glTexCoord2f(1, 1); glVertex3f(-s, s, s); glTexCoord2f(0, 1); glVertex3f(-s, s, -s);
        // Right
        glNormal3f(1, 0, 0); glTexCoord2f(0, 0); glVertex3f(s, -s, s); glTexCoord2f(1, 0); glVertex3f(s, -s, -s); glTexCoord2f(1, 1); glVertex3f(s, s, -s); glTexCoord2f(0, 1); glVertex3f(s, s, s);
        // Top
        glNormal3f(0, 1, 0); glTexCoord2f(0, 0); glVertex3f(-s, s, s); glTexCoord2f(1, 0); glVertex3f(s, s, s); glTexCoord2f(1, 1); glVertex3f(s, s, -s); glTexCoord2f(0, 1); glVertex3f(-s, s, -s);
        glEnd();

        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    glEndList();
}

void InsideWall::draw() {
    if (m_displayListID != 0) {
        glCallList(m_displayListID);
    }
}