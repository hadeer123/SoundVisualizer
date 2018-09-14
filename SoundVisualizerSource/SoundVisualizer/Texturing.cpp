#include "Texturing.h"
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Compile, load and handle Shaders
 */

static CGcontext cgContext;
static unsigned int cubeMapId = 1110;
// Constructor
Texturing::Texturing(void)
{
	ilInit();
	iluInit();
}

// Destructor
Texturing::~Texturing(void)
{
	//...
}
static GLenum faceTarget[6] = {
  GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,
  GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,
  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
};

/* Pre-generated cube map images. */
char *faceFile[6] = {
  "../Assets/sides.bmp", 
  "../Assets/sides.bmp", 
  "../Assets/top.bmp", 
  "../Assets/top.bmp", 
  "../Assets/sides.bmp", 
  "../Assets/sides.bmp", 
};
void loadFace(GLenum target, char *filename)
{

  	unsigned int img;
	ilGenImages(1, &img);
	ilLoadImage(filename);
	ILinfo info;
	iluGetImageInfo(&info);


    //glTexImage2D(target, 0, GL_RGBA, info.Width, info.Height, 0, info.Format, info.Type, info.Data);
	gluBuild2DMipmaps(target, GL_RGBA,
      info.Width, info.Height,
      info.Format, info.Type, info.Data);
}

void makeCubeMap(void)
{
  int i;

  for (i=0; i<6; i++) {
    loadFace(faceTarget[i], faceFile[i]);
  }

  glEnable(GL_TEXTURE_CUBE_MAP_ARB);

  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  

  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   glEnable(GL_TEXTURE_CUBE_MAP_ARB);
}
// Initialize C for Graphics
void Texturing::initCG(void)
{
	cgContext = cgCreateContext();

	vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	cgGLSetOptimalOptions(vertexProfile);

	fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(fragmentProfile);

	cgSetErrorCallback(cgErrorCallback);
}

// Enable Vertex and Fragment Profile
void Texturing::enableShaderProfiles(void)
{
	cgGLEnableProfile( vertexProfile );
	cgGLEnableProfile( fragmentProfile );
}

// Disable Vertex and Fragment Profile
void Texturing::disableShaderProfiles(void)
{
	cgGLDisableProfile( vertexProfile );
	cgGLDisableProfile( fragmentProfile );
}

// Load a Vertex Shader
void Texturing::loadVertexShader(string filename, char *function)
{
	cgShader* vertexShader = new cgShader();
	vertexShader->filename = filename;
	filename = "../Shaders/Vertex/" + filename + ".cg";
	vertexShader->cgProgram = cgCreateProgramFromFile(cgContext, CG_SOURCE, filename.c_str(), vertexProfile, function, 0);
	vertexShaders.push_back(vertexShader);

	if(!vertexShader->cgProgram)
	{
		printf("A Vertex shader couldnt load\n");
	}

	cgGLLoadProgram(vertexShader->cgProgram);
}

// Bind a Vertex Shader
void Texturing::bindVertexShader(string filename)
{
	if(isShaderLoaded(filename, true)) cgGLBindProgram( getShader(filename)->cgProgram );
}

// Load a Fragment Shader
void Texturing::loadFragmentShader(string filename, char *function)
{
	cgShader* fragmentShader = new cgShader();
	fragmentShader->filename = filename;
	filename = "../Shaders/Fragment/" + filename + ".cg";
	fragmentShader->cgProgram = cgCreateProgramFromFile(cgContext, CG_SOURCE, filename.c_str(), fragmentProfile, function, 0);
	fragmentShaders.push_back(fragmentShader);

	if(!fragmentShader->cgProgram)
	{
		printf("A Fragment shader couldnt load\n");
	}

	cgGLLoadProgram(fragmentShader->cgProgram);
}

// Bind a Fragment Shader
void Texturing::bindFragmentShader(string filename)
{
	if(isShaderLoaded(filename, false)) cgGLBindProgram( getShader(filename)->cgProgram );
}

//--------------------------------------------------------------------------------
// Set and update Methode for Float4
//--------------------------------------------------------------------------------
void Texturing::setShaderParam4f(string filename, char *paramName, float param[4])
{
	cgShader* shader = getShader(filename);

	shader->cgParam.push_back(cgGetNamedParameter(shader->cgProgram, paramName));
	shader->cgParamName.push_back(paramName);

	if (!shader->cgParam[shader->cgParam.size()-1])
	{
		printf("Unable to retrieve program parameters, Param4f...\n");
	}

	cgGLSetParameter4f(getShaderParam(shader, paramName), param[0], param[1], param[2], param[3]);
}

