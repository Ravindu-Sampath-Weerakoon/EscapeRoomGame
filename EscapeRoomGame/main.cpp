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
#include "InsideWall.h"    // Internal Walls
#include "CornerTower.h"   // Corner Towers
#include "SecretBook.h"    // <-- NEW: Secret Lore Books
#include "GraphicsUtils.h" // Includes grid constants and collision functions
#include "Cameras.h"
#include "Labels.h"
#include "TheRoom.h"


//--- OpenGL Libraries ---
#include <glut.h>
#include <SOIL2.h> 

// --- FIX: Define LOD Bias Constant for Compatibility ---
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
InsideWall* g_insideWalls = nullptr;
CornerTower* g_tower = nullptr;
SecretBook* g_book = nullptr; // <-- NEW: Pointer for book system

// Game State
bool g_flashlightOn = true;     // Starts with flashlight ON
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
	g_insideWalls = new InsideWall(5.0f);
	g_tower = new CornerTower(5.0f, 1.5f); // Height 5.0, Width 1.5
	g_book = new SecretBook(); // <-- NEW: Initialize Book System

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
	g_camera->setGroundLevel(1.8f);
	g_camera->setPosition(-18.0f, -18.0f);

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
	delete g_insideWalls;
	delete g_tower;
	delete g_book; // <-- NEW: Clean up
	g_camera = nullptr;
	g_labels = nullptr;
	g_room = nullptr;
	g_insideWalls = nullptr;
	g_tower = nullptr;
	g_book = nullptr;

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
// Initialize OpenGL Function
// =================================================================
void init() {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark grey background
	glEnable(GL_DEPTH_TEST);

	// OPTIMIZATION: Use GL_MULTISAMPLE if available 
	glEnable(GLUT_MULTISAMPLE);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	// --- LIGHTING SETUP ---
	glEnable(GL_LIGHTING);

	// 1. Initial Global Ambient (Will be updated in display loop)
	GLfloat global_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

	// 2. Material Properties (High Shininess for Towers)
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess = 50.0f; // Shiny metal look
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// 3. FLASHLIGHT SETUP (GL_LIGHT1)
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 0.9f, 1.0f };
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0f);   // 25 degree beam
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 10.0f); // Focused center

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.05f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.005f);

	glEnable(GL_LIGHT1);

	// --- OPTIMIZATION: Mipmap Level of Detail (LOD) Bias ---
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS_EXT, -0.5f);
	glColor3f(1.0f, 1.0f, 1.0f);

	// --- Load Room Textures ---
	if (g_room) {
		g_room->loadTextures(
			"textures/floor.dds",
			"textures/wall.dds",
			"textures/ceiling.dds"
		);
		g_room->build();
	}

	// --- Setup Inside Walls (Your Layout) ---
	if (g_insideWalls && g_room) {
		// 1. Vertical Wall (Left side)
		g_insideWalls->addWall(-20.0f, -16.0f, 16.0f, -16.0f, 0.5f);

		// 2. Horizontal Wall 
		g_insideWalls->addWall(-16.0f, 0.0f, 16.0f, 0.0f, 0.5f);

		// 3. Other segments
		g_insideWalls->addWall(-16.0f, 16.0f, 20.0f, 16.0f, 0.5f);
		g_insideWalls->addWall(-16.0f, 0.0f, -16.0f, 16.0f, 0.5f);
		g_insideWalls->addWall(0.0f, 0.0f, 0.0f, -12.0f, 0.5f);

		g_insideWalls->build(g_room->getWallTextureID());
	}

	// --- Setup Corner Towers (Your Layout) ---
	if (g_tower && g_room) {
		// Adding towers to specific locations inside the maze
		g_tower->addTower(16.0f, -16.0f);
		g_tower->addTower(16.0f, 0.0f);
		g_tower->addTower(0.0f, -12.0f);
		g_tower->addTower(-16.0f, 16.0f);
		g_tower->addTower(-16.0f, 0.0f);

		g_tower->build(g_room->getWallTextureID());
	}

	// --- NEW: Setup Secret Books ---
	// Add books at interesting locations with story text
	if (g_book) {
		// Book 1: Near spawn
		g_book->addBook(-18.0f, -15.0f, "Day 1: I woke up here. The walls are cold.");

		// Book 2: In the middle corridor
		g_book->addBook(0.0f, 0.0f, "Day 3: The darkness is moving. I need a key.");

		// Book 3: Near a tower
		g_book->addBook(14.0f, -14.0f, "HINT: Look behind the pillars.");
	}

	// --- Collision Grid Setup (Boundaries) ---
	setupCollisionGrid();
}

