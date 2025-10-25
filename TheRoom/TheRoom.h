#pragma once
#include "pch.h" // Includes <glut.h>

class TheRoom { // <-- Renamed to TheRoom
public:
    TheRoom(float width, float height, float depth);

    bool loadTextures(const char* floorTexPath, const char* wallTexPath, const char* ceilingTexPath);
    void draw(); // Master draw function

private:
    float m_width;
    float m_height;
    float m_depth;

    GLuint m_texFloor;
    GLuint m_texWall;
    GLuint m_texCeiling;

    // Internal Drawing Functions
    void drawFloor();   // <-- NEW
    void drawWalls();   // <-- NEW
    void drawCeiling(); // <-- NEW

    GLuint loadSingleTexture(const char* path);
    GLuint createCheckerboardTexture(int width, int height, int checkSize);
};