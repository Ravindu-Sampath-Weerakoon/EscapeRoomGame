#include "pch.h"
#include "RoomDecorations.h"
#include "GraphicsUtils.h" 
#include <math.h>
#include <stdio.h>
#include <SOIL2.h>


// PI constant for round calculations
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

RoomDecorations::RoomDecorations()
    : m_texWood(0), m_texMetal(0)
{
}

void RoomDecorations::addDecoration(int type, float x, float z, float rotation) {
    DecorInstance d;
    d.type = type;
    d.x = x;
    d.z = z;
    d.rotation = rotation;
    m_objects.push_back(d);

    printf("Decoration (Type %d) added at (%.1f, %.1f).\n", type, x, z);
}

void RoomDecorations::loadTextures(const char* woodTex, const char* metalTex) {
    m_texWood = loadTexture(woodTex);
    m_texMetal = loadTexture(metalTex);
}

GLuint RoomDecorations::loadTexture(const char* path) {
    GLuint id = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
    return id;
}

void RoomDecorations::draw() {
    glColor3f(1.0f, 1.0f, 1.0f);

    for (const auto& obj : m_objects) {
        switch (obj.type) {
        case 1: drawChair(obj.x, obj.z, obj.rotation); break;
        case 2: drawTable(obj.x, obj.z, obj.rotation); break;
        case 3: drawCupboard(obj.x, obj.z, obj.rotation); break;
        case 4: drawBed(obj.x, obj.z, obj.rotation); break;
        case 5: drawRack(obj.x, obj.z, obj.rotation); break;
        }
    }
    glDisable(GL_TEXTURE_2D);
}

// =============================================================
// OBJECT DRAWING FUNCTIONS
// =============================================================

// 1. BEAUTIFUL HIGH-BACK CHAIR
void RoomDecorations::drawChair(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    // Frame Texture
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.4f, 0.2f, 0.1f); }

    float seatH = 0.5f; float seatW = 0.6f; float seatD = 0.6f; float legThick = 0.07f;

    // Legs
    glPushMatrix(); glTranslatef(-seatW / 2 + 0.05f, seatH / 2, seatD / 2 - 0.05f); drawBox(legThick, seatH, legThick); glPopMatrix();
    glPushMatrix(); glTranslatef(seatW / 2 - 0.05f, seatH / 2, seatD / 2 - 0.05f); drawBox(legThick, seatH, legThick); glPopMatrix();
    float backH = 1.4f;
    glPushMatrix(); glTranslatef(-seatW / 2 + 0.05f, backH / 2, -seatD / 2 + 0.05f); drawBox(legThick, backH, legThick); glPopMatrix();
    glPushMatrix(); glTranslatef(seatW / 2 - 0.05f, backH / 2, -seatD / 2 + 0.05f); drawBox(legThick, backH, legThick); glPopMatrix();

    // Seat Base
    glPushMatrix(); glTranslatef(0, seatH, 0); drawBox(seatW, 0.08f, seatD); glPopMatrix();

    // Cushion
    glDisable(GL_TEXTURE_2D); glColor3f(0.6f, 0.0f, 0.0f);
    glPushMatrix(); glTranslatef(0, seatH + 0.07f, 0); drawBox(seatW - 0.05f, 0.06f, seatD - 0.05f); glPopMatrix();

    // Backrest
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glColor3f(1, 1, 1); }
    else glColor3f(0.4f, 0.2f, 0.1f);

    glPushMatrix(); glTranslatef(0, backH - 0.05f, -seatD / 2 + 0.05f); drawBox(seatW, 0.15f, 0.05f); glPopMatrix();
    glPushMatrix(); glTranslatef(0, seatH + 0.4f, -seatD / 2 + 0.05f); drawBox(seatW - 0.1f, 0.1f, 0.04f); glPopMatrix();
    glPushMatrix(); glTranslatef(0, seatH + 0.45f, -seatD / 2 + 0.05f); drawBox(0.15f, 0.9f, 0.04f); glPopMatrix();

    glPopMatrix();
}

