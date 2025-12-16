#include "pch.h"
#include "SecretDoor.h"
#include "GraphicsUtils.h" // Needed for grid functions
#include <math.h>
#include <stdio.h>
#include <SOIL2.h>

SecretDoor::SecretDoor()
    : m_interactionRange(2.5f), m_texFrame(0), m_texDoor(0), m_texDetail(0)
{
}

void SecretDoor::addDoor(float x, float z, int direction, const char* pin) {
    DoorData d;
    d.x = x;
    d.z = z;
    d.direction = direction;
    d.pinCode = pin;
    d.isOpen = false;
    d.openAngle = 0.0f;

    m_doors.push_back(d);

    // Immediately block the grid for this new closed door
    updateCollision((int)m_doors.size() - 1, true);
}

void SecretDoor::updateCollision(int index, bool isClosed) {
    if (index < 0 || index >= m_doors.size()) return;

    DoorData& d = m_doors[index];

    // Grid Logic (4 Units Wide):
    // Left/Top Post: ALWAYS BLOCKED
    // Left/Top Door: Blocked if Closed
    // Right/Bot Door: Blocked if Closed
    // Right/Bot Post: ALWAYS BLOCKED

    int centerX, centerZ;
    if (!worldToGrid(d.x - 0.1f, d.z - 0.1f, centerX, centerZ)) return;

    std::vector<std::pair<int, int>> cellsToBlock;
    std::vector<std::pair<int, int>> cellsToUnblock;

    if (d.direction == 1) { // Parallel to X-axis
        // Always block Posts (Outer edges)
        cellsToBlock.push_back({ centerX - 1, centerZ }); // Far Left Post
        cellsToBlock.push_back({ centerX + 2, centerZ }); // Far Right Post

        if (isClosed) {
            // Closed: Block Middle Two
            cellsToBlock.push_back({ centerX, centerZ });
            cellsToBlock.push_back({ centerX + 1, centerZ });
        }
        else {
            // Open: Unblock Middle Two
            cellsToUnblock.push_back({ centerX, centerZ });
            cellsToUnblock.push_back({ centerX + 1, centerZ });
        }
    }
    else { // Parallel to Z-axis
        // Always block Posts
        cellsToBlock.push_back({ centerX, centerZ - 1 }); // Top Post
        cellsToBlock.push_back({ centerX, centerZ + 2 }); // Bottom Post

        if (isClosed) {
            // Closed: Block Middle Two
            cellsToBlock.push_back({ centerX, centerZ });
            cellsToBlock.push_back({ centerX, centerZ + 1 });
        }
        else {
            // Open: Unblock Middle Two
            cellsToUnblock.push_back({ centerX, centerZ });
            cellsToUnblock.push_back({ centerX, centerZ + 1 });
        }
    }

    // Apply Blocks
    for (auto& cell : cellsToBlock) {
        addBlockGridBox(cell.first, cell.second);
    }

    // Apply Unblocks
    for (auto& cell : cellsToUnblock) {
        removeBlockGridBox(cell.first, cell.second);
    }

    d.blockedCells = cellsToBlock;

    if (isClosed) printf("Door %d Closed (4 Cells Blocked).\n", index);
    else printf("Door %d Open (2 Middle Cells Unblocked).\n", index);
}

void SecretDoor::update(float dt) {
    for (auto& door : m_doors) {
        float targetAngle = door.isOpen ? 90.0f : 0.0f;
        float speed = 100.0f * dt;

        if (door.openAngle < targetAngle) {
            door.openAngle += speed;
            if (door.openAngle > targetAngle) door.openAngle = targetAngle;
        }
    }
}

int SecretDoor::getNearestDoorIndex(float playerX, float playerZ) {
    int nearestIndex = -1;
    float minDist = m_interactionRange;

    for (size_t i = 0; i < m_doors.size(); ++i) {
        float dx = m_doors[i].x - playerX;
        float dz = m_doors[i].z - playerZ;
        float dist = sqrt(dx * dx + dz * dz);

        if (dist < minDist) {
            minDist = dist;
            nearestIndex = (int)i;
        }
    }
    return nearestIndex;
}

bool SecretDoor::tryUnlock(int index, const char* enteredPin) {
    if (index >= 0 && index < m_doors.size()) {
        if (m_doors[index].pinCode == enteredPin) {
            m_doors[index].isOpen = true;
            updateCollision(index, false); // Update to open state
            return true;
        }
    }
    return false;
}

bool SecretDoor::isDoorOpen(int index) {
    if (index >= 0 && index < m_doors.size()) return m_doors[index].isOpen;
    return false;
}

void SecretDoor::loadTextures(const char* frameTex, const char* doorTex, const char* detailTex) {
    m_texFrame = loadTexture(frameTex);
    m_texDoor = loadTexture(doorTex);
    m_texDetail = loadTexture(detailTex);
}

GLuint SecretDoor::loadTexture(const char* path) {
    GLuint id = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
    return id;
}

void SecretDoor::draw() {
    glColor3f(1.0f, 1.0f, 1.0f);

    for (const auto& door : m_doors) {
        glPushMatrix();
        glTranslatef(door.x, 0.0f, door.z);

        if (door.direction == 2) {
            glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
        }

        drawDoorModel(door.openAngle, door.direction);
        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
}

// Forward declaration if not in header
// void drawCylinder(float radius, float height); 

// Helper to draw a fancy handle
void drawHandle() {
    // A nice knob handle: A small horizontal cylinder coming out, then a sphere/disk

    // 1. Stem (Horizontal Cylinder)
    GLUquadricObj* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluCylinder(q, 0.03f, 0.03f, 0.1f, 12, 1);

    // 2. Knob (Sphere at the end)
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.1f); // Move to end of stem
    gluSphere(q, 0.06f, 12, 12);
    glPopMatrix();

    gluDeleteQuadric(q);
}

