#pragma once
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Initializes the graphic-engine
 */
#include "Texturing.h"
#include <iostream>
#include <vector>
#include <GL_ADD\glfw.h>
#include <GL_ADD\glut.h>
#include <GL\gl.h>
#include <math.h>
#include <vec3.h>
#include <cstdlib>
#include "Audio.h"
#include "Producing.h"

#include "MathHelper.h"

#define FALSE 0
#define TRUE 1

#define SPECLEN 512

using namespace std;

class Rendering
{
public:
	// Attributes
	Audio *audio;
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
private:
	// audio attributes
	float *bassL;
	float *bassR;
	float br1; float br2; float br3; float br4;
	float bl1; float bl2; float bl3; float bl4;
	// load and prepare all shaders 
	void loadShaders(void);
	// Calculate scene action
	void calculateAction();
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
	// Update animation
	void updateAnimation(void);
	// Update sound
	void updateSound(void);
};