// 2. ROUND TABLE WITH TEAPOT
void RoomDecorations::drawTable(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.5f, 0.3f, 0.1f); }

    float tableH = 0.8f; float radius = 0.8f;

    // Pedestal
    glPushMatrix(); glTranslatef(0, 0.05f, 0); drawCylinder(0.3f, 0.1f, 16); glPopMatrix();
    glPushMatrix(); glTranslatef(0, tableH / 2, 0); drawCylinder(0.12f, tableH, 12); glPopMatrix();

    // Top
    glPushMatrix(); glTranslatef(0, tableH, 0); drawCylinder(radius, 0.08f, 32); glPopMatrix();

    // Teapot
    glDisable(GL_TEXTURE_2D); glColor3f(0.9f, 0.9f, 0.9f);
    glPushMatrix(); glTranslatef(0.0f, tableH + 0.15f, 0.0f); glutSolidTeapot(0.15); glPopMatrix();

    glPopMatrix();
}

// 3. IMPROVED CUPBOARD (Wardrobe Style)
void RoomDecorations::drawCupboard(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    // Dimensions
    float w = 1.4f;
    float h = 2.2f;
    float d = 0.6f;
    float baseH = 0.15f; // Kickplate height

    // --- Main Body (Wood) ---
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.4f, 0.25f, 0.1f); }

    // 1. Base (Kickplate) - Recessed slightly
    glPushMatrix();
    glTranslatef(0, baseH / 2, 0);
    drawBox(w - 0.1f, baseH, d - 0.1f);
    glPopMatrix();

    // 2. Main Frame Box
    glPushMatrix();
    glTranslatef(0, (h + baseH) / 2, 0);
    drawBox(w, h - baseH, d);
    glPopMatrix();

    // 3. Top Cornice (Molding) - Stick out wider
    glPushMatrix();
    glTranslatef(0, h + 0.05f, 0);
    drawBox(w + 0.2f, 0.1f, d + 0.1f);
    glPopMatrix();

    // --- Doors ---
    // Make doors slightly lighter or same wood
    if (!m_texWood) glColor3f(0.45f, 0.3f, 0.15f);

    float doorW = w / 2.0f - 0.05f;
    float doorH = h - baseH - 0.2f;
    float doorThick = 0.05f;

    // Left Door
    glPushMatrix();
    glTranslatef(-w / 4, (h + baseH) / 2, d / 2 + 0.02f);
    drawBox(doorW, doorH, doorThick);
    glPopMatrix();

    // Right Door
    glPushMatrix();
    glTranslatef(w / 4, (h + baseH) / 2, d / 2 + 0.02f);
    drawBox(doorW, doorH, doorThick);
    glPopMatrix();

    // --- Detail: Handles (Gold/Brass knobs) ---
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.8f, 0.7f, 0.2f); // Gold color

    // Left Handle
    glPushMatrix();
    glTranslatef(-0.05f, h / 2 + 0.2f, d / 2 + 0.06f);
    glutSolidSphere(0.04, 10, 10);
    glPopMatrix();

    // Right Handle
    glPushMatrix();
    glTranslatef(0.05f, h / 2 + 0.2f, d / 2 + 0.06f);
    glutSolidSphere(0.04, 10, 10);
    glPopMatrix();

    glPopMatrix();
}