// ================================================================
// Display Callback Function (Dynamic Lighting)
// ================================================================
void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// --- DYNAMIC LIGHTING LOGIC ---
	// Default: Dark, scary atmosphere
	GLfloat currentAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	if (g_camera->isDeveloperMode()) {
		// Developer Mode: Full Brightness
		currentAmbient[0] = 0.6f; currentAmbient[1] = 0.6f; currentAmbient[2] = 0.6f;
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		// Game Mode: Depends on Flashlight
		if (g_flashlightOn) {
			glEnable(GL_LIGHT1);
			glEnable(GL_LIGHT2);
			// Ambient stays at 0.3
		}
		else {
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
			// Extremely dark when flashlight is off
			currentAmbient[0] = 0.05f; currentAmbient[1] = 0.05f; currentAmbient[2] = 0.05f;
		}
	}
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, currentAmbient);

	if (g_flashlightOn || g_camera->isDeveloperMode()) {
		// --- LIGHT 1: THE SPOTLIGHT (Torch) ---
		if (glIsEnabled(GL_LIGHT1)) {
			// POSITION FIX: Move light 0.5 units BEHIND the camera (+Z) to fix dark wall issue
			GLfloat spot_pos[] = { 0.0f, 0.0f, 0.5f, 1.0f };
			GLfloat spot_dir[] = { 0.0f, 0.0f, -1.0f };

			glLightfv(GL_LIGHT1, GL_POSITION, spot_pos);
			glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);

			// WIDER BEAM & SOFTER EDGE
			glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 70.0f);
			glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 20.0f);

			// ATTENUATION: Very slow fade
			glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.8f);
			glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.02f);
			glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0f);
		}

		// --- LIGHT 2: THE PLAYER AURA (Lantern) ---
		if (glIsEnabled(GL_LIGHT2)) {
			GLfloat aura_pos[] = { 0.0f, 0.5f, 0.0f, 1.0f };
			// Brighter, warmer color
			GLfloat aura_color[] = { 1.0f, 0.95f, 0.8f, 1.0f };
			glLightfv(GL_LIGHT2, GL_DIFFUSE, aura_color);
			glLightfv(GL_LIGHT2, GL_SPECULAR, aura_color);
			glLightfv(GL_LIGHT2, GL_POSITION, aura_pos);

			// Omnidirectional
			glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 180.0f);

			// ATTENUATION: Realistic falloff
			glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.0f);
			glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.02f);
			glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.002f);
		}
	}
	// ------------------------------

	g_camera->applyView();

	// --- Draw Scene ---
	if (g_showAxes) {
		drawAxes(GRID_HALF_SIZE);
	}

	if (g_showCoordinates) {
		drawGrid(GRID_SIZE, GRID_SEGMENTS);
		drawGridCoordinates(GRID_SIZE, GRID_SEGMENTS);
	}

	if (g_room) g_room->draw();
	if (g_insideWalls) g_insideWalls->draw();
	if (g_tower) g_tower->draw();

	// --- NEW: Draw Secret Books ---
	if (g_book) {
		g_book->draw();

		// Check for interaction
		if (g_camera) {
			int nearIndex = g_book->getNearestBookIndex(g_camera->getX(), g_camera->getZ());
			if (nearIndex != -1) {
				// If near a book, check if it's open
				if (g_book->isBookOpen(nearIndex)) {
					// Show the book's secret message
					g_labels->drawCenterMessage(g_book->getBookMessage(nearIndex));
				}
				else {
					// Show interaction hint
					g_labels->drawActionHint("Press 'E' to Read");
				}
			}
		}
	}

	// --- Draw 2D UI (Labels) ---
	if (g_labels && g_camera) {
		g_labels->draw(
			g_camera->isDeveloperMode(),
			g_camera->getX(),
			g_camera->getY(),
			g_camera->getZ()
		);
	}

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

	g_camera->onWindowResize(w, h);
	g_labels->onWindowResize(w, h);
}

// ================================================================
// Idle Callback Function (Optimized)
// ================================================================
void idle() {
	// 1. Get current time in milliseconds
	int currentTime = glutGet(GLUT_ELAPSED_TIME);

	// 2. Calculate time passed since last frame (in seconds)
	float dt = (currentTime - g_lastTime) / 1000.0f;

	// --- OPTIMIZATION: Frame Rate Limiter (CPU Saver) ---
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

	// --- NEW: Update Book Animations ---
	if (g_book) {
		g_book->update(dt);
	}

	// 6. Request a redraw for the next frame
	glutPostRedisplay();
}


// ================================================================
// INPUT FUNCTIONS
// ================================================================

void keyboard(unsigned char key, int x, int y) {
	g_camera->updateModifiers(glutGetModifiers());

	if (key == 27) { // ESC Key
		printf("ESC key pressed. Exiting.\n");
		delete g_camera;
		delete g_labels;
		delete g_room;
		delete g_insideWalls;
		delete g_tower;
		delete g_book; // <-- NEW: Clean up
		exit(0);
	}
	if (key == '\t') { // Tab Key
		g_labels->toggleHelp();
		return;
	}

	// Flashlight Toggle ('F')
	if (key == 'f' || key == 'F') {
		g_flashlightOn = !g_flashlightOn;
		printf("Flashlight: %s\n", g_flashlightOn ? "ON" : "OFF");
	}

	// --- NEW: Interaction Key ('E') ---
	if (key == 'e' || key == 'E') {
		if (g_book && g_camera) {
			// Check if we are near any book
			int nearIndex = g_book->getNearestBookIndex(g_camera->getX(), g_camera->getZ());
			if (nearIndex != -1) {
				// Toggle that specific book (Open/Close)
				g_book->toggleBook(nearIndex);
				printf("Interacted with Book %d\n", nearIndex);
			}
		}
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
	g_camera->onMouseMovement(x, y);
}