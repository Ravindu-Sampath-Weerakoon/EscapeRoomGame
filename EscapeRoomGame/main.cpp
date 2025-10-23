//this is  a simple escape room game simulation in C++ with openGl graphics

#include <iostream>
#include "GraphicsUtils.h"
#include <glut.h>

int win_width = 1024;
int win_height = 720;


void welcomeConsoleMessage() {
	std::cout << "Welcome to the Escape Room Game!" << std::endl;
}

int main(int argc, char** argv) {
	welcomeConsoleMessage();
    printf("Starting Escape Room Game...\n");

    // 1. Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGBA);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(win_width, win_height);
    glutCreateWindow("Escape Room Game");

    // 2. Register Callback Functions
    // These functions tell GLUT what to do in different situations

    glutDisplayFunc(display);     // Called when the screen needs to be redrawn
    glutReshapeFunc(reshape);     // Called when the window is resized
    glutKeyboardFunc(keyboard);   // Called when a normal key (like 'w' or ESC) is pressed
    //glutSpecialFunc(specialKeys); // You will add this later for arrow keys
    glutIdleFunc(idle);         // Called when the game is not busy (for animation)

    // 3. Call your one-time setup function
    init();

    // 4. Start the Main Game Loop
    // This function never returns. It continuously processes events.
    glutMainLoop();

    return 0; // Will never be reached

}