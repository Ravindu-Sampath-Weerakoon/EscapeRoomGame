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
            // Original 5 Objects
        case 1: drawChair(obj.x, obj.z, obj.rotation); break;
        case 2: drawTable(obj.x, obj.z, obj.rotation); break;
        case 3: drawCupboard(obj.x, obj.z, obj.rotation); break;
        case 4: drawBed(obj.x, obj.z, obj.rotation); break;
        case 5: drawRack(obj.x, obj.z, obj.rotation); break;

            // --- NEW OBJECTS (6-10) ---
        case 6: drawFloorLamp(obj.x, obj.z, obj.rotation); break;
        case 7: drawSofa(obj.x, obj.z, obj.rotation); break;
        case 8: drawTVUnit(obj.x, obj.z, obj.rotation); break;
        case 9: drawDesk(obj.x, obj.z, obj.rotation); break;
        case 10: drawPlant(obj.x, obj.z, obj.rotation); break;
        }
    }
    glDisable(GL_TEXTURE_2D);
}
// =============================================================
// OBJECT DRAWING FUNCTIONS
// =============================================================




void RoomDecorations::drawFloorLamp(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(1.5f, 1.5f, 1.5f); // Maintain consistent scale

    // ---------------------------------------------------
    // 1. THE ORNATE BASE (Stepped Design)
    // ---------------------------------------------------
    if (m_texMetal) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texMetal); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.2f, 0.2f, 0.2f); } // Dark Metal

    // Bottom wide plate
    glPushMatrix(); glTranslatef(0, 0.02f, 0); drawCylinder(0.30f, 0.04f, 24); glPopMatrix();
    // Middle medium plate
    glPushMatrix(); glTranslatef(0, 0.06f, 0); drawCylinder(0.22f, 0.04f, 24); glPopMatrix();
    // Top connector dome
    glPushMatrix(); glTranslatef(0, 0.10f, 0); glutSolidSphere(0.12f, 16, 16); glPopMatrix();

    // ---------------------------------------------------
    // 2. THE SEGMENTED POLE
    // ---------------------------------------------------
    // Lower Pole Section
    glPushMatrix(); glTranslatef(0, 0.5f, 0); drawCylinder(0.04f, 0.8f, 12); glPopMatrix();

    // Decorative Middle Knob (Sphere)
    glDisable(GL_TEXTURE_2D); glColor3f(0.3f, 0.3f, 0.3f); // Accent color (Darker)
    glPushMatrix(); glTranslatef(0, 0.9f, 0); glutSolidSphere(0.06f, 16, 16); glPopMatrix();

    // Upper Pole Section (re-enable texture)
    if (m_texMetal) { glEnable(GL_TEXTURE_2D); glColor3f(1, 1, 1); }
    else glColor3f(0.2f, 0.2f, 0.2f);

    glPushMatrix(); glTranslatef(0, 1.3f, 0); drawCylinder(0.03f, 0.8f, 12); glPopMatrix();

    // ---------------------------------------------------
    // 3. THE LIGHT BULB & INTERNALS
    // ---------------------------------------------------
    glDisable(GL_TEXTURE_2D);

    // Bulb Holder
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix(); glTranslatef(0, 1.65f, 0); drawCylinder(0.05f, 0.1f, 12); glPopMatrix();

    // The Light Bulb (Bright White/Yellow)
    glColor3f(1.0f, 1.0f, 0.8f);
    glPushMatrix(); glTranslatef(0, 1.72f, 0); glutSolidSphere(0.08f, 12, 12); glPopMatrix();

    // Pull Chain (Switch) - A thin line hanging down
    glColor3f(0.8f, 0.7f, 0.2f); // Gold chain
    glPushMatrix(); glTranslatef(0.08f, 1.6f, 0); drawCylinder(0.005f, 0.25f, 4); glPopMatrix();
    glPushMatrix(); glTranslatef(0.08f, 1.48f, 0); glutSolidSphere(0.015f, 8, 8); glPopMatrix();

    // ---------------------------------------------------
    // 4. THE FANCY SHADE
    // ---------------------------------------------------
    float shadeH = 0.55f;
    float shadeY = 1.75f;
    float shadeR = 0.40f;

    // Main Shade Body (Soft Cream / Fabric look)
    glColor3f(0.95f, 0.90f, 0.80f);
    glPushMatrix();
    glTranslatef(0, shadeY, 0);
    drawCylinder(shadeR, shadeH, 24);
    glPopMatrix();

    // Decorative Rims (Top and Bottom of shade - gives it a finished look)
    glColor3f(0.4f, 0.2f, 0.1f); // Dark Brown trim

    // Bottom Rim
    glPushMatrix();
    glTranslatef(0, shadeY - shadeH / 2 + 0.02f, 0);
    drawCylinder(shadeR + 0.01f, 0.04f, 24); // Slightly wider than shade
    glPopMatrix();

    // Top Rim
    glPushMatrix();
    glTranslatef(0, shadeY + shadeH / 2 - 0.02f, 0);
    drawCylinder(shadeR + 0.01f, 0.04f, 24);
    glPopMatrix();

    glPopMatrix();
}