// 4. IMPROVED BED (Four Poster Style)
void RoomDecorations::drawBed(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    float bedW = 1.4f;
    float bedL = 2.2f;
    float legH_Head = 1.2f; // Headboard posts
    float legH_Foot = 0.7f; // Footboard posts
    float mattressH = 0.45f;

    // --- Wood Frame ---
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.4f, 0.2f, 0.1f); }

    // 1. Four Corner Posts (Cylinders)
    // Head Left
    glPushMatrix(); glTranslatef(-bedW / 2, legH_Head / 2, -bedL / 2); drawCylinder(0.06f, legH_Head, 12); glPopMatrix();
    // Head Right
    glPushMatrix(); glTranslatef(bedW / 2, legH_Head / 2, -bedL / 2); drawCylinder(0.06f, legH_Head, 12); glPopMatrix();
    // Foot Left
    glPushMatrix(); glTranslatef(-bedW / 2, legH_Foot / 2, bedL / 2); drawCylinder(0.06f, legH_Foot, 12); glPopMatrix();
    // Foot Right
    glPushMatrix(); glTranslatef(bedW / 2, legH_Foot / 2, bedL / 2); drawCylinder(0.06f, legH_Foot, 12); glPopMatrix();

    // 2. Side Rails & Base
    glPushMatrix(); glTranslatef(0, 0.25f, 0); drawBox(bedW, 0.15f, bedL); glPopMatrix();

    // 3. Headboard Panel
    glPushMatrix(); glTranslatef(0, 0.7f, -bedL / 2); drawBox(bedW - 0.1f, 0.6f, 0.05f); glPopMatrix();

    // 4. Footboard Panel
    glPushMatrix(); glTranslatef(0, 0.45f, bedL / 2); drawBox(bedW - 0.1f, 0.3f, 0.05f); glPopMatrix();

    // --- Mattress (White Cloth) ---
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.95f, 0.95f, 0.9f); // Off white
    glPushMatrix();
    glTranslatef(0, 0.45f, 0); // Sit on top of base
    drawBox(bedW - 0.15f, 0.25f, bedL - 0.15f);
    glPopMatrix();

    // --- Blanket (Blue/Cozy) ---
    glColor3f(0.3f, 0.4f, 0.7f); // Nice Blue
    glPushMatrix();
    glTranslatef(0, 0.46f, 0.5f); // Covering lower half
    drawBox(bedW - 0.12f, 0.26f, bedL / 2 - 0.1f);
    glPopMatrix();

    // --- Pillows (White) ---
    glColor3f(1.0f, 1.0f, 1.0f);
    // Left Pillow
    glPushMatrix();
    glTranslatef(-0.35f, 0.65f, -bedL / 2 + 0.3f);
    glRotatef(15, 1, 0, 0); // Tilt
    drawBox(0.5f, 0.15f, 0.3f);
    glPopMatrix();
    // Right Pillow
    glPushMatrix();
    glTranslatef(0.35f, 0.65f, -bedL / 2 + 0.3f);
    glRotatef(15, 1, 0, 0); // Tilt
    drawBox(0.5f, 0.15f, 0.3f);
    glPopMatrix();

    glPopMatrix();
}

