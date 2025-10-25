// ----------------------------------------------------------------
// EscapeRoomGame.cpp
//
// Main entry point for the escape room game.
// Handles window creation, OpenGL initialization, and callbacks.
// ----------------------------------------------------------------

// MUST be the first include in your .cpp file
#include "pch.h"

// --- Standard libraries ---
#include <iostream>
#include <stdio.h> // For printf

// --- Your Custom Game Modules ---
#include "GraphicsUtils.h" // Includes grid constants and collision functions
#include "Cameras.h"
#include "Labels.h"

//--- OpenGL Libraries ---
#include <glut.h>

// --- Global Variables ---
int win_width = 1024;
int win_height = 720;

// Delta-time calculation
int g_lastTime = 0;

// Pointers to module objects
Camera* g_camera = nullptr;
Labels* g_labels = nullptr;

// Debug toggle flags
bool g_showAxes = true;        // Start with axes visible
bool g_showCoordinates = false; // Start with coordinates hidden

// --- Function Declarations ---
void display();
void reshape(int w, int h);
void init();
void setupCollisionGrid(); // <-- Declare the new function
void idle();
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void specialKeysUp(int key, int x, int y);
void mouseMotion(int x, int y);


void welcomeConsoleMessage() {
	std::cout << "Welcome to the Escape Room Game!" << std::endl;
}

// ================================================================
// MAIN FUNCTION
// ================================================================
int main(int argc, char** argv) {
	welcomeConsoleMessage();
	printf("Starting Escape Room Game...\n");

	// 1. Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE); // Request multisampling

	// Create Camera and Labels objects *after* glutInit
	g_camera = new Camera(win_width, win_height);
	g_labels = new Labels(win_width, win_height);

	// Center the window
	int screen_width = glutGet(GLUT_SCREEN_WIDTH);
	int screen_height = glutGet(GLUT_SCREEN_HEIGHT);
	int pos_x = (screen_width - win_width) / 2;
	int pos_y = (screen_height - win_height) / 2;
	glutInitWindowPosition(pos_x, pos_y);

	glutInitWindowSize(win_width, win_height);
	glutCreateWindow("Escape Room Game");

	// 2. Register Callback Functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(specialKeys);
	glutSpecialUpFunc(specialKeysUp);
	glutPassiveMotionFunc(mouseMotion); // For mouse look

	// Configure the Camera's starting state
	g_camera->setGroundLevel(1.5f);
	g_camera->setPosition(0.0f, 1.5f, 5.0f);

	// 3. Call one-time setup functions
	init();            // OpenGL setup
	g_camera->init();  // Camera setup requiring window

	// 4. Start the Main Game Loop
	g_lastTime = glutGet(GLUT_ELAPSED_TIME); // Init timer for dt
	glutMainLoop();

	// 5. Clean up memory (though MainLoop never exits)
	delete g_camera;
	delete g_labels;
	g_camera = nullptr;
	g_labels = nullptr;

	return 0;
}

// ================================================================
// Setup Collision Grid Function
// ================================================================
void setupCollisionGrid() {
	printf("Initializing collision grid...\n");

	// Block the Boundary Walls
	for (int i = 0; i < GRID_SEGMENTS; ++i) {
		addBlockGridBox(0, i); // Left Wall (X=0)
		addBlockGridBox(GRID_SEGMENTS - 1, i); // Right Wall (X=Max)
		if (i > 0 && i < GRID_SEGMENTS - 1) { // Avoid double-blocking corners
			addBlockGridBox(i, 0); // Back Wall (Z=0)
			addBlockGridBox(i, GRID_SEGMENTS - 1); // Front Wall (Z=Max)
		}
	}
	printf("Boundary walls marked as blocked.\n");

	// Block specific internal cells
	addBlockGridBox(4, 11);
	printf("Internal cell (4, 11) marked as blocked.\n");

	// Add more addBlockGridBox calls here for tables, obstacles etc.
	// Example: addBlockGridBox(5, 10);
}


