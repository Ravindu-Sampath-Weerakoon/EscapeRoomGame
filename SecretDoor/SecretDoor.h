#pragma once
#include <stdlib.h> // Standard library
#include "pch.h"
#include <glut.h>
#include <vector>
#include <string>

// Structure for a single door instance
struct DoorData {
    float x, z;
    int direction; // 1 = Parallel to X, 2 = Parallel to Z
    std::string pinCode;
    bool isOpen;
    float openAngle; // 0.0 (closed) to 90.0 (open)

    // Store the grid coordinates blocked by this door so we can unblock them later
    std::vector<std::pair<int, int>> blockedCells;
};

class SecretDoor {
public:
    SecretDoor();

    // Add a new door
    // direction: 1 (X-axis) or 2 (Z-axis)
    // pin: 3-digit string (e.g., "123")
    void addDoor(float x, float z, int direction, const char* pin);

    // Load textures (optional, can use colors for now)
    void loadTextures(const char* doorTex);

    // Update animation logic
    void update(float dt);

    // Draw all doors
    void draw();

    // Check if player is near any door
    // Returns index of nearest door, or -1
    int getNearestDoorIndex(float playerX, float playerZ);

    // Attempt to unlock a door
    // Returns true if PIN is correct
    bool tryUnlock(int index, const char* enteredPin);

    // Check if a specific door is already open
    bool isDoorOpen(int index);

private:
    std::vector<DoorData> m_doors;
    float m_interactionRange;
    GLuint m_texDoor;

    // Helper to draw the physical door
    void drawDoorModel(float angle, int direction);
    void drawBox(float w, float h, float d);

    // Collision helpers
    void updateCollision(int index, bool block);
    GLuint loadTexture(const char* path);
};