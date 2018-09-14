#include "Producing.h"
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Produce primtives
 */

static double  M_PI = 3.1416;

// Constructor
Producing::Producing(void)
{

}

// Destructor
Producing::~Producing(void)
{
}

// Draw a Triangle
void Producing::drawTriangle()
{
	 
}

// Draw a plane composed of quads
void Producing::drawPlane()
{
	glBegin(GL_QUADS);
	glNormal3d(0,0,1);
	glTexCoord2d(0.0, 0.0); glVertex3d(-1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 0.0); glVertex3d( 1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 1.0); glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-1.0,  1.0,  1.0);
	glEnd();
}

// Draw a cube composed of quads
void Producing::drawCube()
{
	glBegin(GL_QUADS);
	// Front face
	glNormal3d(0,0,1);
	glTexCoord2d(0.0, 0.0); glVertex3d(-1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 0.0); glVertex3d( 1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 1.0); glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-1.0,  1.0,  1.0);
	// Back face
	glNormal3d(0,0,-1);
	glTexCoord2d(1.0, 0.0); glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 1.0); glVertex3d( 1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 0.0); glVertex3d( 1.0, -1.0, -1.0);
	// Top face
	glNormal3d(0,1,0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 0.0); glVertex3d(-1.0,  1.0,  1.0);
	glTexCoord2d(1.0, 0.0); glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(1.0, 1.0); glVertex3d( 1.0,  1.0, -1.0);
	// Bottom face
	glNormal3d(0,-1,0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(0.0, 1.0); glVertex3d( 1.0, -1.0, -1.0);
	glTexCoord2d(0.0, 0.0); glVertex3d( 1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 0.0); glVertex3d(-1.0, -1.0,  1.0);
	// Right face
	glNormal3d(1,0,0);
	glTexCoord2d(1.0, 0.0); glVertex3d( 1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 1.0); glVertex3d( 1.0,  1.0, -1.0);
	glTexCoord2d(0.0, 1.0); glVertex3d( 1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 0.0); glVertex3d( 1.0, -1.0,  1.0);
	// Ledt face
	glNormal3d(-1,0,0);
	glTexCoord2d(0.0, 0.0); glVertex3d(-1.0, -1.0, -1.0);
	glTexCoord2d(1.0, 0.0); glVertex3d(-1.0, -1.0,  1.0);
	glTexCoord2d(1.0, 1.0); glVertex3d(-1.0,  1.0,  1.0);
	glTexCoord2d(0.0, 1.0); glVertex3d(-1.0,  1.0, -1.0);
	glEnd();
}

// Draw a sphere composed of triangles
void Producing::drawSphere(double radius)
{
	int     i, j, n1 = 25, n2 = 25;
    Vec3    normal, v1;
    double  a1, a1d, a2, a2d, s1, s2, c1, c2;
    a1d = M_PI / (double) n1;
    a2d = M_PI / (double) n2;

    for(i = 0; i < n1; i++)
	{
		a1 = i * M_PI / (double) n1;
		glBegin(GL_TRIANGLE_STRIP);

		for(j=0; j<=n2; j++)
		{
			a2 = (j + .5 * (i%2)) * 2.0 * M_PI / (double) n2;
            s1 = sin( a1);
            c1 = cos( a1);
            s2 = sin( a2);
            c2 = cos( a2);

            normal = c1 * XVec3 + s1 * (c2 * YVec3 + s2 * ZVec3);

            v1 = Vec3(0,0,0) + radius * normal;

            glNormal3dv( normal.p);
			glTexCoord2d(v1.p[0],v1.p[1]);
            glVertex3dv( v1.p);

			s1 = sin( a1 + a1d);
            c1 = cos( a1 + a1d);
            s2 = sin( a2 + a2d);
            c2 = cos( a2 + a2d);

            normal = c1 * XVec3 + s1 * (c2 * YVec3 + s2 * ZVec3);

            v1 = Vec3(0,0,0) + radius * normal;

            glNormal3dv( normal.p);
			glTexCoord2d(v1.p[0],v1.p[1]);
            glVertex3dv( v1.p);
		}

		glEnd();
	}
}

// Draw a cylinder composed of polygons and triangles
void Producing::drawCylinder(int finessLength, int finessCircle, double height)
{
	// Top face
	int i;
	GLfloat px, py;
	glBegin(GL_POLYGON);
	glNormal3d( 0,0,1);
	for(i = 0; i < 100; i++)
	{
		px = cos(i*2*M_PI/100)/2.5;
		py = sin(i*2*M_PI/100)/2.5;
		glTexCoord2d(px, py);
		glVertex3d(px, py, height/2);
	}
	glEnd();

	// Bottom face
	glBegin(GL_POLYGON);
	glNormal3d(0,0,-1);
	for( i = 99; i > 0; i--)
	{
		px = cos(i*2*M_PI/100)/2.5;
		py = sin(i*2*M_PI/100)/2.5;
		glTexCoord2d(px, py);
		glVertex3d(px, py, height/2*-1);
	}
	glEnd();

	// Side face
	double majorStep = height / finessLength;
	double minorStep = 2.0 * M_PI / finessCircle;
	double radius = 0.4;

	int j;
	for (i = 0; i < finessLength; ++i)
	{
		GLfloat z0 = 0.5 * height - i * majorStep;
		GLfloat z1 = z0 - majorStep;
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0; j <= finessCircle; ++j)
		{
			double a = j * minorStep;
			GLfloat x = radius * cos(a);
			GLfloat y = radius * sin(a);

			glNormal3f(x / radius, y / radius, 0.0);
			glTexCoord2f(j / (GLfloat) finessCircle, i / (GLfloat) finessLength);
			glVertex3f(x, y, z0);

			glNormal3f(x / radius, y / radius, 0.0);
			glTexCoord2f(j / (GLfloat) finessCircle, (i + 1) / (GLfloat) finessLength);
			glVertex3f(x, y, z1);
		}
		glEnd();
	}
}

// Draw a pyramid composed of triangles
void Producing::drawPyramid(double a, double h)
{

}