#pragma once

// We get <glut.h> from our precompiled header
#include "pch.h"

class Labels {
public:
    /**
     * @brief Constructor.
     * @param windowWidth The starting width of your game window.
     * @param windowHeight The starting height of your game window.
     */
    Labels(int windowWidth, int windowHeight);

    /**
     * @brief Call this from your main display() function to draw all labels.
     * @param isDeveloperMode Pass in the camera's current mode.
     */
    void draw(bool isDeveloperMode);

    /**
     * @brief Call this from your keyboard() function when Tab is pressed.
     */
    void toggleHelp();

    /**
     * @brief Call this from your main reshape() function.
     */
    void onWindowResize(int w, int h);

private:
    /**
     * @brief Internal helper to draw one line of text.
     */
    void renderText(float x, float y, const char* text);

    // --- State ---
    bool m_showHelp; // Tracks if the Tab menu is open

    // --- Window Info ---
    int m_windowWidth;
    int m_windowHeight;

    // --- Font Info ---
    void* m_font;
    int m_lineHeight;
};