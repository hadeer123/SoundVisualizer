#include "Producing.h"
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Produce primtives
 */
static double  M_PI = 3.1416;

// Constructor
Producing::Producing(void)
{
	this->doRotation = FALSE;
}

// Destructor
Producing::~Producing(void)
{
}

// Draw a Triangle
void Producing::drawTriangle(Vec3 pos, double a, double b)
{
	 
}

// Draw a plane composed of triangles
void Producing::drawPlane(Vec3 pos, double a, double b, unsigned int vertical)
{
	glLoadIdentity();

    glTranslatef(pos.p[0], pos.p[1], pos.p[2]);

	this->makeRotation();

	if(vertical == 1)
		glScaled(a,b,1);
	else if(vertical == 0)
		glScaled(a,1,b);
	else if(vertical == 2 || vertical == 3 )
		glScaled(1,b,a);

	glBegin(GL_QUADS);
	if(vertical == 1)
	{
		glTexCoord2d(0.0, 0.0); glVertex3d(-1.0, -1.0,  1.0);
		glTexCoord2d(1.0, 0.0); glVertex3d( 1.0, -1.0,  1.0);
		glTexCoord2d(1.0, 1.0); glVertex3d( 1.0,  1.0,  1.0);
		glTexCoord2d(0.0, 1.0); glVertex3d(-1.0,  1.0,  1.0);
	}
	else if(vertical == 0)
	{
		glTexCoord2d(1.0, 1.0); glVertex3d(-1.0, -1.0, -1.0);
		glTexCoord2d(0.0, 1.0); glVertex3d( 1.0, -1.0, -1.0);
		glTexCoord2d(0.0, 0.0); glVertex3d( 1.0, -1.0,  1.0);
		glTexCoord2d(1.0, 0.0); glVertex3d(-1.0, -1.0,  1.0);
	}
	else if(vertical == 2)
	{
		glTexCoord2d(1.0, 0.0); glVertex3d( 1.0, -1.0, -1.0);
		glTexCoord2d(1.0, 1.0); glVertex3d( 1.0,  1.0, -1.0);
		glTexCoord2d(0.0, 1.0); glVertex3d( 1.0,  1.0,  1.0);
		glTexCoord2d(0.0, 0.0); glVertex3d( 1.0, -1.0,  1.0);
	}
	else if(vertical == 3)
	{
		glTexCoord2d(0.0, 0.0); glVertex3d(-1.0, -1.0, -1.0);
		glTexCoord2d(1.0, 0.0); glVertex3d(-1.0, -1.0,  1.0);
		glTexCoord2d(1.0, 1.0); glVertex3d(-1.0,  1.0,  1.0);
		glTexCoord2d(0.0, 1.0); glVertex3d(-1.0,  1.0, -1.0);
	}
	glEnd();
}

// Draw a cuboid composed of triangles
void Producing::drawCuboid(Vec3 pos, double a, double b, double c)
{
	glLoadIdentity();

    glTranslatef(pos.p[0], pos.p[1], pos.p[2]);
	this->makeRotation();
	glScaled(a,b,c);

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

// Draw a cube composed of triangles
void Producing::drawCube(Vec3 pos, double a)
{
	glLoadIdentity();

    glTranslatef(pos.p[0], pos.p[1], pos.p[2]);
	this->makeRotation();
	glScaled(a,a,a);

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

// Draw a angle cube composed of triangles
void Producing::drawAngleCube(Vec3 pos, double a, double b, double c, Vec3 angleDir, double angle)
{
	glLoadIdentity();

    glTranslatef(pos.p[0], pos.p[1], pos.p[2]);
	this->makeRotation();
	glScaled(a,b,c);
	glRotated(angle, angleDir.p[0], angleDir.p[1], angleDir.p[2]);

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
void Producing::drawSphere(Vec3 pos, double r)
{
	glLoadIdentity();

	int     i, j, n1 = 6, n2 = 12;
    Vec3    normal, v1;
    double  a1, a1d, a2, a2d, s1, s2, c1, c2;
    a1d = M_PI / (double) n1;
    a2d = M_PI / (double) n2;
    for(i=0; i<n1; i++){
		a1 = i * M_PI / (double) n1;
		glBegin( GL_TRIANGLE_STRIP);
		for(j=0; j<=n2; j++){
			a2 = (j + .5 * (i%2)) * 2.0 * M_PI / (double) n2;
            s1 = sin( a1);
            c1 = cos( a1);
            s2 = sin( a2);
            c2 = cos( a2);
            normal = c1 * XVec3 + s1 * (c2 * YVec3 + s2 * ZVec3);
            v1 = pos + r * normal;
            glNormal3dv( normal.p);
			glTexCoord2d(v1.p[0],v1.p[1]);
            glVertex3dv( v1.p);
			s1 = sin( a1 + a1d);
            c1 = cos( a1 + a1d);
            s2 = sin( a2 + a2d);
            c2 = cos( a2 + a2d);
            normal = c1 * XVec3 + s1 * (c2 * YVec3 + s2 * ZVec3);
            v1 = pos + r * normal;
            glNormal3dv( normal.p);
			glTexCoord2d(v1.p[0],v1.p[1]);
            glVertex3dv( v1.p);
		}
		glEnd();
	}
}


// Draw a cylinder composed of triangles
void Producing::drawCylinder(Vec3 pos, double scale, int finessLength, int finessCircle, double height)
{
	glPushMatrix();
	glTranslatef(pos.p[0], pos.p[1], pos.p[2]);
	glScaled(scale, scale, scale);
	glRotated(90, 1, 0, 0);

	this->makeRotation();

	// Top face
	int i;
	GLfloat px, py;
	glBegin(GL_POLYGON);
	glNormal3d( 0,0,1);
	for(i=0;i<100;i++) {
		px = cos(i*2*M_PI/100)/2.5;
		py = sin(i*2*M_PI/100)/2.5;
		glTexCoord2d(px, py);
		glVertex3d(px, py, height/2);
	}
	glEnd();

	// Bottom face
	glBegin(GL_POLYGON);
	glNormal3d( 0,0,-1);
	for(i=99;i>0;i--) {
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
	for (i = 0; i < finessLength; ++i) {
		GLfloat z0 = 0.5 * height - i * majorStep;
		GLfloat z1 = z0 - majorStep;
		glBegin(GL_TRIANGLE_STRIP);
		for (j = 0; j <= finessCircle; ++j) {
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
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

// Draw a pyramid composed of triangles
void Producing::drawPyramid(Vec3 pos, double a, double h)
{

}

// Make to do rotation on the following object
void Producing::rotate(Vec3 rotationPoint, double rotationAngle)
{
	this->rotationPoint.push_back(rotationPoint);
	this->rotationAngle.push_back(rotationAngle);
	doRotation = TRUE;
}

// Rotate the following object if it was called
void Producing::makeRotation()
{
	if(this->doRotation)
	{
		for(unsigned int i = 0; i < this->rotationPoint.size(); i++)
		{
			glRotated(this->rotationAngle[i], this->rotationPoint[i].p[0], this->rotationPoint[i].p[1], this->rotationPoint[i].p[2]);
		}
		doRotation = FALSE;
		this->rotationPoint.clear();
		this->rotationAngle.clear();
	}
}