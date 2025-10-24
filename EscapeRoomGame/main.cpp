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

// 2. CREATE A GLOBAL CAMERA POINTER
// We create it as a pointer and initialize it to nullptr.
// The actual object will be created inside main() AFTER glutInit.
Camera* g_camera = nullptr;


// --- Global Pointers ---
Labels* g_labels = nullptr; //  2. CREATE A GLOBAL POINTER FOR LABELS


// --- Function Declarations ---
// We need to tell C++ about these functions before main() uses them
void display();
void reshape(int w, int h);
void init();
void idle();

// Input callback declarations
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void specialKeysUp(int key, int x, int y);
void mouseMotion(int x, int y);


void welcomeConsoleMessage() {
	std::cout << "Welcome to the Escape Room Game!" << std::endl;
}

// ================================================================
// MAIN FUNCTION (Corrected)
// ================================================================
int main(int argc, char** argv) {
	welcomeConsoleMessage();
	printf("Starting Escape Room Game...\n");

	// 1. Initialize GLUT
	glutInit(&argc, argv); // <-- GLUT is initialized
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);

	// THIS IS THE NEW LINE:
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE); // <-- Add GLUT_MULTISAMPLE

	// --- FIX: CREATE THE CAMERA OBJECT *AFTER* glutInit ---
	// This gives the g_camera pointer a valid object to point to.
	g_camera = new Camera(win_width, win_height);
	// 3. CREATE THE LABELS OBJECT
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
	// These functions tell GLUT what to do in different situations
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(specialKeys);
	glutSpecialUpFunc(specialKeysUp);
	glutPassiveMotionFunc(mouseMotion); // For mouse look

	// --- Configure the Camera ---
	// Use the -> operator because g_camera is a pointer
	g_camera->setGroundLevel(1.5f);
	g_camera->setPosition(0.0f, 1.5f, 5.0f);

	// 3. Call your one-time setup function
	init();

	// Call the camera's init() function
	// This is the function we added to set the cursor
	g_camera->init();

	// 4. Start the Main Game Loop
	g_lastTime = glutGet(GLUT_ELAPSED_TIME); // Init timer for delta-time
	glutMainLoop();

	// 5. Clean up memory
	delete g_camera;
	delete g_labels;

	g_camera = nullptr;
	g_labels = nullptr;

	return 0; // Will never be reached
}


// ================================================================
// CALLBACK FUNCTIONS
// ================================================================

/**
 * @brief Main drawing function (called every frame)
 */
void display() {
	// Clear the screen (color and depth buffers)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set up the camera (ModelView matrix)
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// === USE THE CAMERA ===
	// Use -> because g_camera is a pointer
	g_camera->applyView();

	// --- Draw your scene here ---

	// Call your function from the GraphicsUtils module!
	drawAxes(40.0f);


	// 👇 Draw the new grid
	// This creates a 40x40 unit grid with 40 lines
	drawGrid(40.0f, 40);

	// Later, you will add:
	// drawRoom();
	// drawTable();


	// --- 2D UI (Draw this last!) ---
	// 👈 5. CALL THE LABELS DRAW FUNCTION
	// Ask the camera for its mode and pass it to the labels
	g_labels->draw(g_camera->isDeveloperMode());

	// --- End of scene ---

	// Swap the front and back buffers to display the new frame
	glutSwapBuffers();
}

/**
 * @brief Called when the window is resized
 */
void reshape(int w, int h) {
	win_width = w;
	win_height = h;

	// Prevent division by zero
	if (h == 0) h = 1;

	// Set the viewport to match the new window size
	glViewport(0, 0, w, h);

	// Set up the projection matrix (the camera's lens)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(
		60.0,                  // 60-degree Field of View (FOV)
		(float)w / (float)h,   // Aspect ratio (width / height)
		0.1,                   // Near clipping plane
		100.0                  // Far clipping plane
	);

	// Notify the camera of the resize
	g_camera->onWindowResize(w, h);

	// NOTIFY LABELS OF RESIZE
	g_labels->onWindowResize(w, h); 
}

/**
 * @brief One-time OpenGL setup
 */
void init() {
	// Set a dark grey background color
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	// Enable the depth test (so objects draw in the correct order)
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

	// Enable basic lighting
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0); // Turn on one light source

	// Set all objects to have a white color by default
	glColor3f(1.0f, 1.0f, 1.0f);

	//  ADD THIS LINE TO ENABLE SMOOTHING
	/*glEnable(GL_MULTISAMPLE);*/
	//work this line
	glEnable(GLUT_MULTISAMPLE);
}

/**
 * @brief Called continuously when no other events are happening
 */
void idle() {
	// --- Calculate Delta-Time (dt) ---
	// This is the time in seconds since the last frame
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float dt = (currentTime - g_lastTime) / 1000.0f; // Convert milliseconds to seconds
	g_lastTime = currentTime;

	// --- Update the Camera ---
	// This handles all movement, physics, and input logic
	g_camera->update(dt);

	// Tell OpenGL that the screen needs to be redrawn
	glutPostRedisplay();
}


// ================================================================
// INPUT FUNCTIONS
// These just pass the input along to the camera class.
// ================================================================

void keyboard(unsigned char key, int x, int y) {
	// 27 is the ASCII code for the ESCAPE key
	if (key == 27) {
		printf("ESC key pressed. Exiting.\n");
		//7. CLEAN UP ON EXIT// Clean up memory
		delete g_camera;
		delete g_labels; 
		exit(0); // Quit the program
	}

	//8. ADD THE TAB KEY
	if (key == '\t') { // '\t' is the character for the Tab key
		g_labels->toggleHelp();
		return; // Don't pass the Tab key to the camera
	}

	// Pass all other keys to the camera
	g_camera->onKeyDown(key);
}

void keyboardUp(unsigned char key, int x, int y) {
	g_camera->onKeyUp(key);
}

void specialKeys(int key, int x, int y) {
	g_camera->onSpecialKeyDown(key);
}

void specialKeysUp(int key, int x, int y) {
	g_camera->onSpecialKeyUp(key);
}

void mouseMotion(int x, int y) {
	g_camera->onMouseMovement(x, y);
}