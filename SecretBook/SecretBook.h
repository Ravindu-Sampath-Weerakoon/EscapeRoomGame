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
    float openAngle; // 0.0 (closed) to 180.0 (open)
};

class SecretBook {
public:
    SecretBook();

    // Add a new book to the world
    void addBook(float x, float z, const char* message);

    // Setup textures (Call this in init)
    void loadTextures(const char* woodTex, const char* bookCoverTex, const char* pageTex);

    // Update animation logic (Call in idle/update)
    void update(float dt);

    // Draw all books
    void draw();

    // Check if player is near ANY book. 
    int getNearestBookIndex(float playerX, float playerZ);

    // Interact with a specific book
    void toggleBook(int index);
    bool isBookOpen(int index);
    const char* getBookMessage(int index);

private:
    std::vector<BookData> m_books;
    float m_interactionRange;

    // Textures
    GLuint m_texWood;
    GLuint m_texCover;
    GLuint m_texPage;

    // Helper functions
    void drawStool();
    void drawAnimatedBook(float angle);
    void drawBox(float w, float h, float d);
    GLuint loadTexture(const char* path);
};