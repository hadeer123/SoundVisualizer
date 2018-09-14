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

#define TRUE 1
#define FALSE 0

using namespace std;

class Producing
{
public:
	// Constructor
	Producing(void);
	// Destructor
	~Producing(void);
	// Draw a Triangle
	void drawTriangle(Vec3 pos, double a, double b);
	// Draw a plane composed of triangles
	void drawPlane(Vec3 pos, double a, double b, unsigned int vertical);
	// Draw a cuboid composed of triangles
	void drawCuboid(Vec3 pos, double a, double b, double c);
	// Draw a cube composed of triangles
	void drawCube(Vec3 pos, double a);
	// Draw a angle cube composed of triangles
	void drawAngleCube(Vec3 pos, double a, double b, double c, Vec3 angleDir, double angle);
	// Draw a sphere composed of triangles
	void drawSphere(Vec3 pos, double r);
	// Draw a circle
	void drawCircle(Vec3 pos);
	// Draw a cylinder composed of triangles
	void drawCylinder(Vec3 pos, double scale, int finessLength, int finessCirle, double height);
	// Draw a pyramid composed of triangles
	void drawPyramid(Vec3 pos, double a, double h);
	// Make to do rotation on the following object
	void rotate(Vec3 rotationPoint, double rotationAngle);
private:
	// Check for doing rotation
	int doRotation;
	// Vector with the rotation angles
	vector<double> rotationAngle;
	// Vector with the Rotation points
	vector<Vec3> rotationPoint;
	// Rotate the following object
	void makeRotation();
};

