// GraphicsUtils.cpp : Defines the functions for the static library.
//
#include "pch.h"

/**
 * @brief Draws X, Y, and Z axes.
 */
void drawAxes(float length) {
    // We don't need to include <glut.h> because
    // it's already in "pch.h"

    glDisable(GL_LIGHTING);

    glBegin(GL_LINES);

    // X-axis in Red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(-length, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);

    // Y-axis in Green
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(0.0f, -length, 0.0f);
    glVertex3f(0.0f, length, 0.0f);

    // Z-axis in Blue
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, -length);
    glVertex3f(0.0f, 0.0f, length);

    glEnd();

    glEnable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);

	


}

void display()
{
    // Clear the screen (color and depth buffers)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up the camera (ModelView matrix)
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Place our camera at (3, 2, 5) looking at the origin (0, 0, 0)
    gluLookAt(
        3.0, 2.0, 5.0,  // Eye position (where the camera IS)
        0.0, 0.0, 0.0,  // Center position (where the camera LOOKS)
        0.0, 1.0, 0.0   // Up vector (which way is "up")
    );

    // --- Draw your scene here ---

    // Call your function from the GraphicsUtils module!
    drawAxes(10.0f);

    // Later, you will add:
    // drawRoom();
    // drawTable();

    // --- End of scene ---

    // Swap the front and back buffers to display the new frame
    glutSwapBuffers();
}



void reshape(int w, int h)
{
   
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
        0.1,                   // Near clipping plane (can't see objects closer than 0.1)
        100.0                  // Far clipping plane (can't see objects farther than 100)
    );
}

void init()
{
    // Set a dark blue-grey background color
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Enable the depth test (so objects draw in the correct order)
    glEnable(GL_DEPTH_TEST);

    // Enable basic lighting
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Turn on one light source

    // Set all objects to have a white color by default
    glColor3f(1.0f, 1.0f, 1.0f);

}

void keyboard(unsigned char key, int x, int y)
{
    // 27 is the ASCII code for the ESCAPE key
    if (key == 27) {
        printf("ESC key pressed. Exiting.\n");
        exit(0); // Quit the program
    }

    // You will add 'w', 'a', 's', 'd' controls here
}

void idle()
{
    // This is where you will update your animations
    // and smooth camera movement.

    // For now, just tell OpenGL that the screen needs to be redrawn
    glutPostRedisplay();
}
