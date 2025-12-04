#pragma once
// --- CRITICAL FIX: Include stdlib.h FIRST to prevent exit() redefinition error ---
#include <stdlib.h> 
#include "pch.h"
#include <vector>
#include <glut.h> 

// Structure to define a single wall segment
struct WallSegment {
    float startX, startZ;
    float endX, endZ;
    float thickness;
    float height;
};

class InsideWall {
public:
    InsideWall(float height);

    // Add a wall from point A to point B
    // thickness: how thick the wall is (usually 0.5 or 1.0)
    void addWall(float startX, float startZ, float endX, float endZ, float thickness);

    // Call this AFTER adding all walls to generate the Display List
    void build(GLuint textureID);

    // Draw the walls
    void draw();

private:
    float m_height;
    GLuint m_textureID;

    // This variable stores the GPU list ID (Critical for the build function)
    GLuint m_displayListID;

    std::vector<WallSegment> m_walls;
};