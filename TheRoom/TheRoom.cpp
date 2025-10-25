#include "pch.h"
#include "TheRoom.h" 
#include <SOIL2.h>
#include <stdio.h>
#include <vector>
#include <math.h>

// Note: loadSingleTexture and loadTextures implementations are assumed to be correct.

// ================================================================
// CORE CLASS IMPLEMENTATIONS
// ================================================================

TheRoom::TheRoom(float width, float height, float depth)
    : m_width(width), m_height(height), m_depth(depth),
    m_texFloor(0), m_texWall(0), m_texCeiling(0) {
    printf("TheRoom created: W=%.2f, H=%.2f, D=%.2f\n", width, height, depth);
}

// Function to load a single texture using SOIL2 (Assumed Correct)
GLuint TheRoom::loadSingleTexture(const char* path) {
    if (!path) return 0;
    // ... (full SOIL2 loading logic) ...
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

// Function to load all textures for the room (Assumed Correct)
bool TheRoom::loadTextures(const char* floorTexPath, const char* wallTexPath, const char* ceilingTexPath) {
    printf("Loading room textures...\n");
    m_texFloor = loadSingleTexture(floorTexPath);
    m_texWall = loadSingleTexture(wallTexPath);
    m_texCeiling = loadSingleTexture(ceilingTexPath);
    return m_texFloor != 0 && m_texWall != 0 && m_texCeiling != 0;
}


// ================================================================
// Draw Helper: Texture Binding and Color Check
// ================================================================

void TheRoom::bindAndCheckTexture(GLuint textureID) {
    if (textureID != 0) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1.0f, 1.0f, 1.0f); // Set material color to white
    }
    else {
        glDisable(GL_TEXTURE_2D);
        glColor3f(1.0f, 0.0f, 1.0f); // Fallback: BRIGHT PINK ERROR COLOR
    }
}

void TheRoom::unbindAndRestore() {
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f); // Reset material color to white
}


// ================================================================
// Draw the Floor (Y = 0) - OPTIMIZED REPEAT
// ================================================================
void TheRoom::drawFloor() {
    float halfW = m_width / 2.0f;
    float halfD = m_depth / 2.0f;

    // --- OPTIMIZATION: Small Number of Repeats (2.0 times total) ---
    // Since m_width=40, this simplifies to 2.0. Best performance/quality trade-off.
    float floorRepeat = m_width / (m_width / 2.0f);
    // -----------------------------------------------------------------

    bindAndCheckTexture(m_texFloor);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, 1.0f, 0.0f); // Normal points UP (Inwards)

    // Floor Quad Vertices
    glTexCoord2f(0.0f, 0.0f);          glVertex3f(-halfW, 0.0f, -halfD); // Front Left
    glTexCoord2f(floorRepeat, 0.0f);   glVertex3f(halfW, 0.0f, -halfD); // Front Right
    glTexCoord2f(floorRepeat, floorRepeat); glVertex3f(halfW, 0.0f, halfD); // Back Right
    glTexCoord2f(0.0f, floorRepeat);   glVertex3f(-halfW, 0.0f, halfD); // Back Left

    glEnd();
    unbindAndRestore();
}

