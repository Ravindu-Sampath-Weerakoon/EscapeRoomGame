#include "pch.h" // Must be first
#include <stdio.h>
#include "CornerTower.h"
#include "GraphicsUtils.h" // Needed for worldToGrid and addBlockGridBox

#include <math.h> // For fabs()

// Constructor: Sets up the dimensions shared by all towers
CornerTower::CornerTower(float roomHeight, float towerWidth)
    : m_height(roomHeight), m_width(towerWidth),
    m_textureID(0), m_displayListID(0)
{
    // Design Tweaks: Adjust these to change how the rims look
    m_rimHeight = 0.2f;    // The base and top are 0.2 units high
    m_rimOverhang = 0.2f;  // The rims stick out 0.2 units wider than the tower
}

// Destructor: Clean up GPU memory
CornerTower::~CornerTower() {
    if (m_displayListID != 0) {
        glDeleteLists(m_displayListID, 1);
    }
}

// Add a new tower at a specific X, Z location
void CornerTower::addTower(float x, float z) {
    // 1. Store the position
    TowerPos t;
    t.x = x;
    t.z = z;
    m_towers.push_back(t);

    // 2. IMMEDIATE COLLISION UPDATE
    // We calculate the footprint of the tower (including the rim)
    // and block the corresponding squares on the global grid.

    float totalWidth = m_width + (m_rimOverhang * 2.0f);
    float halfW = totalWidth / 2.0f;

    // Loop through the tower's footprint (stepping by 0.5 to catch all grid cells)
    for (float i = x - halfW; i <= x + halfW; i += 0.5f) {
        for (float j = z - halfW; j <= z + halfW; j += 0.5f) {
            int gx, gz;
            // Convert world coordinate to grid index
            if (worldToGrid(i, j, gx, gz)) {
                addBlockGridBox(gx, gz); // Block this square!
            }
        }
    }
    printf("CornerTower added at (%.1f, %.1f) - Collision Updated.\n", x, z);
}

// Generate the OpenGL Display List
void CornerTower::build(GLuint textureID) {
    m_textureID = textureID;

    // Clean up old list if exists
    if (m_displayListID != 0) glDeleteLists(m_displayListID, 1);

    // Generate new list ID
    m_displayListID = glGenLists(1);

    // Start Recording commands
    glNewList(m_displayListID, GL_COMPILE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glColor3f(1.0f, 1.0f, 1.0f); // White material to show texture clearly

    // LOOP through all added towers and draw them
    for (const auto& tower : m_towers) {
        float x = tower.x;
        float z = tower.z;

        // --- PART 1: BOTTOM RIM (The Base) ---
        glPushMatrix();
        // Move to position. Y is half of rim height so it sits exactly on floor.
        glTranslatef(x, m_rimHeight / 2.0f, z);
        drawBox(m_width + m_rimOverhang * 2, m_rimHeight, m_width + m_rimOverhang * 2);
        glPopMatrix();

        // --- PART 2: MAIN SHAFT (The Middle) ---
        float shaftHeight = m_height - (m_rimHeight * 2); // Subtract top and bottom rims
        glPushMatrix();
        // Move up: Bottom Rim Height + Half Shaft Height
        glTranslatef(x, m_rimHeight + (shaftHeight / 2.0f), z);
        drawBox(m_width, shaftHeight, m_width);
        glPopMatrix();

        // --- PART 3: TOP RIM (The Cap) ---
        glPushMatrix();
        // Move to very top: Total Height - Half Rim Height
        glTranslatef(x, m_height - (m_rimHeight / 2.0f), z);
        drawBox(m_width + m_rimOverhang * 2, m_rimHeight, m_width + m_rimOverhang * 2);
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    glEndList(); // Stop Recording
}

// Render the towers
void CornerTower::draw() {
    if (m_displayListID != 0) {
        glCallList(m_displayListID);
    }
}

// Helper to draw a textured box centered at (0,0,0)
void CornerTower::drawBox(float w, float h, float d) {
    float hw = w / 2.0f; // Half Width
    float hh = h / 2.0f; // Half Height
    float hd = d / 2.0f; // Half Depth

    glBegin(GL_QUADS);
    // Front Face (+Z)
    glNormal3f(0, 0, 1);
    glTexCoord2f(0, 0); glVertex3f(-hw, -hh, hd);
    glTexCoord2f(1, 0); glVertex3f(hw, -hh, hd);
    glTexCoord2f(1, 1); glVertex3f(hw, hh, hd);
    glTexCoord2f(0, 1); glVertex3f(-hw, hh, hd);

    // Back Face (-Z)
    glNormal3f(0, 0, -1);
    glTexCoord2f(0, 0); glVertex3f(hw, -hh, -hd);
    glTexCoord2f(1, 0); glVertex3f(-hw, -hh, -hd);
    glTexCoord2f(1, 1); glVertex3f(-hw, hh, -hd);
    glTexCoord2f(0, 1); glVertex3f(hw, hh, -hd);

    // Left Face (-X)
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd);
    glTexCoord2f(1, 0); glVertex3f(-hw, -hh, hd);
    glTexCoord2f(1, 1); glVertex3f(-hw, hh, hd);
    glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);

    // Right Face (+X)
    glNormal3f(1, 0, 0);
    glTexCoord2f(0, 0); glVertex3f(hw, -hh, hd);
    glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd);
    glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd);
    glTexCoord2f(0, 1); glVertex3f(hw, hh, hd);

    // Top Face (+Y)
    glNormal3f(0, 1, 0);
    glTexCoord2f(0, 0); glVertex3f(-hw, hh, hd);
    glTexCoord2f(1, 0); glVertex3f(hw, hh, hd);
    glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd);
    glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);

    // Bottom Face (-Y)
    glNormal3f(0, -1, 0);
    glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd);
    glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd);
    glTexCoord2f(1, 1); glVertex3f(hw, -hh, hd);
    glTexCoord2f(0, 1); glVertex3f(-hw, -hh, hd);
    glEnd();
}