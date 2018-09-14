#pragma once
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Initializes the graphic-engine
 */

#include <iostream>
#include <vector>
#include <GL_ADD\glfw.h>
#include <GL_ADD\glut.h>
#include <GL\gl.h>
#include <math.h>
#include <vec3.h>
#include "Producing.h"
#include "Texturing.h"

#define TRUE 1
#define FALSE 0

#define ROOM 2
#define BAR 3

using namespace std;

class Rendering
{
public:
	// Attributes
	Producing* producing;
	Texturing *texturing;
	// Constructor
	Rendering(void);
	// Destructor
	~Rendering(void);
	// Initializes the rendering
	void initialize(void);
	// Handle key input
	static void GLFWCALL handleKeyboardInput(int key, int action);
	// Handle mouse input
	static void GLFWCALL handleMouseInput(int button, int action);
	// Handle mouse wheel
	static void GLFWCALL handleMouseWheel(int pos);
	// Handle mouse position
	static void GLFWCALL handleMousePosition(int x, int y);
	// Calculate scene action
	void calculateAction();
private:
	// Calculate the FPS
	static void calculateFPS(void);
	// Print a string to the given coordinates
	void drawFPS(float x, float y, float z, char* format, ...);
	// Starts the main loop
	void startMainLoop(void);
	// Set lighting and camera
	void updateLightingAndProjection(void);
	// Draw the scene
	void updateScene(void);
	// Build DisplayLists
	void buildDisplayLists(void);
};