// ================================================================
// Draw the Walls - OPTIMIZED REPEAT (Assuming large divisors are desired)
// ================================================================
void TheRoom::drawWalls() {
    float halfW = m_width / 2.0f;
    float roomH = m_height;
    float halfD = m_depth / 2.0f;

    // --- WALLS: Using large divisor for performance (minimal repeats) ---
    // If m_width=40, U repeats 40/32=1.25 times. If m_height=5, V repeats 5/12=0.41 times.
    float wallRepeatU = m_width / 32.0f;
    float wallRepeatV = m_height / 24.0f;
    // -------------------------------------------------------------------

    bindAndCheckTexture(m_texWall);

    glBegin(GL_QUADS);

    // --- Left Wall (X = -halfW) ---
    glNormal3f(1.0f, 0.0f, 0.0f); // Normal points RIGHT (+X, Inwards)
    glTexCoord2f(0.0f, 0.0f);              glVertex3f(-halfW, 0.0f, halfD); // Bottom Back
    glTexCoord2f(wallRepeatU, 0.0f);        glVertex3f(-halfW, 0.0f, -halfD); // Bottom Front
    glTexCoord2f(wallRepeatU, wallRepeatV); glVertex3f(-halfW, roomH, -halfD); // Top Front
    glTexCoord2f(0.0f, wallRepeatV);        glVertex3f(-halfW, roomH, halfD); // Top Back

    // --- Right Wall (X = halfW) ---
    glNormal3f(-1.0f, 0.0f, 0.0f); // Normal points LEFT (-X, Inwards)
    glTexCoord2f(0.0f, 0.0f);              glVertex3f(halfW, 0.0f, -halfD); // Bottom Front
    glTexCoord2f(wallRepeatU, 0.0f);        glVertex3f(halfW, 0.0f, halfD); // Bottom Back
    glTexCoord2f(wallRepeatU, wallRepeatV); glVertex3f(halfW, roomH, halfD); // Top Back
    glTexCoord2f(0.0f, wallRepeatV);        glVertex3f(halfW, roomH, -halfD); // Top Front

    // --- Back Wall (Z = halfD) ---
    glNormal3f(0.0f, 0.0f, -1.0f); // Normal points FORWARD (-Z, Inwards)
    glTexCoord2f(0.0f, 0.0f);              glVertex3f(-halfW, 0.0f, halfD); // Bottom Left
    glTexCoord2f(wallRepeatU, 0.0f);        glVertex3f(halfW, 0.0f, halfD); // Bottom Right
    glTexCoord2f(wallRepeatU, wallRepeatV); glVertex3f(halfW, roomH, halfD); // Top Right
    glTexCoord2f(0.0f, wallRepeatV);        glVertex3f(-halfW, roomH, halfD); // Top Left

    // --- Front Wall (Z = -halfD) ---
    glNormal3f(0.0f, 0.0f, 1.0f); // Normal points BACKWARD (+Z, Inwards)
    glTexCoord2f(0.0f, 0.0f);              glVertex3f(halfW, 0.0f, -halfD); // Bottom Right
    glTexCoord2f(wallRepeatU, 0.0f);        glVertex3f(-halfW, 0.0f, -halfD); // Bottom Left
    glTexCoord2f(wallRepeatU, wallRepeatV); glVertex3f(-halfW, roomH, -halfD); // Top Left
    glTexCoord2f(0.0f, wallRepeatV);        glVertex3f(halfW, roomH, -halfD); // Top Right

    glEnd();
    unbindAndRestore();
}

// ================================================================
// Draw the Ceiling - OPTIMIZED REPEAT
// ================================================================
void TheRoom::drawCeiling() {
    float halfW = m_width / 2.0f;
    float roomH = m_height;
    float halfD = m_depth / 2.0f;

    // --- OPTIMIZATION: Small Number of Repeats (2.0 times total) ---
    float ceilRepeat = m_width / (m_width / 2.0f);

    bindAndCheckTexture(m_texCeiling);

    glBegin(GL_QUADS);
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal points DOWN (Inwards)

    // Ceiling Quad Vertices
    glTexCoord2f(0.0f, ceilRepeat);         glVertex3f(-halfW, roomH, -halfD); // Front Left
    glTexCoord2f(ceilRepeat, ceilRepeat);   glVertex3f(halfW, roomH, -halfD); // Front Right
    glTexCoord2f(ceilRepeat, 0.0f);         glVertex3f(halfW, roomH, halfD); // Back Right
    glTexCoord2f(0.0f, 0.0f);               glVertex3f(-halfW, roomH, halfD); // Back Left

    glEnd();

    unbindAndRestore();
}


// ================================================================
// Master Draw Function
// ================================================================
void TheRoom::draw() {
    // Set global drawing properties needed for all parts
    glColor3f(1.0f, 1.0f, 1.0f);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // Use modulate

    // Call individual drawing functions
    drawFloor();
    drawWalls();
    drawCeiling();
}