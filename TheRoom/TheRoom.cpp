#include "pch.h"
#include "TheRoom.h" 
#include <SOIL2.h>
#include <stdio.h>
#include <vector>
#include <math.h>

// ================================================================
// CORE CLASS IMPLEMENTATIONS
// ================================================================

// Constructor: Initialize variables including the new Display List ID
TheRoom::TheRoom(float width, float height, float depth)
    : m_width(width), m_height(height), m_depth(depth),
    m_texFloor(0), m_texWall(0), m_texCeiling(0),
    m_displayListID(0) // <--- Initialize to 0 (No list yet)
{
    printf("TheRoom created: W=%.2f, H=%.2f, D=%.2f\n", width, height, depth);
}

// Destructor: Clean up the Display List from GPU memory
TheRoom::~TheRoom() {
    if (m_displayListID != 0) {
        glDeleteLists(m_displayListID, 1);
        printf("TheRoom: Display List %u deleted.\n", m_displayListID);
    }
}

// Function to load a single texture using SOIL2
GLuint TheRoom::loadSingleTexture(const char* path) {
    if (!path) return 0;

    GLuint textureID = SOIL_load_OGL_texture(
        path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS
    );
    if (!textureID) { printf("SOIL2 loading error for '%s': %s\n", path, SOIL_last_result()); return 0; }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glBindTexture(GL_TEXTURE_2D, 0);

    printf("SOIL2 loaded texture '%s' successfully (ID: %u)\n", path, textureID);
    return textureID;
}

// Function to load all textures for the room
bool TheRoom::loadTextures(const char* floorTexPath, const char* wallTexPath, const char* ceilingTexPath) {
    printf("Loading room textures...\n");
    m_texFloor = loadSingleTexture(floorTexPath);
    m_texWall = loadSingleTexture(wallTexPath);
    m_texCeiling = loadSingleTexture(ceilingTexPath);
    return m_texFloor != 0 && m_texWall != 0 && m_texCeiling != 0;
}

// ================================================================
// NEW: Build the Display List (The Optimization)
// ================================================================
void TheRoom::build() {
    // 1. Clean up old list if it exists
    if (m_displayListID != 0) {
        glDeleteLists(m_displayListID, 1);
    }

    // 2. Generate a new unique ID
    m_displayListID = glGenLists(1);

    // 3. Start Recording
    glNewList(m_displayListID, GL_COMPILE);

    // 4. Set global state for the room
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // 5. Call the drawing functions (These commands are now stored on the GPU)
    drawFloor();
    drawWalls();
    drawCeiling();

    // 6. Stop Recording
    glEndList();

    printf("TheRoom: Optimized Display List created (ID: %u)\n", m_displayListID);
}

// ================================================================
// Master Draw Function (Now uses the list)
// ================================================================
void TheRoom::draw() {
    if (m_displayListID != 0) {
        // --- FAST PATH: Execute the recorded list ---
        glCallList(m_displayListID);
    }
    else {
        // --- SLOW PATH: Fallback if build() wasn't called ---
        glColor3f(1.0f, 1.0f, 1.0f);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        drawFloor();
        drawWalls();
        drawCeiling();
    }
}

// ================================================================
// Internal Helper Functions
// ================================================================

void TheRoom::bindAndCheckTexture(GLuint textureID) {
    if (textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f);
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 0.0f, 1.0f); // Fallback: Pink
    }
}

void TheRoom::unbindAndRestore() {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
}


// ================================================================
// Geometry Drawing Functions (Unchanged logic, but called by List now)
// ================================================================

void TheRoom::drawFloor() {
    float halfW = m_width / 2.0f;
    float halfD = m_depth / 2.0f;
    float floorRepeat = m_width / (m_width / 4.0f);

    bindAndCheckTexture(m_texFloor);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);          glVertex3f(-halfW, 0.0f, -halfD);
    glTexCoord2f(floorRepeat, 0.0f);   glVertex3f(halfW, 0.0f, -halfD);
    glTexCoord2f(floorRepeat, floorRepeat); glVertex3f(halfW, 0.0f, halfD);
    glTexCoord2f(0.0f, floorRepeat);   glVertex3f(-halfW, 0.0f, halfD);
    glEnd();
    unbindAndRestore();
}

void TheRoom::drawWalls() {
    float halfW = m_width / 2.0f;
    float roomH = m_height;
    float halfD = m_depth / 2.0f;
    float wallRepeatU = m_width / 24.0f;
    float wallRepeatV = m_height / 24.0f;

    bindAndCheckTexture(m_texWall);
    glBegin(GL_QUADS);

    // Left Wall
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);              glVertex3f(-halfW, 0.0f, halfD);
    glTexCoord2f(wallRepeatU, 0.0f);        glVertex3f(-halfW, 0.0f, -halfD);
    glTexCoord2f(wallRepeatU, wallRepeatV); glVertex3f(-halfW, roomH, -halfD);
    glTexCoord2f(0.0f, wallRepeatV);        glVertex3f(-halfW, roomH, halfD);

    // Right Wall
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);              glVertex3f(halfW, 0.0f, -halfD);
    glTexCoord2f(wallRepeatU, 0.0f);        glVertex3f(halfW, 0.0f, halfD);
    glTexCoord2f(wallRepeatU, wallRepeatV); glVertex3f(halfW, roomH, halfD);
    glTexCoord2f(0.0f, wallRepeatV);        glVertex3f(halfW, roomH, -halfD);

    // Back Wall
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);              glVertex3f(-halfW, 0.0f, halfD);
    glTexCoord2f(wallRepeatU, 0.0f);        glVertex3f(halfW, 0.0f, halfD);
    glTexCoord2f(wallRepeatU, wallRepeatV); glVertex3f(halfW, roomH, halfD);
    glTexCoord2f(0.0f, wallRepeatV);        glVertex3f(-halfW, roomH, halfD);

    // Front Wall
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);              glVertex3f(halfW, 0.0f, -halfD);
    glTexCoord2f(wallRepeatU, 0.0f);        glVertex3f(-halfW, 0.0f, -halfD);
    glTexCoord2f(wallRepeatU, wallRepeatV); glVertex3f(-halfW, roomH, -halfD);
    glTexCoord2f(0.0f, wallRepeatV);        glVertex3f(halfW, roomH, -halfD);

    glEnd();
    unbindAndRestore();
}

void TheRoom::drawCeiling() {
    float halfW = m_width / 2.0f;
    float roomH = m_height;
    float halfD = m_depth / 2.0f;
    float ceilRepeat = m_width / (m_width / 1.0f);

    bindAndCheckTexture(m_texCeiling);
    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, ceilRepeat);         glVertex3f(-halfW, roomH, -halfD);
    glTexCoord2f(ceilRepeat, ceilRepeat);   glVertex3f(halfW, roomH, -halfD);
    glTexCoord2f(ceilRepeat, 0.0f);         glVertex3f(halfW, roomH, halfD);
    glTexCoord2f(0.0f, 0.0f);               glVertex3f(-halfW, roomH, halfD);
    glEnd();
    unbindAndRestore();
}