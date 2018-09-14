#pragma once
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Compile, load and handle Shaders
 */
#include <string.h> 
#include <iostream>
#include <GL_ADD\glew.h>
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

// Shader container
struct cgShader
{
	// Name of the program
	string filename;
	// CGprogram
	CGprogram cgProgram;
	// CGparams
	vector<CGparameter> cgParam;
	// Name of the params
	vector<char*> cgParamName;
};

class Texturing
{
public:
	// Constructor
	Texturing(void);
	// Destructor
	~Texturing(void);
	// Initialize C for Graphics
	void initCG(void);
	// Enable Vertex and Fragment Profile
	void enableShaderProfiles(void);
	// Disable Vertex and Fragment Profile
	void disableShaderProfiles(void);
	// Load a Vertex Shader
	void loadVertexShader(string filename, char *function);
	// Load a Fragment Shader
	void loadFragmentShader(string filename, char *function);
	// Bind a Vertex Shader
	void bindVertexShader(string filename);
	// Bind a Fragment Shader
	void bindFragmentShader(string filename);
	// Set and update Methode for Float4
	void setShaderParam4f(string filename, char *paramName, float param[4]);
	void updateShaderParam4f(string filename, char *paramName, float param[4]);
	// Set and update Methode for Float3
	void setShaderParam3f(string filename, char *paramName, float param[3]);
	void updateShaderParam3f(string filename, char *paramName, float param[3]);
	// Set and update Methode for Float1
	void setShaderParam1f(string filename, char *paramName, float param);
	void updateShaderParam1f(string filename, char *paramName, float param);
	// Set and update Methode for Texture
	void setShaderTexture(char* filename, char *paramName);
	void enableShaderTexture(char* filename, char *paramName, char* texture);
	void disableShaderTexture(char* filename, char *paramName);
	// Set and update Methode for Texture
	void setShaderCubeTexture(char* filename,char *paramName);
	void enableShaderCubeTexture(char* filename, char *paramName);
	void disableShaderCubeTexture(char* filename, char *paramName);
	// Set and update Methode for Matrix
	void setShaderStateMatrix(string filename, char *paramName);
	void updateShaderStateMatrix(string filename, char *paramName, CGGLenum matrix, CGGLenum transform);
	// Set the static material color with RGB
	void setStaticMaterialColor(int side, double r, double g, double b);
private:
	// CG Context
	//CGcontext cgContext;
	// Vertex profile
	CGprofile vertexProfile;
	// Fragment profile
	CGprofile fragmentProfile;
	// All loaded vertex shaders
	vector<cgShader*> vertexShaders;
	// All loaded fragment shaders
	vector<cgShader*> fragmentShaders;
	// Error handler for CG
	static void cgErrorCallback(void);
	// Returns true if a shader is loaded (identify - true=vertex/false=fragment)
	bool isShaderLoaded(string filename, bool identify);
	// Returns a shader by name
	cgShader* getShader(string filename);
	// Returns true if a param is set to a shader
	bool isParamSet(cgShader* shader, char *param);
	// Returns a param of a shader by name
	CGparameter getShaderParam(cgShader* shader, char *param);
	// All texture IDs
	unsigned int texture[128];
	// All texture names
	vector<char*> texture_names;
	// Return a textureID by name
	unsigned int Texturing::getTextureByName(char *filename);
	// Load a texture
	void loadTexture(char *filename, unsigned int slot);
	// Returns true if a texture is loaded
	bool isLoaded(char *filename);
};