void RoomDecorations::drawSofa(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    float sofaW = 2.4f;
    float sofaH = 0.45f;
    float sofaD = 0.85f;

    // =============================================================
    // 1. LEFT SIDE DESIGN: WOODEN END TABLE WITH COFFEE
    // =============================================================
    glPushMatrix();
    glTranslatef(-(sofaW / 2 + 0.5f), 0, 0);

    // -- Wooden Table Body --
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.5f, 0.3f, 0.1f); }

    // Table Top Box
    glPushMatrix(); glTranslatef(0, 0.55f, 0); drawBox(0.6f, 0.25f, 0.6f); glPopMatrix();

    // 4 Wooden Legs
    glPushMatrix(); glTranslatef(-0.25f, 0.21f, -0.25f); drawBox(0.06f, 0.42f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.25f, 0.21f, -0.25f); drawBox(0.06f, 0.42f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.25f, 0.21f, 0.25f); drawBox(0.06f, 0.42f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.25f, 0.21f, 0.25f); drawBox(0.06f, 0.42f, 0.06f); glPopMatrix();

    // -- Drawer Detail --
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.4f, 0.2f, 0.05f); // Darker wood for drawer outline
    glPushMatrix(); glTranslatef(0, 0.55f, 0.31f); drawBox(0.5f, 0.18f, 0.02f); glPopMatrix();
    // Silver Handle
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix(); glTranslatef(0, 0.55f, 0.33f); glutSolidSphere(0.03f, 8, 8); glPopMatrix();

    // -- Coffee Cup on Top --
    glColor3f(1.0f, 1.0f, 1.0f); // White China
    glPushMatrix(); glTranslatef(0, 0.75f, 0); glutSolidTeapot(0.08f); glPopMatrix();
    glPopMatrix();


    // =============================================================
    // 2. RIGHT SIDE DESIGN: WOODEN END TABLE WITH LAMP
    // =============================================================
    glPushMatrix();
    glTranslatef((sofaW / 2 + 0.5f), 0, 0);

    // -- Wooden Table Body --
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.5f, 0.3f, 0.1f); }

    glPushMatrix(); glTranslatef(0, 0.55f, 0); drawBox(0.6f, 0.25f, 0.6f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.25f, 0.21f, -0.25f); drawBox(0.06f, 0.42f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.25f, 0.21f, -0.25f); drawBox(0.06f, 0.42f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.25f, 0.21f, 0.25f); drawBox(0.06f, 0.42f, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(0.25f, 0.21f, 0.25f); drawBox(0.06f, 0.42f, 0.06f); glPopMatrix();

    // Drawer Detail
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.4f, 0.2f, 0.05f);
    glPushMatrix(); glTranslatef(0, 0.55f, 0.31f); drawBox(0.5f, 0.18f, 0.02f); glPopMatrix();
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix(); glTranslatef(0, 0.55f, 0.33f); glutSolidSphere(0.03f, 8, 8); glPopMatrix();

    // -- Small Table Lamp --
    // Base
    glColor3f(0.2f, 0.2f, 0.2f); // Black Base
    glPushMatrix(); glTranslatef(0, 0.70f, 0); drawCylinder(0.12f, 0.05f, 12); glPopMatrix();
    // Pole
    glPushMatrix(); glTranslatef(0, 0.85f, 0); drawCylinder(0.02f, 0.3f, 8); glPopMatrix();
    // Shade (Square modern shade)
    glColor3f(0.9f, 0.9f, 0.8f); // Cream
    glPushMatrix(); glTranslatef(0, 1.0f, 0); drawBox(0.25f, 0.25f, 0.25f); glPopMatrix();

    glPopMatrix();


    // =============================================================
    // 3. LUXURY SOFA: DARK BLUE MIX WITH WOODEN BACK
    // =============================================================

    // --- Wooden Base Plinth & BACK DECORATION ---
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.4f, 0.2f, 0.1f); }

    // 1. Base Platform
    glPushMatrix(); glTranslatef(0, 0.15f, 0); drawBox(sofaW, 0.12f, sofaD); glPopMatrix();
    // Wooden Legs (Small block feet)
    glPushMatrix(); glTranslatef(-sofaW / 2 + 0.1f, 0.05f, sofaD / 2 - 0.1f); drawBox(0.1f, 0.1f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef(sofaW / 2 - 0.1f, 0.05f, sofaD / 2 - 0.1f); drawBox(0.1f, 0.1f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef(-sofaW / 2 + 0.1f, 0.05f, -sofaD / 2 + 0.1f); drawBox(0.1f, 0.1f, 0.1f); glPopMatrix();
    glPushMatrix(); glTranslatef(sofaW / 2 - 0.1f, 0.05f, -sofaD / 2 + 0.1f); drawBox(0.1f, 0.1f, 0.1f); glPopMatrix();

    // 2. NEW: Full Wooden Back Panel Decoration
    // Sits behind the upholstered section
    float backPanelH = 1.15f;
    glPushMatrix();
    glTranslatef(0, 0.2f + backPanelH / 2.0f, -sofaD / 2.0f - 0.03f);
    drawBox(sofaW, backPanelH, 0.05f);
    glPopMatrix();

    // 3. NEW: Top Wood Rail Cap (Detail on top of the back panel)
    glPushMatrix();
    glTranslatef(0, 0.2f + backPanelH, -sofaD / 2.0f - 0.01f);
    drawBox(sofaW + 0.05f, 0.08f, 0.12f);
    glPopMatrix();


    // --- UPHOLSTERY (Dark Blue Velvet) ---
    glDisable(GL_TEXTURE_2D); // Texture OFF for fabric
    glColor3f(0.05f, 0.08f, 0.25f); // Deep Indigo Base

    // Armrests (Rounded top)
    float armW = 0.28f;
    glPushMatrix(); glTranslatef(-sofaW / 2 + armW / 2, 0.65f, 0); drawBox(armW, 0.88f, sofaD); glPopMatrix();
    glPushMatrix(); glTranslatef(sofaW / 2 - armW / 2, 0.65f, 0); drawBox(armW, 0.88f, sofaD); glPopMatrix();

    // Backrest Frame (Upholstered part)
    glPushMatrix(); glTranslatef(0, 0.9f, -sofaD / 2 + 0.1f); drawBox(sofaW - 0.1f, 1.0f, 0.25f); glPopMatrix();

    // Base Seat cushion area (Unified under-seat)
    glPushMatrix(); glTranslatef(0, 0.35f, 0); drawBox(sofaW - 0.2f, 0.3f, sofaD - 0.1f); glPopMatrix();


    // --- DETAILED CUSHIONS (Lighter Blue Mix) ---
    float innerW = sofaW - (armW * 2);
    float cushionW = (innerW / 3.0f) - 0.02f;

    for (int i = -1; i <= 1; i++) {
        float xPos = i * (cushionW + 0.02f);

        // Seat Cushion (Plump)
        glColor3f(0.1f, 0.15f, 0.40f); // Richer Royal Blue
        glPushMatrix();
        glTranslatef(xPos, 0.52f, 0.05f);
        drawBox(cushionW, 0.20f, sofaD - 0.15f);
        glPopMatrix();

        // Back Cushion (Tufted)
        glPushMatrix();
        glTranslatef(xPos, 0.9f, -sofaD / 2 + 0.3f);
        glRotatef(-12, 1, 0, 0); // Tilt back
        drawBox(cushionW, 0.6f, 0.18f);

        // ** BUTTON DETAILS **
        glColor3f(0.02f, 0.02f, 0.2f); // Dark Navy Buttons
        for (int r = 0; r < 2; r++) {
            for (int c = -1; c <= 1; c++) {
                glPushMatrix();
                glTranslatef(c * 0.15f, (r * 0.2f) - 0.1f, 0.095f);
                glutSolidSphere(0.025f, 6, 6);
                glPopMatrix();
            }
        }
        glPopMatrix();
    }

    // (Blanket decoration removed here)

    glPopMatrix();
}

void RoomDecorations::drawTVUnit(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    float unitW = 1.6f;
    float unitH = 0.5f;
    float unitD = 0.5f;
    float legH = 0.2f;

    // --------------------------------------------------
    // 1. STYLISH LEGS (Angled/Tapered Look)
    // --------------------------------------------------
    // We use wood texture or dark brown color
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.4f, 0.25f, 0.1f); }

    float legInset = 0.1f;
    // Draw 4 legs slightly narrower than the body
    glPushMatrix(); glTranslatef(-unitW / 2 + legInset, legH / 2, -unitD / 2 + legInset); drawBox(0.06f, legH, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(unitW / 2 - legInset, legH / 2, -unitD / 2 + legInset); drawBox(0.06f, legH, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(-unitW / 2 + legInset, legH / 2, unitD / 2 - legInset); drawBox(0.06f, legH, 0.06f); glPopMatrix();
    glPushMatrix(); glTranslatef(unitW / 2 - legInset, legH / 2, unitD / 2 - legInset); drawBox(0.06f, legH, 0.06f); glPopMatrix();

    // --------------------------------------------------
    // 2. MAIN CABINET BODY
    // --------------------------------------------------
    glPushMatrix();
    glTranslatef(0, legH + unitH / 2, 0);
    drawBox(unitW, unitH, unitD);
    glPopMatrix();

    // --------------------------------------------------
    // 3. STORAGE DETAILS (Drawers & Open Shelf)
    // --------------------------------------------------
    glDisable(GL_TEXTURE_2D);

    // -- Left Drawer Face --
    glColor3f(1.0f, 1.0f, 1.0f); // White glossy accent or Lighter Wood
    if (m_texWood) glColor3f(0.9f, 0.9f, 0.9f); // Tint if textured

    glPushMatrix();
    glTranslatef(-unitW / 3.0f, legH + unitH / 2, unitD / 2 + 0.01f);
    drawBox(unitW / 3.0f - 0.05f, unitH - 0.1f, 0.02f);
    glPopMatrix();

    // -- Right Drawer Face --
    glPushMatrix();
    glTranslatef(unitW / 3.0f, legH + unitH / 2, unitD / 2 + 0.01f);
    drawBox(unitW / 3.0f - 0.05f, unitH - 0.1f, 0.02f);
    glPopMatrix();

    // -- Handles (Gold Knobs) --
    glColor3f(0.8f, 0.7f, 0.2f);
    glPushMatrix(); glTranslatef(-unitW / 3.0f, legH + unitH / 2, unitD / 2 + 0.03f); glutSolidSphere(0.03f, 8, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(unitW / 3.0f, legH + unitH / 2, unitD / 2 + 0.03f); glutSolidSphere(0.03f, 8, 8); glPopMatrix();

    // -- Center Open Shelf (Simulated by a dark box) --
    glColor3f(0.2f, 0.1f, 0.05f); // Dark shadow color
    glPushMatrix();
    glTranslatef(0, legH + unitH / 2, unitD / 2 + 0.005f);
    drawBox(unitW / 3.0f - 0.05f, unitH - 0.1f, 0.01f);
    glPopMatrix();

    // -- Media Player / Console inside the open shelf --
    glColor3f(0.1f, 0.1f, 0.1f); // Black box
    glPushMatrix(); glTranslatef(0, legH + 0.2f, unitD / 2 + 0.02f); drawBox(0.3f, 0.06f, 0.3f); glPopMatrix();

    // Green power light on console
    glColor3f(0.0f, 1.0f, 0.0f);
    glPushMatrix(); glTranslatef(0.12f, legH + 0.2f, unitD / 2 + 0.04f); glutSolidSphere(0.01f, 6, 6); glPopMatrix();


    // --------------------------------------------------
    // 4. AUDIO SPEAKERS (Left & Right Towers)
    // --------------------------------------------------
    float speakerH = 0.6f;
    float speakerW = 0.15f;
    float tableTopY = legH + unitH;

    glColor3f(0.15f, 0.15f, 0.15f); // Dark Grey housing

    // Left Speaker
    glPushMatrix(); glTranslatef(-unitW / 2 + 0.15f, tableTopY + speakerH / 2, 0); drawBox(speakerW, speakerH, 0.2f); glPopMatrix();
    // Right Speaker
    glPushMatrix(); glTranslatef(unitW / 2 - 0.15f, tableTopY + speakerH / 2, 0); drawBox(speakerW, speakerH, 0.2f); glPopMatrix();

    // Speaker Mesh/Cones (Lighter grey circles)
    glColor3f(0.3f, 0.3f, 0.3f);
    // Left cones
    glPushMatrix(); glTranslatef(-unitW / 2 + 0.15f, tableTopY + 0.45f, 0.105f); glutSolidSphere(0.05f, 8, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(-unitW / 2 + 0.15f, tableTopY + 0.25f, 0.105f); glutSolidSphere(0.05f, 8, 8); glPopMatrix();
    // Right cones
    glPushMatrix(); glTranslatef(unitW / 2 - 0.15f, tableTopY + 0.45f, 0.105f); glutSolidSphere(0.05f, 8, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(unitW / 2 - 0.15f, tableTopY + 0.25f, 0.105f); glutSolidSphere(0.05f, 8, 8); glPopMatrix();


    // --------------------------------------------------
    // 5. THE TV (Sleek Modern Design)
    // --------------------------------------------------
    float tvW = 1.1f;
    float tvH = 0.65f;

    // TV Stand/Neck
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix(); glTranslatef(0, tableTopY + 0.05f, 0); drawBox(0.3f, 0.1f, 0.15f); glPopMatrix();

    // TV Frame/Back
    glColor3f(0.05f, 0.05f, 0.05f);
    glPushMatrix(); glTranslatef(0, tableTopY + 0.1f + tvH / 2, 0); drawBox(tvW, tvH, 0.04f); glPopMatrix();

    // TV Screen (Glossy Reflection)
    glColor3f(0.05f, 0.05f, 0.15f); // Very Dark Blue
    glPushMatrix(); glTranslatef(0, tableTopY + 0.1f + tvH / 2, 0.025f); drawBox(tvW - 0.05f, tvH - 0.05f, 0.01f); glPopMatrix();

    // Red Standby Light
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix(); glTranslatef(tvW / 2 - 0.1f, tableTopY + 0.15f, 0.03f); glutSolidSphere(0.008f, 6, 6); glPopMatrix();

    glPopMatrix();
}

void RoomDecorations::drawDesk(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glRotatef(rot, 0.0f, 1.0f, 0.0f);
    glScalef(1.5f, 1.5f, 1.5f);

    float deskW = 1.4f;
    float deskD = 0.7f;
    float deskH = 0.75f;

    // --------------------------------------------------
    // 1. THE DESK STRUCTURE
    // --------------------------------------------------

    // --- Table Top (Thick Wood) ---
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.5f, 0.3f, 0.1f); }

    glPushMatrix(); glTranslatef(0, deskH, 0); drawBox(deskW, 0.08f, deskD); glPopMatrix();

    // --- Modesty Panel (Back Board) ---
    // Connects left legs to right cabinet
    glPushMatrix();
    glTranslatef(0, deskH / 2 + 0.1f, -deskD / 2 + 0.05f);
    drawBox(deskW - 0.2f, deskH - 0.2f, 0.02f);
    glPopMatrix();

    // --- Left Side: Metal Legs ---
    if (m_texMetal) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texMetal); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.3f, 0.3f, 0.3f); }

    glPushMatrix(); glTranslatef(-deskW / 2 + 0.1f, deskH / 2, -deskD / 2 + 0.1f); drawCylinder(0.04f, deskH, 8); glPopMatrix();
    glPushMatrix(); glTranslatef(-deskW / 2 + 0.1f, deskH / 2, deskD / 2 - 0.1f); drawCylinder(0.04f, deskH, 8); glPopMatrix();

    // --- Right Side: Drawer Cabinet ---
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.5f, 0.3f, 0.1f); }

    // Main Cabinet Box
    float cabW = 0.45f;
    glPushMatrix();
    glTranslatef(deskW / 2 - 0.25f, deskH / 2, 0);
    drawBox(cabW, deskH, deskD - 0.05f);
    glPopMatrix();

    // --------------------------------------------------
    // 2. DETAILED DRAWERS
    // --------------------------------------------------
    // We draw 3 distinct drawer faces slightly popping out

    glDisable(GL_TEXTURE_2D); // Use solid color for detailing
    float drawerH = (deskH - 0.1f) / 3.0f;

    for (int i = 0; i < 3; i++) {
        float yPos = (deskH - 0.1f) - (i * drawerH) - (drawerH / 2);

        // Drawer Face (Slightly lighter wood/contrast)
        if (m_texWood) glColor3f(0.9f, 0.9f, 0.9f); // Tint existing texture if enabled
        else glColor3f(0.55f, 0.35f, 0.15f);

        glPushMatrix();
        glTranslatef(deskW / 2 - 0.25f, yPos, deskD / 2 - 0.02f); // Pop out forward
        drawBox(cabW - 0.04f, drawerH - 0.02f, 0.04f);
        glPopMatrix();

        // Handle (Silver)
        glColor3f(0.8f, 0.8f, 0.8f);
        glPushMatrix();
        glTranslatef(deskW / 2 - 0.25f, yPos, deskD / 2 + 0.01f);
        drawBox(0.15f, 0.02f, 0.02f);
        glPopMatrix();
    }

    // --------------------------------------------------
    // 3. DESK PROPS (Computer & Clutter)
    // --------------------------------------------------

    // --- Desk Mat (Black Leather) ---
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0, deskH + 0.041f, 0.1f);
    drawBox(0.7f, 0.005f, 0.35f);
    glPopMatrix();

    // --- Monitor Stand ---
    glColor3f(0.2f, 0.2f, 0.2f); // Dark Grey
    glPushMatrix(); glTranslatef(0, deskH + 0.05f, -0.15f); drawBox(0.2f, 0.02f, 0.15f); glPopMatrix(); // Base
    glPushMatrix(); glTranslatef(0, deskH + 0.2f, -0.2f); drawBox(0.05f, 0.3f, 0.02f); glPopMatrix();   // Neck

    // --- Monitor Screen ---
    // Bezel
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix(); glTranslatef(0, deskH + 0.35f, -0.18f); drawBox(0.8f, 0.45f, 0.03f); glPopMatrix();
    // Screen Area (Blueish reflection)
    glColor3f(0.1f, 0.15f, 0.25f);
    glPushMatrix(); glTranslatef(0, deskH + 0.35f, -0.165f); drawBox(0.75f, 0.4f, 0.01f); glPopMatrix();

    // --- Keyboard ---
    glColor3f(0.2f, 0.2f, 0.2f);
    glPushMatrix();
    glTranslatef(0, deskH + 0.05f, 0.1f);
    drawBox(0.5f, 0.02f, 0.18f);
    glPopMatrix();

    // --- Mouse ---
    glColor3f(0.1f, 0.1f, 0.1f);
    glPushMatrix();
    glTranslatef(0.35f, deskH + 0.05f, 0.1f);
    glScalef(1, 0.6f, 1);
    glutSolidSphere(0.04f, 10, 10);
    glPopMatrix();

    // --- Stack of Papers (Messy) ---
    glColor3f(0.95f, 0.95f, 0.95f); // White paper
    glPushMatrix(); glTranslatef(-0.5f, deskH + 0.045f, 0.1f);  glRotatef(10, 0, 1, 0); drawBox(0.21f, 0.01f, 0.3f); glPopMatrix();
    glPushMatrix(); glTranslatef(-0.5f, deskH + 0.055f, 0.1f);  glRotatef(-5, 0, 1, 0); drawBox(0.21f, 0.01f, 0.3f); glPopMatrix();

    glPopMatrix();
}

