#pragma once
#include "pch.h" // Includes <glut.h> and other standards

class TheRoom {
public:
    // Constructor: Define room dimensions
    TheRoom(float width, float height, float depth);

    // Destructor: Cleans up the Display List
    ~TheRoom();

    // Loads the textures from files
    bool loadTextures(const char* floorTexPath, const char* wallTexPath, const char* ceilingTexPath);

    // --- NEW: Compiles the drawing commands into a Display List ---
    // Call this AFTER loadTextures()
    void build();

    // Master draw function (Optimized to use the Display List)
    void draw();

private:
    float m_width;
    float m_height;
    float m_depth;

    GLuint m_texFloor;
    GLuint m_texWall;
    GLuint m_texCeiling;

    // --- NEW: Variable to store the Display List ID ---
    GLuint m_displayListID;

    // Internal Drawing Functions
    void drawFloor();
    void drawWalls();
    void drawCeiling();

    // Internal Texture Management
    GLuint loadSingleTexture(const char* path);

    // Helper functions for texture binding
    void bindAndCheckTexture(GLuint textureID);
    void unbindAndRestore();
};