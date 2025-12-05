#include "pch.h" // Must be first
#include "CornerTower.h"
#include "GraphicsUtils.h" // For collision functions
#include <stdio.h>
#include <math.h>

// Constructor sets the shared dimensions
CornerTower::CornerTower(float roomHeight, float towerWidth)
    : m_height(roomHeight), m_width(towerWidth),
    m_textureID(0), m_displayListID(0)
{
    // Design Tweaks: 
    // We will use these for the cascading effect
    m_rimHeight = 0.15f;    // Thinner individual layers for cascade
    m_rimOverhang = 0.15f;  // Each layer steps in by this much
}

CornerTower::~CornerTower() {
    if (m_displayListID != 0) glDeleteLists(m_displayListID, 1);
}

// Add a tower position to the list
void CornerTower::addTower(float x, float z) {
    TowerPos t;
    t.x = x;
    t.z = z;
    m_towers.push_back(t);

    // --- COLLISION UPDATE ---
    // Collision covers the WIDEST part (the bottom-most base layer)
    // 3 layers of overhang means width + (3 * overhang * 2)
    float maxBaseWidth = m_width + (m_rimOverhang * 3.0f * 2.0f);
    float halfW = maxBaseWidth / 2.0f;

    for (float i = x - halfW; i <= x + halfW; i += 0.5f) {
        for (float j = z - halfW; j <= z + halfW; j += 0.5f) {
            int gx, gz;
            if (worldToGrid(i, j, gx, gz)) {
                addBlockGridBox(gx, gz);
            }
        }
    }
    printf("Design Tower added at (%.1f, %.1f)\n", x, z);
}

void CornerTower::build(GLuint textureID) {
    m_textureID = textureID;

    if (m_displayListID != 0) glDeleteLists(m_displayListID, 1);
    m_displayListID = glGenLists(1);

    glNewList(m_displayListID, GL_COMPILE);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glColor3f(1.0f, 1.0f, 1.0f);

    for (const auto& tower : m_towers) {
        float x = tower.x;
        float z = tower.z;

        // ======================================================
        // 1. CASCADING BASE (3 Layers)
        // ======================================================
        // Layer 1 (Bottom - Widest)
        float baseY = m_rimHeight / 2.0f;
        float layer1W = m_width + (m_rimOverhang * 6.0f); // Widest
        glPushMatrix();
        glTranslatef(x, baseY, z);
        drawBox(layer1W, m_rimHeight, layer1W);
        glPopMatrix();

        // Layer 2 (Middle Base)
        baseY += m_rimHeight;
        float layer2W = m_width + (m_rimOverhang * 4.0f);
        glPushMatrix();
        glTranslatef(x, baseY, z);
        drawBox(layer2W, m_rimHeight, layer2W);
        glPopMatrix();

        // Layer 3 (Top Base)
        baseY += m_rimHeight;
        float layer3W = m_width + (m_rimOverhang * 2.0f);
        glPushMatrix();
        glTranslatef(x, baseY, z);
        drawBox(layer3W, m_rimHeight, layer3W);
        glPopMatrix();

        // Total height used by base
        float totalBaseH = m_rimHeight * 3.0f;


        // ======================================================
        // 2. CASCADING TOP CAP (3 Layers - Reversed)
        // ======================================================
        float topY = m_height - (m_rimHeight / 2.0f);

        // Layer 1 (Top-most - Widest)
        glPushMatrix();
        glTranslatef(x, topY, z);
        drawBox(layer1W, m_rimHeight, layer1W);
        glPopMatrix();

        // Layer 2
        topY -= m_rimHeight;
        glPushMatrix();
        glTranslatef(x, topY, z);
        drawBox(layer2W, m_rimHeight, layer2W);
        glPopMatrix();

        // Layer 3
        topY -= m_rimHeight;
        glPushMatrix();
        glTranslatef(x, topY, z);
        drawBox(layer3W, m_rimHeight, layer3W);
        glPopMatrix();

        float totalTopH = m_rimHeight * 3.0f;


        // ======================================================
        // 3. DETAILED MAIN SHAFT
        // ======================================================
        float shaftH = m_height - totalBaseH - totalTopH;
        float shaftCenterY = totalBaseH + (shaftH / 2.0f);

        // A. Inner Core (The main block)
        glPushMatrix();
        glTranslatef(x, shaftCenterY, z);
        drawBox(m_width * 0.9f, shaftH, m_width * 0.9f); // Slightly inset
        glPopMatrix();

        // B. Corner Pillars (Vertical Ridges)
        // We draw 4 thin posts at the corners of the shaft to give it a "framed" look
        float postW = m_width * 0.15f; // Thin posts
        float postOffset = (m_width / 2.0f) - (postW / 2.0f); // Push to corners

        // Front-Left Post
        glPushMatrix();
        glTranslatef(x - postOffset, shaftCenterY, z + postOffset);
        drawBox(postW, shaftH, postW);
        glPopMatrix();

        // Front-Right Post
        glPushMatrix();
        glTranslatef(x + postOffset, shaftCenterY, z + postOffset);
        drawBox(postW, shaftH, postW);
        glPopMatrix();

        // Back-Left Post
        glPushMatrix();
        glTranslatef(x - postOffset, shaftCenterY, z - postOffset);
        drawBox(postW, shaftH, postW);
        glPopMatrix();

        // Back-Right Post
        glPushMatrix();
        glTranslatef(x + postOffset, shaftCenterY, z - postOffset);
        drawBox(postW, shaftH, postW);
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
    glEndList();
}

void CornerTower::draw() {
    if (m_displayListID != 0) {
        glCallList(m_displayListID);
    }
}

// Helper to draw a textured cube centered at (0,0,0)
void CornerTower::drawBox(float w, float h, float d) {
    float hw = w / 2.0f;
    float hh = h / 2.0f;
    float hd = d / 2.0f;

    glBegin(GL_QUADS);
    // Front
    glNormal3f(0, 0, 1); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, hd);
    // Back
    glNormal3f(0, 0, -1); glTexCoord2f(0, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(-hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(hw, hh, -hd);
    // Left
    glNormal3f(-1, 0, 0); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(-hw, -hh, hd); glTexCoord2f(1, 1); glVertex3f(-hw, hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);
    // Right
    glNormal3f(1, 0, 0); glTexCoord2f(0, 0); glVertex3f(hw, -hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(hw, hh, hd);
    // Top
    glNormal3f(0, 1, 0); glTexCoord2f(0, 0); glVertex3f(-hw, hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, hh, hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);
    // Bottom
    glNormal3f(0, -1, 0); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(hw, -hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, -hh, hd);
    glEnd();
}