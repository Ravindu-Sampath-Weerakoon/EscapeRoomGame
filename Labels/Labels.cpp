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

// --- Helper to measure text width in pixels (Multi-line support) ---
int Labels::getTextWidth(const char* text) {
    int maxWidth = 0;
    int currentWidth = 0;

    while (*text) {
        if (*text == '\n') {
            // End of line: check if this was the widest so far
            if (currentWidth > maxWidth) maxWidth = currentWidth;
            currentWidth = 0; // Reset for next line
        }
        else {
            currentWidth += glutBitmapWidth(m_font, *text);
        }
        text++;
    }
    // Check last line
    if (currentWidth > maxWidth) maxWidth = currentWidth;

    return maxWidth;
}

// --- UPDATED: Renders text with support for '\n' newlines ---
void Labels::renderText(float x, float y, const char* text) {
    float startX = x;
    glRasterPos2f(x, y);

    while (*text) {
        if (*text == '\n') {
            // Move down by one line height
            y -= m_lineHeight;
            glRasterPos2f(startX, y);
        }
        else {
            glutBitmapCharacter(m_font, *text);
        }
        text++;
    }
}

// --- Helper: Draws a transparent black box ---
static void drawBackgroundBox(float x, float y, float w, float h) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.8f); // Darker background for readability

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
    // RIGHT PANEL: Coordinates (Top-Right)
    // ============================================================
    if (isDeveloperMode) {
        char coordBuffer[128];
        sprintf_s(coordBuffer, sizeof(coordBuffer), "X : %.1f   Y : %.1f   Z : %.1f", camX, camY, camZ);

        int textWidth = getTextWidth(coordBuffer);
        float padding = 30.0f;
        float boxWidth = textWidth + padding;
        float boxHeight = 30.0f;

        float rightMargin = 20.0f;
        float topMargin = 20.0f;
        float boxX = m_windowWidth - boxWidth - rightMargin;
        float boxY = m_windowHeight - topMargin;

        drawBackgroundBox(boxX, boxY, boxWidth, boxHeight);
        glColor3f(1.0f, 1.0f, 1.0f);
        renderText(boxX + (padding / 2), boxY - 20, coordBuffer);
    }

    // ============================================================
    // LEFT PANEL: Controls / Help (Top-Left)
    // ============================================================
    std::vector<HudLine> lines;

    if (!m_showHelp) {
        lines.push_back({ "Press 'Tab' to show controls", 1.0f, 1.0f, 1.0f });
    }
    else {
        lines.push_back({ "Press 'Tab' to hide controls", 1.0f, 1.0f, 1.0f });
        lines.push_back({ "", 1.0f, 1.0f, 1.0f });

        if (isDeveloperMode) {
            lines.push_back({ "[ DEVELOPER MODE ]", 1.0f, 0.5f, 0.5f });
            lines.push_back({ "", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "WASD       : Move (Fly)", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Q / E      : Fly Up/Down", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Arrows     : Look Around", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Shift      : Move Faster", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "T          : Toggle Axes", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "C          : Toggle Coords", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "P          : Switch to Game Mode", 1.0f, 1.0f, 1.0f });
        }
        else {
            lines.push_back({ "[ GAME MODE ]", 0.5f, 1.0f, 0.5f });
            lines.push_back({ "", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "W A S D    : Move (Walk)", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Mouse      : Look Around", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Space      : Jump", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "Shift      : Sprint", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "F          : Flashlight", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "E          : Interact", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "", 1.0f, 1.0f, 1.0f });
            lines.push_back({ "P          : Switch to Developer", 1.0f, 1.0f, 1.0f });
        }
    }

    int maxTextWidth = 0;
    for (const auto& line : lines) {
        int w = getTextWidth(line.text.c_str());
        if (w > maxTextWidth) maxTextWidth = w;
    }

    float padding = 30.0f;
    float boxWidth = maxTextWidth + padding;
    float boxHeight = (lines.size() * m_lineHeight) + 15.0f;
    if (lines.size() == 1) boxHeight = 30.0f;

    float leftMargin = 20.0f;
    float topMargin = 20.0f;
    float boxX = leftMargin;
    float boxY = m_windowHeight - topMargin;

    drawBackgroundBox(boxX, boxY, boxWidth, boxHeight);

    float textX = boxX + (padding / 2);
    float textY = boxY - 20;

    for (const auto& line : lines) {
        glColor3f(line.r, line.g, line.b);
        renderText(textX, textY, line.text.c_str());
        textY -= m_lineHeight;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

// ================================================================
// NEW: Draw Center Message (UPDATED for Multi-line)
// ================================================================
void Labels::drawCenterMessage(const char* message) {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, m_windowWidth, 0, m_windowHeight);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);

    // 1. Calculate Width (Max line width)
    int textWidth = getTextWidth(message);

    // 2. Calculate Height (Count newlines)
    int lineCount = 1;
    const char* ptr = message;
    while (*ptr) {
        if (*ptr == '\n') lineCount++;
        ptr++;
    }

    float padding = 40.0f;
    float boxWidth = textWidth + padding;
    float boxHeight = (lineCount * m_lineHeight) + padding;

    float centerX = m_windowWidth / 2.0f;
    float centerY = m_windowHeight / 2.0f;

    float boxX = centerX - (boxWidth / 2.0f);
    float boxY = centerY + (boxHeight / 2.0f);

    // Draw Background
    drawBackgroundBox(boxX, boxY, boxWidth, boxHeight);

    // Draw Text
    glColor3f(1.0f, 1.0f, 0.5f); // Pale Yellow

    // Adjust start Y position so text is vertically centered
    // Start at top of box minus padding
    float textY = boxY - (padding / 1.5f);
    renderText(centerX - (textWidth / 2.0f), textY, message);

    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}

// ================================================================
// NEW: Draw Action Hint
// ================================================================
void Labels::drawActionHint(const char* message) {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, m_windowWidth, 0, m_windowHeight);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING); glDisable(GL_DEPTH_TEST);

    int textWidth = getTextWidth(message);
    float centerX = m_windowWidth / 2.0f;
    float y = m_windowHeight / 4.0f;

    float padding = 20.0f;
    float boxWidth = textWidth + padding;
    float boxHeight = 30.0f;
    float boxX = centerX - (boxWidth / 2.0f);
    float boxY = y + 20.0f;

    drawBackgroundBox(boxX, boxY, boxWidth, boxHeight);

    glColor3f(1.0f, 1.0f, 1.0f); // White Text
    renderText(centerX - (textWidth / 2.0f), y, message);

    glEnable(GL_DEPTH_TEST); glEnable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
}