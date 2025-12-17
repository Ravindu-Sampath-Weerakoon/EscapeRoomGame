#pragma once
#include <stdlib.h> // Standard library
#include "pch.h"
#include <glut.h>
#include <vector>

// Enum for object types to make code readable
enum DecorType {
    DECOR_CHAIR = 1,
    DECOR_TABLE = 2,
    DECOR_CRATE = 3,
    // Add more types here later
};

// Structure for a single decoration instance
struct DecorInstance {
    int type;
    float x, z;
    float rotation; // Degrees
};

class RoomDecorations {
public:
    RoomDecorations();

    // Add a new decoration object
    // type: 1=Chair, 2=Table, etc.
    // x, z: Position
    // rotation: Rotation in degrees (optional, default 0)
    void addDecoration(int type, float x, float z, float rotation = 0.0f);

    // Load textures for decorations
    void loadTextures(const char* woodTex, const char* metalTex);

    // Draw all decorations
    void draw();

private:
    std::vector<DecorInstance> m_objects;

    // Textures
    GLuint m_texWood;
    GLuint m_texMetal;

    // Helper functions for specific objects
    void drawChair(float x, float z, float rot);
    void drawTable(float x, float z, float rot); // Placeholder for next type
    void drawCupboard(float x, float z, float rot);
    void drawBed(float x, float z, float rot);
    void drawRack(float x, float z, float rot);
    void drawCrate(float x, float z, float rot); // Placeholder

    void drawFloorLamp(float x, float z, float rot);
    void drawSofa(float x, float z, float rot);
    void drawTVUnit(float x, float z, float rot);
    void drawDesk(float x, float z, float rot);
    void drawPlant(float x, float z, float rot);

    void drawCylinder(float radius, float height, int segments);

    // Generic box drawer
    void drawBox(float w, float h, float d);
    GLuint loadTexture(const char* path);
};