// ================================================================
// Initialize OpenGL Function
// ================================================================
void init() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey background
	glEnable(GL_DEPTH_TEST);              // Enable Z-buffering
	
	//not work my pc and universiy pc
	//glEnable(GL_MULTISAMPLE);             // Enable Antialiasing


	// 1. Enable Blending (Required for line/polygon smoothing)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Standard alpha blending

	// 2. Enable Line Smoothing
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); // Ask for best quality (optional)

	// 3. Enable Polygon Smoothing (Use with Caution!)
	 glEnable(GL_POLYGON_SMOOTH);
	 glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST); // Ask for best quality (optional)


	// --- Lighting ---
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);                  // Enable light source 0
	// Add light position/color settings here later
	// Example: GLfloat light_pos[] = { 0.0f, 5.0f, 0.0f, 1.0f };
	// glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	glColor3f(1.0f, 1.0f, 1.0f);          // Set default draw color to white

	// --- Collision Grid Setup ---
	setupCollisionGrid();                 // Call the separate setup function
}

// ================================================================
// Display Callback Function
// ================================================================
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Apply camera view transformation
	g_camera->applyView();

	// --- Draw 3D Scene ---
	if (g_showAxes) {
		drawAxes(GRID_HALF_SIZE); // Use grid constant for size
	}
	drawGrid(GRID_SIZE, GRID_SEGMENTS); // Use grid constants
	if (g_showCoordinates) {
		drawGridCoordinates(GRID_SIZE, GRID_SEGMENTS); // Use grid constants
	}
	// drawRoom(); // Add later

	// --- Draw 2D UI (Labels) ---
	g_labels->draw(g_camera->isDeveloperMode());

	// Swap buffers to show the drawn frame
	glutSwapBuffers();
}

// ================================================================
// Reshape Callback Function
// ================================================================
void reshape(int w, int h) {
	win_width = w;
	win_height = h;
	if (h == 0) h = 1; // Prevent division by zero

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / h, 0.1, 100.0); // Field of View, Aspect Ratio, Near, Far

	// Notify modules of resize
	g_camera->onWindowResize(w, h);
	g_labels->onWindowResize(w, h);
}

// ================================================================
// Idle Callback Function
// ================================================================
void idle() {
	// Calculate Delta-Time (dt)
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float dt = (currentTime - g_lastTime) / 1000.0f; // Time in seconds
	if (dt > 0.1f) dt = 0.1f; // Clamp dt to prevent large jumps
	g_lastTime = currentTime;

	// Update Camera (handles movement, physics, smoothing, collision)
	g_camera->update(dt);

	// Request a redraw for the next frame
	glutPostRedisplay();
}


// ================================================================
// INPUT FUNCTIONS
// ================================================================

void keyboard(unsigned char key, int x, int y) {
	// Update modifiers FIRST in every input callback (EXCEPT mouseMotion)
	g_camera->updateModifiers(glutGetModifiers());

	if (key == 27) { // ESC Key
		printf("ESC key pressed. Exiting.\n");
		delete g_camera;
		delete g_labels;
		exit(0);
	}
	if (key == '\t') { // Tab Key
		g_labels->toggleHelp();
		return; // Consume the Tab key
	}
	// Toggle Axes ('T') - Dev Mode Only
	if (key == 't' || key == 'T') {
		if (g_camera->isDeveloperMode()) {
			g_showAxes = !g_showAxes;
			printf("DEBUG: Axes %s\n", g_showAxes ? "ON" : "OFF");
			glutPostRedisplay();
		}
		return; // Consume 't' key
	}
	// Toggle Coordinates ('C') - Dev Mode Only
	if (key == 'c' || key == 'C') {
		if (g_camera->isDeveloperMode()) {
			g_showCoordinates = !g_showCoordinates;
			printf("DEBUG: Coordinates %s\n", g_showCoordinates ? "ON" : "OFF");
			glutPostRedisplay();
		}
		return; // Consume 'c' key
	}

	// Pass other keys to the camera
	g_camera->onKeyDown(key);
}

void keyboardUp(unsigned char key, int x, int y) {
	g_camera->updateModifiers(glutGetModifiers());
	g_camera->onKeyUp(key);
}

void specialKeys(int key, int x, int y) {
	g_camera->updateModifiers(glutGetModifiers());
	g_camera->onSpecialKeyDown(key);
}

void specialKeysUp(int key, int x, int y) {
	g_camera->updateModifiers(glutGetModifiers());
	g_camera->onSpecialKeyUp(key);
}

void mouseMotion(int x, int y) {
	// DO NOT call updateModifiers here to avoid GLUT warnings
	g_camera->onMouseMovement(x, y);
}