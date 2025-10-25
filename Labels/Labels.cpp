#include "pch.h" // Must be first
#include "Labels.h"

Labels::Labels(int windowWidth, int windowHeight) {
    onWindowResize(windowWidth, windowHeight);

    m_showHelp = false; // Start with the help text hidden

    // Set a good, readable font and its height
    m_font = GLUT_BITMAP_9_BY_15;
    m_lineHeight = 17; // 15px font + 2px spacing
}

void Labels::onWindowResize(int w, int h) {
    m_windowWidth = w;
    m_windowHeight = (h == 0) ? 1 : h; // Prevent divide by zero
}

void Labels::toggleHelp() {
    m_showHelp = !m_showHelp; // Flip the state
}

void Labels::renderText(float x, float y, const char* text) {
    // Set the 2D position to start drawing
    glRasterPos2f(x, y);

    // Loop through the string and draw each character
    while (*text) {
        glutBitmapCharacter(m_font, *text);
        text++;
    }
}

void Labels::draw(bool isDeveloperMode) {
    // --- This is the most important part ---
    // 1. Save the 3D projection and modelview matrices
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    // 2. Set up a 2D "orthographic" (flat) view
    gluOrtho2D(0, m_windowWidth, 0, m_windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 3. Disable 3D lighting and depth testing
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    // 4. Draw the text
    glColor3f(1.0f, 1.0f, 0.9f); // A nice off-white color

    float x = 10.0f; // 10 pixels from the left
    float y = m_windowHeight - m_lineHeight - 10; // 10 pixels from the top

    //// Draw the static hint
    //renderText(x, y, "Press 'P' to switch camera mode");
    //y -= m_lineHeight; // Move down one line

    if (!m_showHelp) {
        renderText(x, y, "Press 'Tab' to show controls");
    }
    else
    {
        renderText(x, y, "Press 'Tab' to hide controls");
    }
    

    // Draw the dynamic help text if 'Tab' is pressed
    if (m_showHelp) {
        y -= m_lineHeight * 2; // Add a blank line

        if (isDeveloperMode) {
            glColor3f(1.0f, 0.8f, 0.8f); // Red-ish tint for Dev
            renderText(x, y, "[ YOU ARE INDEVELOPER MODE ]");
            y -= m_lineHeight*2;
            glColor3f(1.0f, 1.0f, 0.9f);
            renderText(x, y, " WASD: Move (Fly)");
            y -= m_lineHeight;
            renderText(x, y, " Q/E: Fly Up/Down");
            y -= m_lineHeight;
            renderText(x, y, " Arrows: Look Around");
            y -= m_lineHeight;
            renderText(x, y, " Shift: Move Faster");
            y -= m_lineHeight;
            renderText(x, y, " T : on/off Axise");
            y -= m_lineHeight;
            renderText(x, y, " C : on/off Axise Coordinate");

            y -= m_lineHeight * 2;
            renderText(x, y, " Press 'P' For Game mode");
        }
        else {
            glColor3f(0.8f, 1.0f, 0.8f); // Green-ish tint for Game
         
   
            renderText(x, y, "[ YOU ARE IN GAME MODE ]");
            y -= m_lineHeight*2;
            glColor3f(1.0f, 1.0f, 0.9f);
            renderText(x, y, " W A S D: Move (Walk)");
            y -= m_lineHeight;
            renderText(x, y, " Mouse: Look Around");
            y -= m_lineHeight;
            renderText(x, y, " Space: Jump");
            y -= m_lineHeight;
            renderText(x, y, " Shift: Sprint(Run)");
            y -= m_lineHeight*2;
            renderText(x, y, "Press 'P' For Developer Mode");
           
            
        }
    }

    // 5. Re-enable 3D settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);

    // 6. Restore the 3D matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}