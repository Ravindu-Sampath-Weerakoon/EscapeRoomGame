#pragma once
#include "pch.h" // Includes <glut.h>

class TheRoom {
public:
    // Constructor: Define room dimensions
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
    void drawFloor();
    void drawWalls();
    void drawCeiling();

    // Internal Texture Management
    GLuint loadSingleTexture(const char* path);
    // GLuint createCheckerboardTexture(int width, int height, int checkSize); // (Optional, depending on implementation)

    // ADD THESE NEW HELPER DECLARATIONS
    void bindAndCheckTexture(GLuint textureID);
    void unbindAndRestore();
};