void SecretDoor::drawDoorModel(float angle, int direction) {
    float doorW = 4.0f;
    float doorH = 3.5f;
    float doorThick = 0.4f;
    float postW = 1.0f;
    float postD = 0.8f;

    // --- STATIC FRAME ---
    // Apply FRAME Texture
    if (m_texFrame) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texFrame); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.2f, 0.2f, 0.2f); }

    // Left Post (-1.5)
    glPushMatrix(); glTranslatef(-1.5f, doorH / 2, 0); drawBox(postW, doorH, postD); glPopMatrix();
    // Right Post (+1.5)
    glPushMatrix(); glTranslatef(1.5f, doorH / 2, 0); drawBox(postW, doorH, postD); glPopMatrix();
    // Top Bar
    glPushMatrix(); glTranslatef(0, doorH, 0); drawBox(doorW, 0.5f, postD); glPopMatrix();

    // --- NEW: TOP CYLINDERS (Wicker Fence Style) ---
    // Apply DETAIL Texture
    if (m_texDetail) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texDetail); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.6f, 0.6f, 0.6f); }

    // Dimensions for top pillars
    float cylHeight = 1.5f;
    float cylRadius = 0.10f; // Slightly thinner as requested

    // Place 8 cylinders spaced along the top bar
    // Width is 4.0 (-2.0 to 2.0). 
    float startX = -1.8f;
    float step = 3.6f / 7.0f; // Distribute 8 items across 3.6 width

    for (int i = 0; i < 8; i++) {
        glPushMatrix();
        glTranslatef(startX + (i * step), doorH + 0.25f, 0.0f); // Sit on top of the bar
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
        drawCylinder(cylRadius, cylHeight);
        glPopMatrix();
    }


    // --- DOUBLE DOORS ---
    // Apply DOOR Texture
    if (m_texDoor) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texDoor); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.5f, 0.55f, 0.6f); }

    float panelW = 1.0f;

    // -- LEFT PANEL --
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f); // Hinge
    glRotatef(angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(panelW / 2.0f, doorH / 2, 0.0f); // Center Panel

    // Draw Door Panel
    drawBox(panelW, doorH, doorThick);

    // Draw Handle (Detailed)
    if (m_texDetail) { glBindTexture(GL_TEXTURE_2D, m_texDetail); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.8f, 0.7f, 0.2f); }

    // Front Handle
    glPushMatrix();
    glTranslatef(0.3f, 0.0f, (doorThick / 5.0f) + 0.02f); // Stick out from surface
    drawHandle();
    glPopMatrix();

    // Back Handle (Rotate 180 to face other way)
    glPushMatrix();
    glTranslatef(0.3f, 0.0f, -(doorThick / 5.0f) - 0.02f);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    drawHandle();
    glPopMatrix();

    // Restore Door Texture
    if (m_texDoor) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texDoor); glColor3f(1, 1, 1); }
    glPopMatrix();


    // -- RIGHT PANEL --
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f); // Hinge
    glRotatef(-angle, 0.0f, 1.0f, 0.0f);
    glTranslatef(-panelW / 2.0f, doorH / 2, 0.0f); // Center Panel
    drawBox(panelW, doorH, doorThick);

    // Draw Handle (Detailed)
    if (m_texDetail) { glBindTexture(GL_TEXTURE_2D, m_texDetail); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.8f, 0.7f, 0.2f); }

    // Front Handle
    glPushMatrix();
    glTranslatef(-0.3f, 0.0f, (doorThick / 5.0f) + 0.02f);
    drawHandle();
    glPopMatrix();

    // Back Handle
    glPushMatrix();
    glTranslatef(-0.3f, 0.0f, -(doorThick / 5.0f) - 0.02f);
    glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
    drawHandle();
    glPopMatrix();

    glPopMatrix();
}

// UPDATED drawBox with thinned depth (d/5.0f)
void SecretDoor::drawBox(float w, float h, float d) {
    float hw = w / 2.0f;
    float hh = h / 2.0f;
    float hd = d / 5.0f; // <-- UPDATED as requested to make panels thinner visually relative to collision

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, hd);
    glNormal3f(0, 0, -1); glTexCoord2f(0, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(-hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(hw, hh, -hd);
    glNormal3f(-1, 0, 0); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(-hw, -hh, hd); glTexCoord2f(1, 1); glVertex3f(-hw, hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);
    glNormal3f(1, 0, 0); glTexCoord2f(0, 0); glVertex3f(hw, -hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(hw, hh, hd);
    glNormal3f(0, 1, 0); glTexCoord2f(0, 0); glVertex3f(-hw, hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, hh, hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);
    glNormal3f(0, -1, 0); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(hw, -hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, -hh, hd);
    glEnd();
}

void SecretDoor::drawCylinder(float radius, float height) {
    GLUquadricObj* quadratic;
    quadratic = gluNewQuadric();
    gluQuadricNormals(quadratic, GLU_SMOOTH);
    gluQuadricTexture(quadratic, GL_TRUE); // Enable texture coords
    gluCylinder(quadratic, radius, radius, height, 16, 2);
    // Draw cap
    gluDisk(quadratic, 0, radius, 16, 1);
    glPushMatrix();
    glTranslatef(0, 0, height);
    gluDisk(quadratic, 0, radius, 16, 1);
    glPopMatrix();
    gluDeleteQuadric(quadratic);
}