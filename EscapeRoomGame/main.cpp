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
#include "TheRoom.h"       // <-- Include TheRoom header

//--- OpenGL Libraries ---
#include <glut.h>
#include <SOIL2.h> 

// --- FIX: Define LOD Bias Constant for Compatibility ---
// This is required if your standard headers don't define it (0x8501 is the raw value)
#ifndef GL_TEXTURE_LOD_BIAS_EXT
#define GL_TEXTURE_LOD_BIAS_EXT 0x8501 
#endif

// --- Global Variables ---
int win_width = 1024;
int win_height = 720;

// Delta-time calculation
int g_lastTime = 0;

// Pointers to module objects
Camera* g_camera = nullptr;
Labels* g_labels = nullptr;
TheRoom* g_room = nullptr;

// Debug toggle flags
bool g_showAxes = false;        // Start with axes hidden
bool g_showCoordinates = false; // Start with coordinates hidden

// --- Function Declarations ---
void display();
void reshape(int w, int h);
void init();
void setupCollisionGrid();
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

	// Create Module objects *after* glutInit
	g_camera = new Camera(win_width, win_height);
	g_labels = new Labels(win_width, win_height);
	g_room = new TheRoom(GRID_SIZE, 5.0f, GRID_SIZE);

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
	init();
	g_camera->init();

	// 4. Start the Main Game Loop
	g_lastTime = glutGet(GLUT_ELAPSED_TIME);
	glutMainLoop();

	// 5. Clean up memory
	delete g_camera;
	delete g_labels;
	delete g_room;
	g_camera = nullptr;
	g_labels = nullptr;
	g_room = nullptr;

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
}


// ================================================================
// Initialize OpenGL Function (Updated with Optimization)
// =================================================================
void init() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey background
	glEnable(GL_DEPTH_TEST);



	// OPTIMIZATION: Use GL_MULTISAMPLE if available 
	glEnable(GLUT_MULTISAMPLE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	// --- Lighting Setup (Ambient Only) ---
	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);

	// 1. GLOBAL AMBIENT: HIGH AMBIENT FOR VISIBILITY (The only light source)
	GLfloat global_ambient[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	// --- Material Properties Setup ---
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat mat_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat mat_shininess = 0.0f;
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// --- OPTIMIZATION: Mipmap Level of Detail (LOD) Bias ---
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS_EXT, 10.0f); // <-- Aggressive bias for performance

	glColor3f(1.0f, 1.0f, 1.0f); // Set default draw color to white

	// --- Load Room Textures ---
	if (g_room) {
		// Loading using optimized DDS filenames
		g_room->loadTextures(
			"textures/floor.dds",
			"textures/wall.dds",
			"textures/ceiling.dds"
		);

		// =========================================================
		// [IMPORTANT] BUILD THE OPTIMIZED DISPLAY LIST HERE
		// =========================================================
		g_room->build();
	}

	// --- Collision Grid Setup ---
	setupCollisionGrid();
}

// ================================================================
// Display Callback Function
// ================================================================
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	g_camera->applyView();

	// --- Draw 3D Scene ---
	if (g_showAxes) {
		drawAxes(GRID_HALF_SIZE);
	}
	// drawGrid is omitted for optimization unless coordinates are on
	if (g_showCoordinates) {
		drawGrid(GRID_SIZE, GRID_SEGMENTS);
		drawGridCoordinates(GRID_SIZE, GRID_SEGMENTS);
	}

	// Draw TheRoom
	if (g_room) {
		g_room->draw(); // This draws the room geometry (using the Display List if available)
	}

	// --- Draw 2D UI (Labels) ---
	g_labels->draw(g_camera->isDeveloperMode());

	glutSwapBuffers();
}

// ================================================================
// Reshape Callback Function
// ================================================================
void reshape(int w, int h) {
	win_width = w;
	win_height = h;
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / h, 0.1, 100.0);

	// Notify modules of resize
	g_camera->onWindowResize(w, h);
	g_labels->onWindowResize(w, h);
}

// ================================================================
// Idle Callback Function
// ================================================================
void idle() {
	// 1. Get current time in milliseconds
	int currentTime = glutGet(GLUT_ELAPSED_TIME);

	// 2. Calculate time passed since last frame (in seconds)
	float dt = (currentTime - g_lastTime) / 1000.0f;

	// --- OPTIMIZATION: Frame Rate Limiter (CPU Saver) ---
	// Target: 60 Frames Per Second.
	// Math: 1.0 second / 60 frames = 0.0166 seconds per frame.
	// If less than 0.016s (16ms) has passed, we wait.
	if (dt < 0.016f) {
		return;
	}
	// ----------------------------------------------------

	// 3. Cap delta-time to prevent physics glitches if lag occurs
	if (dt > 0.1f) dt = 0.1f;

	// 4. Update the time tracker
	g_lastTime = currentTime;

	// 5. Update Game Logic (Physics, Movement, Collision)
	if (g_camera) {
		g_camera->update(dt);
	}

	// 6. Request a redraw for the next frame
	glutPostRedisplay();
}


// ================================================================
// INPUT FUNCTIONS
// ================================================================

void keyboard(unsigned char key, int x, int y) {
	// IMPORTANT: Update modifiers FIRST in key/special callbacks
	g_camera->updateModifiers(glutGetModifiers());

	if (key == 27) { // ESC Key
		printf("ESC key pressed. Exiting.\n");
		delete g_camera;
		delete g_labels;
		delete g_room;
		exit(0);
	}
	if (key == '\t') { // Tab Key
		g_labels->toggleHelp();
		return;
	}
	// Toggle Axes ('T') - Dev Mode Only
	if (key == 't' || key == 'T') {
		if (g_camera->isDeveloperMode()) {
			g_showAxes = !g_showAxes;
			printf("DEBUG: Axes %s\n", g_showAxes ? "ON" : "OFF");
			glutPostRedisplay();
		}
		return;
	}
	// Toggle Coordinates ('C') - Dev Mode Only
	if (key == 'c' || key == 'C') {
		if (g_camera->isDeveloperMode()) {
			g_showCoordinates = !g_showCoordinates;
			printf("DEBUG: Coordinates %s\n", g_showCoordinates ? "ON" : "OFF");
			glutPostRedisplay();
		}
		return;
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