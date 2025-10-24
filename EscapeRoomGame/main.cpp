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
#include "GraphicsUtils.h"
#include "Cameras.h"
#include "Labels.h"

//--- OpenGL Libraries ---
#include <glut.h>

// --- Global Variables ---
int win_width = 1024;
int win_height = 720;

// We need to calculate delta-time
int g_lastTime = 0;

// Global Camera Pointer
Camera* g_camera = nullptr;

// Global Labels Pointer
Labels* g_labels = nullptr;

// --- Function Declarations ---
void display();
void reshape(int w, int h);
void init();
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
	glutInit(&argc, argv); // <-- GLUT is initialized
	// Request Double Buffering, Depth Buffer, RGBA Color, and Multisampling
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE); // <-- Correct single call

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
	glutCreateWindow("Escape Room Game"); // <-- Window is now created

	// 2. Register Callback Functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(specialKeys);
	glutSpecialUpFunc(specialKeysUp);
	glutPassiveMotionFunc(mouseMotion); // For mouse look

	// Configure the Camera
	g_camera->setGroundLevel(1.5f);
	g_camera->setPosition(0.0f, 1.5f, 5.0f);

	// 3. Call one-time setup functions
	init();
	g_camera->init(); // Camera setup that requires GLUT/Window

	// 4. Start the Main Game Loop
	g_lastTime = glutGet(GLUT_ELAPSED_TIME); // Init timer for delta-time
	glutMainLoop();

	// 5. Clean up memory (though MainLoop never exits)
	delete g_camera;
	delete g_labels;
	g_camera = nullptr;
	g_labels = nullptr;

	return 0;
}


// ================================================================
// CALLBACK FUNCTIONS
// ================================================================

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Apply camera view transformation
	g_camera->applyView();

	// --- Draw 3D Scene ---
	drawAxes(40.0f);
	drawGrid(40.0f, 40);
	// drawRoom(); // Add later

	// --- Draw 2D UI (Labels) ---
	g_labels->draw(g_camera->isDeveloperMode());

	// Swap buffers to show the drawn frame
	glutSwapBuffers();
}

void reshape(int w, int h) {
	win_width = w;
	win_height = h;
	if (h == 0) h = 1; // Prevent division by zero

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (float)w / h, 0.1, 100.0);

	// Notify modules of resize
	g_camera->onWindowResize(w, h);
	g_labels->onWindowResize(w, h);
}

void init() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey background
	glEnable(GL_DEPTH_TEST); // Enable Z-buffering

	// --- Antialiasing ---
	//glEnable(GL_MULTISAMPLE); // <-- CORRECT way to enable smoothing

	// --- Lighting ---
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); // Enable light source 0
	// You might add light position/color settings here later

	glColor3f(1.0f, 1.0f, 1.0f); // Set default draw color to white
}

void idle() {
	// Calculate Delta-Time (dt)
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float dt = (currentTime - g_lastTime) / 1000.0f; // Time in seconds
	// Prevent large dt jumps (e.g., after breakpoint)
	if (dt > 0.1f) dt = 0.1f;
	g_lastTime = currentTime;

	// Update Camera (handles movement, physics, smoothing)
	g_camera->update(dt);

	// Request a redraw for the next frame
	glutPostRedisplay();
}


// ================================================================
// INPUT FUNCTIONS
// ================================================================

void keyboard(unsigned char key, int x, int y) {
	// IMPORTANT: Update modifiers FIRST in every input callback
	g_camera->updateModifiers(glutGetModifiers());

	if (key == 27) { // ESC Key
		printf("ESC key pressed. Exiting.\n");
		delete g_camera;
		delete g_labels;
		exit(0);
	}
	if (key == '\t') { // Tab Key
		g_labels->toggleHelp();
		return; // Consume the Tab key, don't pass to camera
	}

	// Pass other keys to the camera
	g_camera->onKeyDown(key);
}

void keyboardUp(unsigned char key, int x, int y) {
	// Update modifiers
	g_camera->updateModifiers(glutGetModifiers());
	// Pass key up event to camera
	g_camera->onKeyUp(key);
}

void specialKeys(int key, int x, int y) {
	// Update modifiers
	g_camera->updateModifiers(glutGetModifiers());
	// Pass special key down event to camera
	g_camera->onSpecialKeyDown(key);
}

void specialKeysUp(int key, int x, int y) {
	// Update modifiers
	g_camera->updateModifiers(glutGetModifiers());
	// Pass special key up event to camera
	g_camera->onSpecialKeyUp(key);
}

void mouseMotion(int x, int y) {
	// Update modifiers
	//g_camera->updateModifiers(glutGetModifiers());
	// Pass mouse motion event to camera
	g_camera->onMouseMovement(x, y);
}