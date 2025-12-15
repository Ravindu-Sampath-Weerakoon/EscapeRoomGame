#include "pch.h"
#include "SecretBook.h"
#include <math.h>
#include <stdio.h>
#include <SOIL2.h> 

SecretBook::SecretBook()
    : m_interactionRange(2.0f), m_texWood(0), m_texCover(0), m_texPage(0)
{
}

void SecretBook::addBook(float x, float z, const char* message) {
    BookData b;
    b.x = x;
    b.z = z;
    b.message = message;
    b.isOpen = false;
    b.openAngle = 0.0f;
    m_books.push_back(b);
}

void SecretBook::loadTextures(const char* woodTex, const char* bookCoverTex, const char* pageTex) {
    m_texWood = loadTexture(woodTex);
    m_texCover = loadTexture(bookCoverTex);
    m_texPage = loadTexture(pageTex);
}

GLuint SecretBook::loadTexture(const char* path) {
    GLuint id = SOIL_load_OGL_texture(path, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
        SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_TEXTURE_REPEATS);
    // Suppress error spam if texture is missing, just return 0
    return id;
}

void SecretBook::update(float dt) {
    for (auto& book : m_books) {
        float targetAngle = book.isOpen ? 170.0f : 0.0f; // 170 degrees (almost flat)
        float speed = 300.0f * dt;

        if (book.openAngle < targetAngle) {
            book.openAngle += speed;
            if (book.openAngle > targetAngle) book.openAngle = targetAngle;
        }
        else if (book.openAngle > targetAngle) {
            book.openAngle -= speed;
            if (book.openAngle < targetAngle) book.openAngle = targetAngle;
        }
    }
}

int SecretBook::getNearestBookIndex(float playerX, float playerZ) {
    int nearestIndex = -1;
    float minDist = m_interactionRange;

    for (size_t i = 0; i < m_books.size(); ++i) {
        float dx = m_books[i].x - playerX;
        float dz = m_books[i].z - playerZ;
        float dist = sqrt(dx * dx + dz * dz);

        if (dist < minDist) {
            minDist = dist;
            nearestIndex = (int)i;
        }
    }
    return nearestIndex;
}

void SecretBook::toggleBook(int index) {
    if (index >= 0 && index < m_books.size()) {
        m_books[index].isOpen = !m_books[index].isOpen;
    }
}

bool SecretBook::isBookOpen(int index) {
    if (index >= 0 && index < m_books.size()) return m_books[index].isOpen;
    return false;
}

const char* SecretBook::getBookMessage(int index) {
    if (index >= 0 && index < m_books.size()) return m_books[index].message.c_str();
    return "";
}

void SecretBook::draw() {
    // Enable textures globally if available, or just toggle inside
    glColor3f(1.0f, 1.0f, 1.0f);

    for (const auto& book : m_books) {
        glPushMatrix();
        glTranslatef(book.x, 0.0f, book.z);

        // 1. Draw Stool
        drawStool();

        // 2. Draw Book on top
        // Stool height is 1.0 (legs) + 0.1 (seat) = 1.1
        glTranslatef(0.0f, 1.1f, 0.0f);

        // Rotate book to face player or random direction? 
        // Let's rotate 90 deg so spine is along Z or X as needed. 
        // Default drawing assumes spine is Z-axis (0,0,1).

        drawAnimatedBook(book.openAngle);

        glPopMatrix();
    }

    glDisable(GL_TEXTURE_2D);
}

void SecretBook::drawStool() {
    if (m_texWood) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texWood); }
    else glDisable(GL_TEXTURE_2D);

    glColor3f(0.6f, 0.4f, 0.2f); // Wood color fallback

    // --- Seat ---
    glPushMatrix();
    glTranslatef(0.0f, 1.05f, 0.0f); // Center of seat (1.0 + 0.1/2)
    drawBox(0.9f, 0.1f, 0.9f);
    glPopMatrix();

    // --- Legs ---
    float legW = 0.1f;
    float legH = 1.0f;
    float offset = 0.35f;

    glPushMatrix(); glTranslatef(offset, 0.5f, offset); drawBox(legW, legH, legW); glPopMatrix();
    glPushMatrix(); glTranslatef(-offset, 0.5f, offset); drawBox(legW, legH, legW); glPopMatrix();
    glPushMatrix(); glTranslatef(offset, 0.5f, -offset); drawBox(legW, legH, legW); glPopMatrix();
    glPushMatrix(); glTranslatef(-offset, 0.5f, -offset); drawBox(legW, legH, legW); glPopMatrix();
}

void SecretBook::drawAnimatedBook(float angle) {
    float coverW = 0.4f;
    float coverH = 0.02f;
    float coverD = 0.5f;
    float pageBlockH = 0.06f; // Total thickness of pages

    // We split pages into bottom half and top half
    float halfPageH = pageBlockH / 2.0f;

    // --- BOTTOM ASSEMBLY (Static) ---
    // 1. Bottom Cover
    if (m_texCover) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texCover); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.6f, 0.0f, 0.0f); }

    glPushMatrix();
    // Offset X by +width/2 so left edge is at 0 (spine)
    // Spine is at local X=0. The book extends to +X.
    glTranslatef(coverW / 2.0f, coverH / 2.0f, 0.0f);
    drawBox(coverW, coverH, coverD);
    glPopMatrix();

    // 2. Bottom Pages
    if (m_texPage) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texPage); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.95f, 0.95f, 0.9f); }

    glPushMatrix();
    // Sit on top of bottom cover
    glTranslatef(coverW / 2.0f, coverH + (halfPageH / 2.0f), 0.0f);
    // Slightly smaller width/depth for pages
    drawBox(coverW - 0.02f, halfPageH, coverD - 0.02f);
    glPopMatrix();


    // --- TOP ASSEMBLY (Rotates around Spine) ---
    // Pivot Point: X=0 (Spine), Y = Height of bottom assembly
    // The spine is at X=0.
    float pivotY = coverH + halfPageH;

    glPushMatrix();
    glTranslatef(0.0f, pivotY, 0.0f); // Move coordinate system to the hinge point

    // --- FIX: Positive angle rotates UP and OVER (Opening) ---
    // Was -angle (which rotated down into stool). Now 'angle' rotates up.
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    // 3. Top Pages (Attached to top cover)
    if (m_texPage) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texPage); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.95f, 0.95f, 0.9f); }

    glPushMatrix();
    // Relative to pivot: Move RIGHT (X+) and UP (Y+) because we are now in the rotated space
    // The top pages start at the spine (X=0) and go out.
    glTranslatef(coverW / 2.0f, halfPageH / 2.0f, 0.0f);
    drawBox(coverW - 0.02f, halfPageH, coverD - 0.02f);
    glPopMatrix();

    // 4. Top Cover
    if (m_texCover) { glEnable(GL_TEXTURE_2D); glBindTexture(GL_TEXTURE_2D, m_texCover); glColor3f(1, 1, 1); }
    else { glDisable(GL_TEXTURE_2D); glColor3f(0.6f, 0.0f, 0.0f); }

    glPushMatrix();
    // Sit on top of top pages
    glTranslatef(coverW / 2.0f, halfPageH + (coverH / 2.0f), 0.0f);
    drawBox(coverW, coverH, coverD);
    glPopMatrix();

    glPopMatrix(); // End Rotation
}

void SecretBook::drawBox(float w, float h, float d) {
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