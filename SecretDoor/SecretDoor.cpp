#include "pch.h"
#include "SecretDoor.h"
#include "GraphicsUtils.h" // Needed for grid functions
#include <math.h>
#include <stdio.h>
#include <SOIL2.h>

SecretDoor::SecretDoor()
    : m_interactionRange(2.5f), m_texDoor(0)
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
    // Center is at (d.x, d.z). 
    // This typically aligns with a grid line boundary if the width is even (4).
    // Let's assume (x,z) is the exact center of the 4-unit span.
    //
    // Logic:
    // Left/Top Post: ALWAYS BLOCKED
    // Left/Top Door: Blocked if Closed
    // Right/Bot Door: Blocked if Closed
    // Right/Bot Post: ALWAYS BLOCKED

    int centerX, centerZ;
    // We use a small offset to ensure we catch the 'lower' grid index if on a boundary
    if (!worldToGrid(d.x - 0.1f, d.z - 0.1f, centerX, centerZ)) return;

    std::vector<std::pair<int, int>> cellsToBlock;
    std::vector<std::pair<int, int>> cellsToUnblock;

    if (d.direction == 1) { // Parallel to X-axis
        // Center is between centerX and centerX+1.
        // We span 4 cells: [centerX-1], [centerX], [centerX+1], [centerX+2]

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
        // Spanning Z.
        // Cells: [centerZ-1], [centerZ], [centerZ+1], [centerZ+2]

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

void SecretDoor::loadTextures(const char* doorTex) {
    m_texDoor = loadTexture(doorTex);
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

void SecretDoor::drawDoorModel(float angle, int direction) {
    // Dimensions: Width is 4.0
    float doorW = 4.0f;
    float doorH = 3.5f;
    float doorThick = 0.3f;

    // Post Dimensions (Takes up 1.0 unit each)
    float postW = 1.0f;
    float postD = 0.8f;

    // --- STATIC FRAME ---
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.3f, 0.3f, 0.3f); // Dark Grey Frame

    // Left Post (Centered at -1.5)
    // -doorW/2 + postW/2 = -2.0 + 0.5 = -1.5
    glPushMatrix();
    glTranslatef(-1.5f, doorH / 2, 0);
    drawBox(postW, doorH, postD);
    glPopMatrix();

    // Right Post (Centered at +1.5)
    // doorW/2 - postW/2 = 2.0 - 0.5 = 1.5
    glPushMatrix();
    glTranslatef(1.5f, doorH / 2, 0);
    drawBox(postW, doorH, postD);
    glPopMatrix();

    // Top Bar (Spans the whole 4.0 width)
    glPushMatrix();
    glTranslatef(0, doorH, 0);
    drawBox(doorW, 0.5f, postD);
    glPopMatrix();


    // --- DOUBLE DOORS (Two Pieces in the Middle) ---
    if (m_texDoor) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texDoor); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.6f, 0.4f, 0.2f); }

    // Middle Opening = Total (4.0) - Posts (2.0) = 2.0
    // Each panel width = 1.0
    float panelW = 1.0f;

    // -- LEFT PANEL --
    // Pivot at inner edge of Left Post.
    // Left Post inner edge is at -1.0.
    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f); // Hinge
    glRotatef(angle, 0.0f, 1.0f, 0.0f);   // Rotate Out (Left)

    // Draw Panel (Centered at +0.5 local X)
    glTranslatef(panelW / 2.0f, doorH / 2, 0.0f);
    drawBox(panelW, doorH, doorThick);
    glPopMatrix();


    // -- RIGHT PANEL --
    // Pivot at inner edge of Right Post.
    // Right Post inner edge is at +1.0.
    glPushMatrix();
    glTranslatef(1.0f, 0.0f, 0.0f); // Hinge
    glRotatef(-angle, 0.0f, 1.0f, 0.0f);   // Rotate Out (Right)

    // Draw Panel (Centered at -0.5 local X)
    glTranslatef(-panelW / 2.0f, doorH / 2, 0.0f);
    drawBox(panelW, doorH, doorThick);
    glPopMatrix();
}

void SecretDoor::drawBox(float w, float h, float d) {
    float hw = w / 2.0f;
    float hh = h / 2.0f;
    float hd = d / 2.0f;

    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, hd);
    glNormal3f(0, 0, -1); glTexCoord2f(0, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(-hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(hw, hh, -hd);
    glNormal3f(-1, 0, 0); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(-hw, -hh, hd); glTexCoord2f(1, 1); glVertex3f(-hw, hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);
    glNormal3f(1, 0, 0); glTexCoord2f(0, 0); glVertex3f(hw, -hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(hw, hh, hd);
    glNormal3f(0, 1, 0); glTexCoord2f(0, 0); glVertex3f(-hw, hh, hd); glTexCoord2f(1, 0); glVertex3f(hw, hh, hd); glTexCoord2f(1, 1); glVertex3f(hw, hh, -hd); glTexCoord2f(0, 1); glVertex3f(-hw, hh, -hd);
    glNormal3f(0, -1, 0); glTexCoord2f(0, 0); glVertex3f(-hw, -hh, -hd); glTexCoord2f(1, 0); glVertex3f(hw, -hh, -hd); glTexCoord2f(1, 1); glVertex3f(hw, -hh, hd); glTexCoord2f(0, 1); glVertex3f(-hw, -hh, hd);
    glEnd();
}