void RoomDecorations::drawPlant(float x, float z, float rot) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);
    glScalef(1.5f, 1.5f, 1.5f);

    // --------------------------------------------------
    // 1. THE POT (Modern Ceramic Look)
    // --------------------------------------------------
    glDisable(GL_TEXTURE_2D);

    // -- Saucer (Base plate) --
    glColor3f(0.8f, 0.8f, 0.8f); // White/Light Grey Ceramic
    glPushMatrix();
    glTranslatef(0, 0.02f, 0);
    drawCylinder(0.32f, 0.04f, 16);
    glPopMatrix();

    // -- Main Pot Body --
    glColor3f(0.7f, 0.3f, 0.1f); // Terracotta or Dark Orange
    glPushMatrix();
    glTranslatef(0, 0.25f, 0);
    drawCylinder(0.28f, 0.45f, 16);
    glPopMatrix();

    // -- Pot Rim (Top detail) --
    glColor3f(0.8f, 0.4f, 0.2f); // Slightly lighter rim
    glPushMatrix();
    glTranslatef(0, 0.48f, 0);
    drawCylinder(0.32f, 0.08f, 16);
    glPopMatrix();

    // -- Soil (Dark Earth) --
    glColor3f(0.15f, 0.1f, 0.05f); // Very dark brown
    glPushMatrix();
    glTranslatef(0, 0.45f, 0);
    drawCylinder(0.26f, 0.02f, 12);
    glPopMatrix();

    // --------------------------------------------------
    // 2. THE TRUNK (Wood Texture or Brown Color)
    // --------------------------------------------------
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.4f, 0.3f, 0.2f); }

    // Main central trunk
    glPushMatrix();
    glTranslatef(0, 0.8f, 0);
    drawCylinder(0.05f, 0.8f, 8);
    glPopMatrix();

    // --------------------------------------------------
    // 3. BRANCHES & FOLIAGE CLUSTERS
    // --------------------------------------------------
    // We will draw 3 branches coming off the main trunk
    // Each branch has a "Cluster" of green spheres to look like a bush

    glDisable(GL_TEXTURE_2D); // Leaves are just green color

    // --- Helper to draw a leaf cluster ---
    auto drawLeafCluster = []() {
        glColor3f(0.1f, 0.5f, 0.1f); // Dark Green
        glutSolidSphere(0.20f, 8, 8); // Center

        glColor3f(0.15f, 0.6f, 0.15f); // Lighter Green for outer leaves
        glPushMatrix(); glTranslatef(0.15f, 0.1f, 0); glutSolidSphere(0.15f, 8, 8); glPopMatrix();
        glPushMatrix(); glTranslatef(-0.15f, 0.05f, 0.1f); glutSolidSphere(0.15f, 8, 8); glPopMatrix();
        glPushMatrix(); glTranslatef(0, 0.15f, -0.15f); glutSolidSphere(0.15f, 8, 8); glPopMatrix();
        glPushMatrix(); glTranslatef(0, -0.1f, 0.15f); glutSolidSphere(0.14f, 8, 8); glPopMatrix();
        };

    // --- Branch 1 (Right) ---
    glPushMatrix();
    glTranslatef(0, 1.0f, 0); // Start higher up trunk
    glRotatef(-30, 0, 0, 1);  // Tilt Right

    // Draw Branch Stem
    if (m_texWood) glEnable(GL_TEXTURE_2D); else glColor3f(0.4f, 0.3f, 0.2f);
    glPushMatrix(); glTranslatef(0, 0.3f, 0); drawCylinder(0.03f, 0.6f, 6); glPopMatrix();

    // Draw Leaves at tip
    glDisable(GL_TEXTURE_2D);
    glTranslatef(0, 0.6f, 0);
    drawLeafCluster();
    glPopMatrix();

    // --- Branch 2 (Left) ---
    glPushMatrix();
    glTranslatef(0, 0.9f, 0);
    glRotatef(45, 0, 0, 1);   // Tilt Left

    // Draw Branch Stem
    if (m_texWood) glEnable(GL_TEXTURE_2D); else glColor3f(0.4f, 0.3f, 0.2f);
    glPushMatrix(); glTranslatef(0, 0.25f, 0); drawCylinder(0.03f, 0.5f, 6); glPopMatrix();

    // Draw Leaves at tip
    glDisable(GL_TEXTURE_2D);
    glTranslatef(0, 0.5f, 0);
    drawLeafCluster();
    glPopMatrix();

    // --- Branch 3 (Top / Back) ---
    glPushMatrix();
    glTranslatef(0, 1.1f, 0);
    glRotatef(20, 1, 0, 0);   // Tilt Back

    // Draw Branch Stem
    if (m_texWood) glEnable(GL_TEXTURE_2D); else glColor3f(0.4f, 0.3f, 0.2f);
    glPushMatrix(); glTranslatef(0, 0.2f, 0); drawCylinder(0.03f, 0.4f, 6); glPopMatrix();

    // Draw Leaves at tip
    glDisable(GL_TEXTURE_2D);
    glTranslatef(0, 0.4f, 0);
    drawLeafCluster();
    glPopMatrix();

    glPopMatrix();
}


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