// 5. IMPROVED RACK (Bookshelf Style)
void RoomDecorations::drawRack(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    float rackW = 1.4f;
    float rackH = 2.0f;
    float rackD = 0.5f;
    float radius = 0.04f;

    // --- Metal Frame ---
    if (m_texMetal) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texMetal); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.2f, 0.2f, 0.2f); } // Dark Grey Metal

    // 4 Legs (Cylinders look better for metal racks)
    glPushMatrix(); glTranslatef(-rackW / 2, rackH / 2, -rackD / 2); drawCylinder(radius, rackH, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(rackW / 2, rackH / 2, -rackD / 2); drawCylinder(radius, rackH, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(-rackW / 2, rackH / 2, rackD / 2); drawCylinder(radius, rackH, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(rackW / 2, rackH / 2, rackD / 2); drawCylinder(radius, rackH, 8); glPopMatrix();

    // --- Wooden Shelves ---
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.6f, 0.4f, 0.2f); }

    float shelfY[] = { 0.2f, 0.8f, 1.4f, 1.9f }; // 4 Shelves
    for (int i = 0; i < 4; i++) {
        glPushMatrix();
        glTranslatef(0, shelfY[i], 0);
        drawBox(rackW + 0.1f, 0.05f, rackD + 0.05f);
        glPopMatrix();
    }

    // --- Decoration: BOOKS on the shelves ---
    glDisable(GL_TEXTURE_2D);

    // Row of books on second shelf
    float startX = -rackW / 2 + 0.2f;
    for (int i = 0; i < 6; i++) {
        // Randomize color slightly
        if (i % 3 == 0) glColor3f(0.7f, 0.1f, 0.1f); // Red
        else if (i % 3 == 1) glColor3f(0.1f, 0.4f, 0.1f); // Green
        else glColor3f(0.2f, 0.2f, 0.6f); // Blue

        float bookH = 0.35f + (i % 2) * 0.05f; // Vary height
        glPushMatrix();
        glTranslatef(startX + (i * 0.12f), shelfY[1] + bookH / 2 + 0.025f, 0);
        drawBox(0.08f, bookH, rackD - 0.1f);
        glPopMatrix();
    }

    // A stack of books on third shelf
    glColor3f(0.8f, 0.8f, 0.2f); // Yellow book
    glPushMatrix(); glTranslatef(0.3f, shelfY[2] + 0.05f, 0); drawBox(0.4f, 0.08f, 0.3f); glPopMatrix();
    glColor3f(0.5f, 0.1f, 0.5f); // Purple book
    glPushMatrix(); glTranslatef(0.3f, shelfY[2] + 0.13f, 0); drawBox(0.35f, 0.08f, 0.28f); glPopMatrix();

    glPopMatrix();
}

// ---------------------------------------------------
// HELPERS
// ---------------------------------------------------

// Helper: Draws a cylinder (or disk) standing upright
void RoomDecorations::drawCylinder(float radius, float height, int segments) {
    float halfH = height / 2.0f;
    float angleStep = 2.0f * M_PI / segments;

    // Draw Top Cap (Disk)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, 1, 0); glTexCoord2f(0.5f, 0.5f); glVertex3f(0, halfH, 0);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float u = 0.5f + (cos(angle) * 0.5f);
        float v = 0.5f + (sin(angle) * 0.5f);
        glTexCoord2f(u, v);
        glVertex3f(x, halfH, z);
    }
    glEnd();

    // Draw Bottom Cap (Disk)
    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0, -1, 0); glTexCoord2f(0.5f, 0.5f); glVertex3f(0, -halfH, 0);
    for (int i = 0; i <= segments; i++) {
        float angle = -i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;
        float u = 0.5f + (cos(angle) * 0.5f);
        float v = 0.5f + (sin(angle) * 0.5f);
        glTexCoord2f(u, v);
        glVertex3f(x, -halfH, z);
    }
    glEnd();

    // Draw Sides (Rectangle Strip)
    glBegin(GL_QUAD_STRIP);
    for (int i = 0; i <= segments; i++) {
        float angle = i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        glNormal3f(x / radius, 0, z / radius); // Normal points outward
        glTexCoord2f((float)i / segments, 1.0f); glVertex3f(x, halfH, z);
        glTexCoord2f((float)i / segments, 0.0f); glVertex3f(x, -halfH, z);
    }
    glEnd();
}

// Helper: Standard Box
void RoomDecorations::drawBox(float w, float h, float d) {
    float hw = w / 2.0f; float hh = h / 2.0f; float hd = d / 2.0f;
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, hd);
    glNormal3f(0, 0, -1); glTexCoord2f(0, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(-hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(hw, hh, -hd);
    glNormal3f(-1, 0, 0); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(-hw, -hh, hd); glTexCoord2f(1, 1); glVertex3f(-hw, hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);
    glNormal3f(1, 0, 0); glTexCoord2f(0, 0); glVertex3f(hw, -hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(hw, hh, hd);
    glNormal3f(0, 1, 0); glTexCoord2f(0, 0); glVertex3f(-hw, hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, hh, hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);
    glNormal3f(0, -1, 0); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(hw, -hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, -hh, hd);
    glEnd();
}