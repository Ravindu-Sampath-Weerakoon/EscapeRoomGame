//this is  a simple escape room game simulation in C++ with openGl graphics

#include <iostream>
#include "GraphicsUtils.h"


void welcomeConsoleMessage() {
	std::cout << "Welcome to the Escape Room Game!" << std::endl;
}

int main() {
	welcomeConsoleMessage();
	drawAxes(45.0f);
	

	return 0;
}