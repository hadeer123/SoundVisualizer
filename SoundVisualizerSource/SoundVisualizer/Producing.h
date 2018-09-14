#pragma once
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Produce primtives
 */

#include <iostream>
#include <vector>
#include <GL_ADD\glfw.h>
#include <GL_ADD\glut.h>
#include <GL\gl.h>
#include <math.h>
#include "vec3.h"

using namespace std;

class Producing
{
public:
	// Constructor
	Producing(void);
	// Destructor
	~Producing(void);
	// Draw a Triangle
	void drawTriangle();
	// Draw a plane composed of quads
	void drawPlane();
	// Draw a cube composed of quads
	void drawCube();
	// Draw a sphere composed of triangles
	void drawSphere(double radius);
	// Draw a cylinder composed of polygons and triangles
	void drawCylinder(int finessLength, int finessCirle, double height);
	// Draw a pyramid composed of triangles
	void drawPyramid(double a, double h);
private:
};

