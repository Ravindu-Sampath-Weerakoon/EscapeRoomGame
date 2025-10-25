#pragma once
#include "pch.h" // Gets <glut.h>

// --- Grid Constants ---
// Use 'extern' for global constants defined elsewhere (in GraphicsUtils.cpp)
extern const float GRID_SIZE;
extern const int   GRID_SEGMENTS;
extern const float GRID_CELL_SIZE;
extern const float GRID_HALF_SIZE;

// --- Function Declarations ---

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

/**
 * @brief Converts world X, Z coordinates to grid indices.
 * @param worldX World X coordinate.
 * @param worldZ World Z coordinate.
 * @param gridX Output: Grid column index.
 * @param gridZ Output: Grid row index.
 * @return True if the coordinates are within the grid bounds, false otherwise.
 */
bool worldToGrid(float worldX, float worldZ, int& gridX, int& gridZ);

/**
 * @brief Checks if a target world position (X, Z) corresponds to a blocked grid cell.
 * @param worldX World X coordinate.
 * @param worldZ World Z coordinate.
 * @return True if the cell is blocked or outside the grid, false otherwise.
 */
bool isGridPositionBlocked(float worldX, float worldZ);

/**
 * @brief Marks a grid cell as blocked using integer grid coordinates.
 * @param gridX The X index of the cell (column).
 * @param gridZ The Z index of the cell (row).
 */
void addBlockGridBox(int gridX, int gridZ);

/**
 * @brief Marks a grid cell as unblocked (walkable) using integer grid coordinates.
 * @param gridX The X index of the cell (column).
 * @param gridZ The Z index of the cell (row).
 */
void removeBlockGridBox(int gridX, int gridZ);