#include "pch.h"
#include "SecretBook.h"
#include <math.h>
#include <stdio.h>

SecretBook::SecretBook()
    : m_interactionRange(1.5f) // Threshold distance 1.5
{
}

void SecretBook::addBook(float x, float z, const char* message) {
    BookData b;
    b.x = x;
    b.z = z;
    b.message = message;
    b.isOpen = false;
    m_books.push_back(b);
}

// Find nearest book within range
int SecretBook::getNearestBookIndex(float playerX, float playerZ) {
    int nearestIndex = -1;
    float minDist = m_interactionRange; // Start with max range

    for (size_t i = 0; i < m_books.size(); ++i) {
        float dx = m_books[i].x - playerX;
        float dz = m_books[i].z - playerZ;
        float dist = sqrt(dx * dx + dz * dz);

        if (dist < minDist) {
            minDist = dist;
            nearestIndex = (int)i;
        }
    }
    return nearestIndex; // Returns -1 if nothing is close enough
}

void SecretBook::toggleBook(int index) {
    if (index >= 0 && index < m_books.size()) {
        // Toggle the specific book
        m_books[index].isOpen = !m_books[index].isOpen;

        // OPTIONAL: Close all other books when opening one?
        // For now, let's just toggle this one.
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
    for (const auto& book : m_books) {
        drawSingleBook(book.x, book.z, book.isOpen);
    }
}

void SecretBook::drawSingleBook(float x, float z, bool isOpen) {
    glPushMatrix();
    glTranslatef(x, 0.0f, z);

    // Podium (Brown)
    glColor3f(0.4f, 0.2f, 0.1f);
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glScalef(0.8f, 1.0f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();

    // Book (Red/White)
    glTranslatef(0.0f, 1.1f, 0.0f);
    if (isOpen) {
        glColor3f(1.0f, 1.0f, 1.0f); // Open pages
        glScalef(0.6f, 0.1f, 0.4f);
    }
    else {
        glColor3f(0.8f, 0.0f, 0.0f); // Closed cover
        glScalef(0.4f, 0.15f, 0.5f);
    }
    glutSolidCube(1.0f);

    glPopMatrix();
    glColor3f(1.0f, 1.0f, 1.0f); // Reset
}