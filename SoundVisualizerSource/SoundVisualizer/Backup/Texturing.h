#pragma once
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Creating materials and textures
 */
#include <string.h> 
#include <iostream>
#include <GL_ADD\glfw.h>
#include <math.h>
#include "vec3.h"
#include <vector>
#include <string>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <cg/cg.h>
#include <cg/cgGL.h>


using namespace std;

struct cgShader
{
	string filename;
	CGprogram cgProgram;
	vector<CGparameter> cgParam;
	vector<char*> cgParamName;
};

class Texturing
{
public:
	// Constructor
	Texturing(void);
	// Destructor
	~Texturing(void);
	// Set the static material color with RGB
	void setStaticMaterialColor(int side, double r, double g, double b);

	// Set Shader
	void setVertexShader(string filename, char *function);
	void setFragmentShader(string filename, char *function);

	void setShaderParam4f(string filename, char *paramName, float param[4], bool update);
	void setShaderParam3f(string filename, char *paramName, float param[3], bool update);
	void setShaderParam1f(string filename, char *paramName, float param, bool update);
	void setShaderTexture(char* filename, char *paramName, char* texture, bool update);
	void setShaderStateMatrix(string filename, char *paramName, CGGLenum matrix, CGGLenum transform);

	void initCG(void);
	void enableShaders(void);
	void disableShaders(void);

	void clearVertex(string filename);
	void clearFragment(string filename);

	void setTexture(char *Filename, unsigned int mode);
private:
	// SHADING
	CGcontext cgContext;

	CGprofile vertexProfile;
	CGprofile fragmentProfile;

	vector<cgShader> vertexShaders;
	vector<cgShader> fragmentShaders;

	static void cgErrorCallback(void);

	bool isShaderLoaded(string filename, bool identify);
	cgShader getShader(string filename);

	bool isParamSet(cgShader shader, char *param);
	CGparameter getShaderParam(cgShader shader, char *param);

	// TEXTURING
	unsigned int texture[16];
	vector<char*> texture_names;
	unsigned int Texturing::getTextureByName(char *filename);
	// Load a texture
	void loadTexture(char *filename, unsigned int slot);
	// Is texture loaded
	bool isLoaded(char *filename);
};

