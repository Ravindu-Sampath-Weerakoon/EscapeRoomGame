#include "pch.h" // Must be first
#include "Labels.h"
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm> // For std::max
#include <glut.h>

// Define a simple structure to hold text lines locally
struct HudLine {
    std::string text;
    float r, g, b; // Color
};

Labels::Labels(int windowWidth, int windowHeight) {
    onWindowResize(windowWidth, windowHeight);
    m_showHelp = false;
    m_font = GLUT_BITMAP_9_BY_15;
    m_lineHeight = 17; // 15px font + 2px spacing
}

void Labels::onWindowResize(int w, int h) {
    m_windowWidth = w;
    m_windowHeight = (h == 0) ? 1 : h;
}

void Labels::toggleHelp() {
    m_showHelp = !m_showHelp;
}

// --- Helper to measure text width in pixels ---
int Labels::getTextWidth(const char* text) {
    return glutBitmapLength(m_font, (const unsigned char*)text);
}

void Labels::renderText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(m_font, *text);
        text++;
    }
}

// --- Helper: Draws a transparent black box ---
// (Static function: internal to this file only)
static void drawBackgroundBox(float x, float y, float w, float h) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.6f); // Black with 60% Opacity

    glBegin(GL_QUADS);
    glVertex2f(x, y);         // Top Left
    glVertex2f(x + w, y);     // Top Right
    glVertex2f(x + w, y - h); // Bottom Right
    glVertex2f(x, y - h);     // Bottom Left
    glEnd();

    glDisable(GL_BLEND);
}

void Labels::draw(bool isDeveloperMode, float camX, float camY, float camZ) {
    // Setup 2D Orthographic View
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, m_windowWidth, 0, m_windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // ============================================================
    // RIGHT PANEL: Coordinates (Top-Right) - AUTO SIZED
    // ============================================================
    if (isDeveloperMode) {
        char coordBuffer[128];
        // Format: X : 10.5  Y : 5.0  Z : -15.2
        sprintf_s(coordBuffer, sizeof(coordBuffer), "X : %.1f   Y : %.1f   Z : %.1f", camX, camY, camZ);

        // 1. Calculate Width dynamically
        int textWidth = getTextWidth(coordBuffer);

        float padding = 30.0f; // Total horizontal padding
        float boxWidth = textWidth + padding;
        float boxHeight = 30.0f;

        float rightMargin = 20.0f;
        float topMargin = 20.0f;

        // Position: Align to right based on dynamic width
        float boxX = m_windowWidth - boxWidth - rightMargin;
        float boxY = m_windowHeight - topMargin;

        // Draw Box
        drawBackgroundBox(boxX, boxY, boxWidth, boxHeight);

        // Draw Text (White)
        glColor3f(1.0f, 1.0f, 1.0f);
        // Center text vertically inside the box
        renderText(boxX + (padding / 2), boxY - 20, coordBuffer);
    }

    // ============================================================
    // LEFT PANEL: Controls / Help (Top-Left) - AUTO SIZED
    // ============================================================

    // 1. Prepare the list of lines to draw
    std::vector<HudLine> lines;

    if (!m_showHelp) {
        lines.push_back({ "Press 'Tab' to show controls", 1.0f, 1.0f, 1.0f });
    }
    else {
        lines.push_back({ "Press 'Tab' to hide controls", 1.0f, 1.0f, 1.0f });
        lines.push_back({ "", 1.0f, 1.0f, 1.0f }); // Spacer

        if (isDeveloperMode) {
            lines.push_back({ "[ DEVELOPER MODE ]", 1.0f, 0.5f, 0.5f }); // Light Red
            lines.push_back({ "", 1.0f, 1.0f, 1.0f }); // Spacer
            lines.push_back({ "WASD       : Move (Fly)", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Q / E      : Fly Up/Down", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Arrows     : Look Around", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Shift      : Move Faster", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "T          : Toggle Axes", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "C          : Toggle Coords", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "P          : Switch to Game Mode", 1.0f, 1.0f, 1.0f });
        }
        else {
            lines.push_back({ "[ GAME MODE ]", 0.5f, 1.0f, 0.5f }); // Light Green
            lines.push_back({ "", 1.0f, 1.0f, 1.0f }); // Spacer
            lines.push_back({ "W A S D    : Move (Walk)", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Mouse      : Look Around", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Space      : Jump", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Shift      : Sprint", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "", 1.0f, 1.0f, 1.0f }); // Spacer
            lines.push_back({ "P          : Switch to Developer", 1.0f, 1.0f, 1.0f });
        }
    }

    // 2. Calculate Max Width needed
    int maxTextWidth = 0;
    for (const auto& line : lines) {
        int w = getTextWidth(line.text.c_str());
        if (w > maxTextWidth) maxTextWidth = w;
    }

    // 3. Define Box Dimensions based on content
    float padding = 30.0f;
    float boxWidth = maxTextWidth + padding;
    float boxHeight = (lines.size() * m_lineHeight) + 15.0f; // 15px vertical padding

    // Adjust height for single line (make it a bit slimmer visually)
    if (lines.size() == 1) boxHeight = 30.0f;

    float leftMargin = 20.0f;
    float topMargin = 20.0f;
    float boxX = leftMargin;
    float boxY = m_windowHeight - topMargin;

    // 4. Draw Background
    drawBackgroundBox(boxX, boxY, boxWidth, boxHeight);

    // 5. Draw All Lines
    float textX = boxX + (padding / 2);
    float textY = boxY - 20;

    for (const auto& line : lines) {
        glColor3f(line.r, line.g, line.b);
        renderText(textX, textY, line.text.c_str());
        textY -= m_lineHeight;
    }

    // Restore 3D settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}