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
#include <stdio.h> 
#include <string>  

// --- Your Custom Game Modules ---
#include "InsideWall.h"    
#include "CornerTower.h"   
#include "SecretBook.h"    
#include "SecretDoor.h"    
#include "RoomDecorations.h" // <-- NEW: Room Decorations
#include "GraphicsUtils.h" 
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
SecretBook* g_book = nullptr;
SecretDoor* g_door = nullptr;
RoomDecorations* g_decor = nullptr; // <-- NEW: Pointer for decorations

// Game State
bool g_flashlightOn = true;
bool g_showAxes = false;
bool g_showCoordinates = false;

// --- PIN Entry State ---
bool g_isEnteringPin = false;
std::string g_currentPin = "";
int g_interactingDoorIndex = -1;

// HELPER: Map door index to its PIN
std::string getPinForDoor(int index) {
	if (index == 0) return "157";
	if (index == 1) return "1134";
	if (index == 2) return "1927";
	if (index == 3) return "188";
	if (index == 4) return "111";
	return "000";
}

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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA | GLUT_MULTISAMPLE);

	// Create Module objects *after* glutInit
	g_camera = new Camera(win_width, win_height);
	g_labels = new Labels(win_width, win_height);
	g_room = new TheRoom(GRID_SIZE, 5.0f, GRID_SIZE);
	g_insideWalls = new InsideWall(5.0f);
	g_tower = new CornerTower(5.0f, 1.5f);
	g_book = new SecretBook();
	g_door = new SecretDoor();
	g_decor = new RoomDecorations(); // <-- NEW: Initialize Decorations

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
	glutPassiveMotionFunc(mouseMotion);

	// Configure the Camera's starting state
	g_camera->setGroundLevel(2.2f);
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
	delete g_book;
	delete g_door;
	delete g_decor; // <-- NEW: Clean up
	g_camera = nullptr;
	g_labels = nullptr;
	g_room = nullptr;
	g_insideWalls = nullptr;
	g_tower = nullptr;
	g_book = nullptr;
	g_door = nullptr;
	g_decor = nullptr;

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

	// 2. Material Properties
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess = 50.0f; // Shiny metal look
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

	// 3. FLASHLIGHT SETUP
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 0.9f, 1.0f };
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 180.0f);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.0f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.01f);

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

	// --- Load Secret Book Textures ---
	if (g_book) {
		g_book->loadTextures(
			"textures/wood.dds",
			"textures/book_cover.dds",
			"textures/book_pages.dds"
		);
	}

	// --- Load Secret Door Textures ---
	if (g_door) {
		g_door->loadTextures(
			"textures/wall.dds", // Frame
			"textures/wood.dds", // Panels
			"textures/floor.dds" // Details (Metal/Wicker)
		);
	}

	// --- Load Decoration Textures ---
	if (g_decor) {
		g_decor->loadTextures("textures/wood.dds", "textures/wall.dds"); // Using existing textures for now
	}

	// --- Setup Inside Walls (Your Layout) ---
	if (g_insideWalls && g_room) {
		g_insideWalls->addWall(-20.0f, -16.0f, 16.0f, -16.0f, 0.5f);
		g_insideWalls->addWall(-16.0f, 0.0f, 16.0f, 0.0f, 0.5f);
		g_insideWalls->addWall(-16.0f, 16.0f, 20.0f, 16.0f, 0.5f);
		g_insideWalls->addWall(-16.0f, 0.0f, -16.0f, 16.0f, 0.5f);
		g_insideWalls->addWall(0.0f, 0.0f, 0.0f, -12.0f, 0.5f);
		g_insideWalls->build(g_room->getWallTextureID());
	}

	// --- Setup Corner Towers (Your Layout) ---
	if (g_tower && g_room) {
		g_tower->addTower(16.0f, -16.0f);
		g_tower->addTower(16.0f, 0.0f);
		g_tower->addTower(0.0f, -12.0f);
		g_tower->addTower(-16.0f, 16.0f);
		g_tower->addTower(-16.0f, 0.0f);
		g_tower->build(g_room->getWallTextureID());
	}

	// --- Setup Secret Books ---
	if (g_book) {
		g_book->addBook(-14.0f, -17.0f, "Note #1:\n\nThe first number is the loneliest number.\n");
		g_book->addBook(-14.8f, -17.0f, "Note #2:\n\nLook at your hand.\nCount the fingers.");
		g_book->addBook(-15.6f, -17.0f, "Note #3:\n\nDays in a week.\nColors in a rainbow.");
		g_book->addBook(-2.5f, -17.0f, "oh!! Sometimes \nI forget the pin number,\ntherefore I attach three notes with three hints.");
		g_book->addBook(1.0f, -12.0f, "As I remember \nI write a pin number's Hint \non my bedroom diary.I");
		g_book->addBook(6.0f, -15.0f, "There are four inner planets in our solar system: \nMercury, Venus, Earth, and Mars, \noften called terrestrial planets because they are rocky, \ndense, and orbit closest to the Sun, \ninside the asteroid belt. ");
		g_book->addBook(7.0f, -15.0f, "The first man landed on the Moon in 1969, \nduring the NASA Apollo 11 mission, \nwhen astronaut Neil Armstrong stepped onto the lunar \nsurface on July 20, 1969, \nfollowed by Buzz Aldrin, fulfilling President Kennedy's goal. ");
		g_book->addBook(19.0f, -3.0f, "I saw You sleep lot of time,\and therefore I set look,\n the look is the 4 digit\n are what is the __ apollo , How many people in rocket . \nand ,how many inner planets in our solar system.");
		g_book->addBook(6.0f, -2.0f, "The Apollo 11 crew consisted of three astronauts: ");
		g_book->addBook(6.0f, -3.0f, "The first fully electronic television system was demonstrated \nby Philo Taylor Farnsworth in 1927 ");
		g_book->addBook(-1.0f, 4.0f, "The tv room pin is which year the fist tv made");
		g_book->addBook(-14.0f, -2.0f, "The fist tow digit look at the sofa and cout something");
		g_book->addBook(-2.0f, -2.0f, "The next  digit how may pellows in my bed room");
	}

	// --- Setup Secret Door ---
	if (g_door) {
		g_door->addDoor(0.0f, -18.0f, 2, "157");
		g_door->addDoor(18.2f, 0.0f, 1, "1134");
		g_door->addDoor(0.0f, -14.4f, 2, "1927");
		g_door->addDoor(-18.5f, 0.0f, 1, "188");
		g_door->addDoor(-16.0f, 18.25f, 2, "111");
	}

	// --- Setup Room Decorations ---
	if (g_decor) {
		// Add some chairs and tables
		g_decor->addDecoration(1, 11.5f, -6.0f, -90.0f); // Chair near book 1
		g_decor->addDecoration(2, 10.0f, -6.0f, 90.0f);  // Table near book 1
		g_decor->addDecoration(1, 8.5f, -6.0f, 90.0f); // Chair near book 1
		g_decor->addDecoration(1, 10.0f, -7.5f, 0.0f); // Chair near book 1
		g_decor->addDecoration(1, 10.0f, -4.5f, -180.0f); // Chair near book 1
		g_decor->addDecoration(1, 4.0f, -3.0f, -180.0f); // Chair near book 1


		g_decor->addDecoration(2, 2.0f, 2.0f, 135.0f);   // Chair near book 2

		g_decor->addDecoration(4, -11.0f, 4.0f, 90.0f);    
		g_decor->addDecoration(4, -11.0f, 6.00f, 90.0f);
		g_decor->addDecoration(4, -11.0f, 10.0f, 90.0f);
		g_decor->addDecoration(4, -11.0f, 12.0f, 90.0f);

		//crberd
		g_decor->addDecoration(3, 15.0f, -15.0f, -45.0f); // Chair near book 3
		g_decor->addDecoration(3, 1.0f, -7.0f, 90.0f); // Chair near book 3

		//rack
		g_decor->addDecoration(5, 10.0f, -15.0f, 0.0f);   
		g_decor->addDecoration(5, 19.0f, -10.0f, 90.0f);   
		g_decor->addDecoration(5, 19.0f, -6.0f, 90.0f);   

		//lamp
		g_decor->addDecoration(6, 3.0f, -7.0f, 0.0f);   // Table near book 3
		g_decor->addDecoration(6, 14.0f, -9.0f, 0.0f);   // Table near book 3
		g_decor->addDecoration(6, -19.0f, -19.0f, 0.0f);   // Table near book 3
		g_decor->addDecoration(6, -19.0f, -14.0f, 0.0f);   // Table near book 3
		g_decor->addDecoration(6, -14.0f, 1.0f, 0.0f);   // Table near book 3
		g_decor->addDecoration(6, -14.0f, 14.0f, 0.0f);   // Table near book 3

		//plant
		g_decor->addDecoration(10, 2.0f, -2.0f, 90.0f);   // Table near book 3
		g_decor->addDecoration(10, -5.0f, -3.0f, 75.0f);   // Table near book 3
		g_decor->addDecoration(10, -11.0f, -3.0f, 45.0f);   // Table near book 3
		g_decor->addDecoration(10, 15.5f, -17.5f, 45.0f);   // Table near book 3

		  // Table near book 3
		g_decor->addDecoration(9, 4.0f, -5.0f, 0.0f);   // Table near book 3

		//tv unit
		g_decor->addDecoration(8, -8.0f, -3.0f, 180.0f);   // Table near book 3

		//sofa
		g_decor->addDecoration(7, -8.0f, -10.0f, 0.0f);   // Table near book 3


	}

	// --- Collision Grid Setup ---
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
	GLfloat currentAmbient[] = { 0.3f, 0.3f, 0.3f, 1.0f };

	if (g_camera->isDeveloperMode()) {
		// Developer Mode: Full Brightness
		currentAmbient[0] = 0.6f; currentAmbient[1] = 0.6f; currentAmbient[2] = 0.6f;
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	}
	else {
		// Game Mode Logic
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
			// Positioned slightly behind eye for better wall coverage
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
	if (g_showAxes) drawAxes(GRID_HALF_SIZE);
	if (g_showCoordinates) { drawGrid(GRID_SIZE, GRID_SEGMENTS); drawGridCoordinates(GRID_SIZE, GRID_SEGMENTS); }

	if (g_room) g_room->draw();
	if (g_insideWalls) g_insideWalls->draw();
	if (g_tower) g_tower->draw();
	if (g_decor) g_decor->draw(); // <-- NEW: Draw Decorations

	// Draw Secret Books
	if (g_book) {
		g_book->draw();
		if (g_camera && !g_isEnteringPin) {
			int nearIndex = g_book->getNearestBookIndex(g_camera->getX(), g_camera->getZ());
			if (nearIndex != -1) {
				if (g_book->isBookOpen(nearIndex)) {
					g_labels->drawCenterMessage(g_book->getBookMessage(nearIndex));
				}
				else {
					g_labels->drawActionHint("Press 'E' to Read");
				}
			}
		}
	}

	// Draw Secret Doors
	if (g_door) {
		g_door->draw();
		if (g_camera) {
			int doorIdx = g_door->getNearestDoorIndex(g_camera->getX(), g_camera->getZ());
			if (doorIdx != -1 && !g_door->isDoorOpen(doorIdx)) {
				// --- PIN UI LOGIC ---
				if (g_isEnteringPin && g_interactingDoorIndex == doorIdx) {
					// UPDATED MESSAGE: Two lines for better visibility
					const char* pinMsg = "Enter PIN to Unlock.\n(Press 'Esc' or 'E' to Cancel)";
					g_labels->drawCenterMessage(pinMsg);
				}
				else if (!g_isEnteringPin) {
					g_labels->drawActionHint("Press 'E' to Unlock");
				}
			}
		}
	}

	// --- Draw 2D UI (Labels) ---
	if (g_labels && g_camera) {
		g_labels->draw(g_camera->isDeveloperMode(), g_camera->getX(), g_camera->getY(), g_camera->getZ());
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
	int currentTime = glutGet(GLUT_ELAPSED_TIME);
	float dt = (currentTime - g_lastTime) / 1000.0f;
	if (dt < 0.016f) return;
	if (dt > 0.1f) dt = 0.1f;
	g_lastTime = currentTime;

	if (g_camera) g_camera->update(dt);
	if (g_book) g_book->update(dt);
	if (g_door) g_door->update(dt);

	glutPostRedisplay();
}


// ================================================================
// INPUT FUNCTIONS
// ================================================================

void keyboard(unsigned char key, int x, int y) {
	if (!g_isEnteringPin) {
		g_camera->updateModifiers(glutGetModifiers());
	}

	// --- HANDLE PIN ENTRY MODE ---
	if (g_isEnteringPin) {
		// CANCEL logic: 'Esc' OR 'E'
		if (key == 27 || key == 'e' || key == 'E') {
			g_isEnteringPin = false;
			g_currentPin = "";
			g_interactingDoorIndex = -1;
			printf("PIN Entry Cancelled.\n");
		}
		else if (key >= '0' && key <= '9') { // Number keys
			// LOGIC: Check if this next digit is correct
			std::string nextPin = g_currentPin + (char)key;

			// DYNAMICALLY get the correct PIN for this door
			std::string targetPin = getPinForDoor(g_interactingDoorIndex);

			// Check validity
			bool isValidSoFar = true;
			if (nextPin.length() <= targetPin.length()) {
				for (size_t i = 0; i < nextPin.length(); i++) {
					if (nextPin[i] != targetPin[i]) {
						isValidSoFar = false;
						break;
					}
				}
			}
			else {
				isValidSoFar = false;
			}

			if (isValidSoFar) {
				g_currentPin = nextPin;
				if (g_currentPin == targetPin) {
					if (g_door && g_door->tryUnlock(g_interactingDoorIndex, g_currentPin.c_str())) {
						printf("Door Unlocked!\n");
						g_isEnteringPin = false;
						g_currentPin = "";
						g_interactingDoorIndex = -1;
					}
				}
			}
			else {
				printf("Wrong digit! Resetting.\n");
				g_currentPin = "";
			}
		}
		return;
	}

	// --- NORMAL GAME KEYS ---

	if (key == 27) { // ESC Key
		printf("ESC key pressed. Exiting.\n");
		delete g_camera; delete g_labels; delete g_room;
		delete g_insideWalls; delete g_tower; delete g_book; delete g_door;
		delete g_decor; // <-- NEW: Clean up
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

	// --- INTERACTION KEY ('E') ---
	if (key == 'e' || key == 'E') {
		// 1. Check Door Interaction FIRST
		if (g_door && g_camera) {
			int doorIndex = g_door->getNearestDoorIndex(g_camera->getX(), g_camera->getZ());
			if (doorIndex != -1 && !g_door->isDoorOpen(doorIndex)) {
				// Start PIN Entry
				g_isEnteringPin = true;
				g_currentPin = "";
				g_interactingDoorIndex = doorIndex;
				return; // Stop here
			}
		}

		// 2. Check Book Interaction
		if (g_book && g_camera) {
			int bookIndex = g_book->getNearestBookIndex(g_camera->getX(), g_camera->getZ());
			if (bookIndex != -1) {
				g_book->toggleBook(bookIndex);
			}
		}
	}

	// Developer Toggles
	if (key == 't' || key == 'T') {
		if (g_camera->isDeveloperMode()) g_showAxes = !g_showAxes;
	}
	if (key == 'c' || key == 'C') {
		if (g_camera->isDeveloperMode()) g_showCoordinates = !g_showCoordinates;
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