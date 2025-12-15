#pragma once
#pragma once
#include <stdlib.h> // Fix for exit() redefinition
#include "pch.h"
#include <glut.h>
#include <vector>
#include <string>

// Structure for a single book instance
struct BookData {
    float x, z;
    std::string message;
    bool isOpen;
};

class SecretBook {
public:
    SecretBook();

    // Add a new book to the world
    void addBook(float x, float z, const char* message);

    // Draw all books
    void draw();

    // Check if player is near ANY book. 
    // Returns index of nearest book, or -1 if none found.
    int getNearestBookIndex(float playerX, float playerZ);

    // Interact with a specific book
    void toggleBook(int index);
    bool isBookOpen(int index);
    const char* getBookMessage(int index);

private:
    std::vector<BookData> m_books;
    float m_interactionRange; // e.g., 5.0f

    // Internal helper to draw one book model
    void drawSingleBook(float x, float z, bool isOpen);
};