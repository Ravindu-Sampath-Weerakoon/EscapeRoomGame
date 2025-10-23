#pragma once // Use this instead of the old #ifndef...

/**
 * @brief Draws X, Y, and Z axes centered at the origin.
 * @param length The length of each axis from the origin.
 */
void drawAxes(float length);
void display();
void reshape(int w, int h);
void init();
void keyboard(unsigned char key, int x, int y);
void idle();

// Add other function declarations here later...
