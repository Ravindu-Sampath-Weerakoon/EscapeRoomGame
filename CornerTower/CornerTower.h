#pragma once
// Standard Library First to prevent exit() error
#include <stdlib.h>
#include "pch.h"
#include <glut.h>
#include <vector> // Needed for storing multiple towers

// Structure to hold the position of a single tower
struct TowerPos {
    float x;
    float z;
};

class CornerTower {
public:
    // Constructor: Now takes the room height and tower width as fixed parameters
    CornerTower(float roomHeight, float towerWidth);
    ~CornerTower();

    // Add a new tower at a specific location
    void addTower(float x, float z);

    // Create the display list and collision
    // textureID: The metal texture
    void build(GLuint textureID);

    // Draw all towers
    void draw();

private:
    // Shared properties for all towers
    float m_width;
    float m_height;
    float m_rimHeight;
    float m_rimOverhang;

    GLuint m_textureID;
    GLuint m_displayListID;

    // List of positions
    std::vector<TowerPos> m_towers;

    // Internal helper to draw a textured box
    void drawBox(float width, float height, float depth);
};