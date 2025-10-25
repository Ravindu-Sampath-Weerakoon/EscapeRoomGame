#pragma once // Use this instead of the old #ifndef...

/**
 * @brief Draws X, Y, and Z axes centered at the origin.
 * @param length The length of each axis from the origin.
 */
void drawAxes(float length);

/**
 * @brief Draws a grid of lines on the XZ plane, centered at the origin.
 * @param size The total width and depth of the grid (e.g., 20.0f).
 * @param numSegments The number of squares in each direction (e.g., 20).
 */
void drawGrid(float size, int numSegments);

/**
 * @brief Draws (X, Z) coordinate labels in the center of each grid square.
 * @param size The total width and depth of the grid (must match drawGrid).
 * @param numSegments The number of squares (must match drawGrid).
 */
void drawGridCoordinates(float size, int numSegments);