void Texturing::updateShaderParam4f(string filename, char *paramName, float param[4])
{
	cgShader* shader = getShader(filename);
	if(isParamSet(shader, paramName)) cgGLSetParameter4f(getShaderParam(shader, paramName), param[0], param[1], param[2], param[3]);
}

//--------------------------------------------------------------------------------
// Set and update Methode for Float3
//--------------------------------------------------------------------------------
void Texturing::setShaderParam3f(string filename, char *paramName, float param[3])
{
	cgShader* shader = getShader(filename);

	shader->cgParam.push_back(cgGetNamedParameter(shader->cgProgram, paramName));
	shader->cgParamName.push_back(paramName);

	if (!shader->cgParam[shader->cgParam.size()-1])
	{
		printf("Unable to retrieve program parameters, Param3f...\n");
	}

	cgGLSetParameter3f(getShaderParam(shader, paramName), param[0], param[1], param[2]);
}

void Texturing::updateShaderParam3f(string filename, char *paramName, float param[3])
{
	cgShader* shader = getShader(filename);
	if(isParamSet(shader, paramName)) cgGLSetParameter3f(getShaderParam(shader, paramName), param[0], param[1], param[2]);
}

//--------------------------------------------------------------------------------
// Set and update Methode for Float1
//--------------------------------------------------------------------------------
void Texturing::setShaderParam1f(string filename, char *paramName, float param)
{
	cgShader* shader = getShader(filename);

	shader->cgParam.push_back(cgGetNamedParameter(shader->cgProgram, paramName));
	shader->cgParamName.push_back(paramName);

	if (!shader->cgParam[shader->cgParam.size()-1])
	{
		printf("Unable to retrieve program parameters, Param1f...\n");
	}

	cgGLSetParameter1f(getShaderParam(shader, paramName), param);
}

void Texturing::updateShaderParam1f(string filename, char *paramName, float param)
{
	cgShader* shader = getShader(filename);
	if(isParamSet(shader, paramName)) cgGLSetParameter1f(getShaderParam(shader, paramName), param);
}

//--------------------------------------------------------------------------------
// Set, enable and disable Methode for Texture
//--------------------------------------------------------------------------------
void Texturing::setShaderTexture(char* filename, char *paramName)
{
	cgShader* shader = getShader(filename);

	shader->cgParam.push_back(cgGetNamedParameter(shader->cgProgram, paramName));
	shader->cgParamName.push_back(paramName);

	if (!shader->cgParam[shader->cgParam.size()-1])
	{
		printf("Unable to retrieve program parameters, Texture...\n");
	}
}

void Texturing::enableShaderTexture(char* filename, char *paramName, char* texture)
{
	cgShader* shader = getShader(filename);
	if(!this->isLoaded(texture))
	{
		this->loadTexture(texture, this->texture_names.size());
	}
	unsigned int textureID = getTextureByName(texture);
	if(isParamSet(shader, paramName))
	{
		cgGLSetTextureParameter(getShaderParam(shader, paramName), textureID);
		cgGLEnableTextureParameter(getShaderParam(shader, paramName));
		
	}
}

void Texturing::disableShaderTexture(char* filename, char *paramName)
{
	cgShader* shader = getShader(filename);
	if(isParamSet(shader, paramName))
	{
		cgGLDisableTextureParameter(getShaderParam(shader, paramName));
	}
}
//--------------------------------------------------------------------------------
// Set, enable and disable Methode for Texture cube
//--------------------------------------------------------------------------------
void Texturing::setShaderCubeTexture( char* filename,char *paramName)
{
	cgShader* shader = getShader(filename);

	shader->cgParam.push_back(cgGetNamedParameter(shader->cgProgram, paramName));
	shader->cgParamName.push_back(paramName);

	if (!shader->cgParam[shader->cgParam.size()-1])
	{
		printf("Unable to retrieve program parameters, Texture...\n");
	}
}

void Texturing::enableShaderCubeTexture(char* filename, char *paramName)
{
	cgShader* shader = getShader(filename);

	unsigned int textureID = cubeMapId;
	if(isParamSet(shader, paramName))
	{
		cgGLSetTextureParameter(getShaderParam(shader, paramName), textureID);
		cgGLEnableTextureParameter(getShaderParam(shader, paramName));
		
	}
}

void Texturing::disableShaderCubeTexture(char* filename, char *paramName)
{
	cgShader* shader = getShader(filename);
	if(isParamSet(shader, paramName))
	{
		cgGLDisableTextureParameter(getShaderParam(shader, paramName));
	}
}
//--------------------------------------------------------------------------------
// Set and update Methode for StateMatrix
//--------------------------------------------------------------------------------
void Texturing::setShaderStateMatrix(string filename, char *paramName)
{
	cgShader* shader = getShader(filename);

	shader->cgParam.push_back(cgGetNamedParameter(shader->cgProgram, paramName));
	shader->cgParamName.push_back(paramName);

	if (!shader->cgParam[shader->cgParam.size()-1])
	{
		printf("Unable to retrieve program parameters, StateMatrix...\n");
	}
}

void Texturing::updateShaderStateMatrix(string filename, char *paramName, CGGLenum matrix, CGGLenum transform)
{
	cgShader* shader = getShader(filename);
	if(isParamSet(shader, paramName)) cgGLSetStateMatrixParameter(getShaderParam(shader, paramName), matrix, transform);
}

// Returns a shader by name
cgShader* Texturing::getShader(string filename)
{
	for(unsigned int i = 0; i < vertexShaders.size(); i++)
	{
		if(vertexShaders[i]->filename == filename)
		{
			return vertexShaders[i];
		}
	}
	for(unsigned int i = 0; i < fragmentShaders.size(); i++)
	{
		if(fragmentShaders[i]->filename == filename)
		{
			return fragmentShaders[i];
		}
	}
	printf("Shader not found...\n");
	return NULL;
}

// Returns a param of a shader by name
CGparameter Texturing::getShaderParam(cgShader* shader, char *param)
{
	for(unsigned int a = 0; a < shader->cgParamName.size(); a++)
	{
		if(shader->cgParamName[a] == param)
		{
			 return shader->cgParam[a];
		}
	}
	printf("Shader param not found...\n");
}

// Returns true if a shader is loaded (identify - true=vertex/false=fragment)
bool Texturing::isShaderLoaded(string filename, bool identify)
{
	vector<cgShader*> shaders = identify ? vertexShaders : fragmentShaders;

	for(unsigned int i = 0; i < shaders.size(); i++)
	{
		if(shaders[i]->filename == filename)
		{
			return TRUE;
		}
	}
	printf("Shader was not loaded...\n");
	return FALSE;
}

// Returns true if a param is set to a shader
bool Texturing::isParamSet(cgShader* shader, char *param)
{
	for(unsigned int a = 0; a < shader->cgParamName.size(); a++)
	{
		if(shader->cgParamName[a] == param)
		{
			return TRUE;
		}
	}
	printf("Shader param was not set...\n");
	return FALSE;
}

// Error handler for CG
void Texturing::cgErrorCallback(void)
{
  CGerror lastError = cgGetError();
  if (lastError) {
    printf("%s\n",cgGetErrorString(lastError));
    printf("%s\n", cgGetLastListing(cgContext));
    printf("Cg error, exiting...\n");
    //exit(0);
  }
}

// Texture by filename
unsigned int Texturing::getTextureByName(char *filename)
{
	for(unsigned int i = 0; i < texture_names.size(); i++)
	{
		if(texture_names[i] == filename)
		{
			return texture[i];
		}
	}
	return -1;
}

// Is texture loaded
bool Texturing::isLoaded(char *filename)
{
	for(unsigned int i = 0; i < texture_names.size(); i++)
	{
		if(texture_names[i] == filename)
		{
			return TRUE;
		}
	}
	return FALSE;
}

// Load a texture
void Texturing::loadTexture(char *filename, unsigned int slot)
{
	glEnable(GL_MULTISAMPLE_ARB);

	unsigned int img;
	ilGenImages(1, &img);
	ilLoadImage(filename);
	ILinfo info;
	iluGetImageInfo(&info);

	glGenTextures(1, &texture[slot]);
	glBindTexture(GL_TEXTURE_2D, texture[slot]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.Width, info.Height, 0, info.Format, info.Type, info.Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	float maximumAnistropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maximumAnistropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maximumAnistropy);
	const char* errorstr = iluErrorString(ilGetError());
	const unsigned char* glErrStr = gluErrorString(glGetError());
	this->texture_names.push_back(filename);

	glGenTextures(1, &cubeMapId);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, cubeMapId);
	makeCubeMap();
}


// Set the material color with RGB
void Texturing::setStaticMaterialColor(int side, double r, double g, double b)
{
	float	amb[4], dif[4], spe[4];
	int	i, mat;
	dif[0] = r;
	dif[1] = g;
	dif[2] = b;
	for( i=0; i<3; i++)
	{
		amb[i] = .1 * dif[i];
		spe[i] = .5;
	}
	amb[3] = dif[3] = spe[3] = 1.0;
	switch(side)
	{
		case 1:	mat = GL_FRONT; break;
		case 2:	mat = GL_BACK; break;
		default: mat = GL_FRONT_AND_BACK; break;
	}
	glMaterialfv(mat, GL_AMBIENT, amb);
	glMaterialfv(mat, GL_DIFFUSE, dif);
	glMaterialfv(mat, GL_SPECULAR, spe);
	glMaterialf(mat, GL_SHININESS, 20);
}
