#include "Texturing.h"
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Creating materials and textures
 */

// Constructor
Texturing::Texturing(void)
{
	ilInit();
	iluInit();

	
}

// Destructor
Texturing::~Texturing(void)
{

}

void Texturing::initCG(void)
{
	cgContext = cgCreateContext();

	vertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	cgGLSetOptimalOptions(vertexProfile);

	fragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    cgGLSetOptimalOptions(fragmentProfile);

	cgSetErrorCallback(cgErrorCallback);
}

void Texturing::enableShaders(void)
{
	cgGLEnableProfile( vertexProfile );
	cgGLEnableProfile( fragmentProfile );
}

void Texturing::disableShaders(void)
{
	cgGLDisableProfile( vertexProfile );
	cgGLDisableProfile( fragmentProfile );
}

void Texturing::clearVertex(string filename)
{
	//cgGLUnbindProgram( vertexProfile );
	//cgGLUnloadProgram( getShader(filename).cgProgram );
}

void Texturing::clearFragment(string filename)
{
	//cgGLUnbindProgram( fragmentProfile );
	//cgGLUnloadProgram( getShader(filename).cgProgram );
}

void Texturing::setVertexShader(string filename, char *function)
{
	
	if(!isShaderLoaded(filename, true))
	{
		cgShader vertexShader;
		vertexShader.filename = filename;
		filename = "../Shaders/Vertex/" + filename + ".cg";
		vertexShader.cgProgram = cgCreateProgramFromFile(cgContext, CG_SOURCE, filename.c_str(), vertexProfile, function, 0);
		vertexShaders.push_back(vertexShader);

		cgGLLoadProgram(vertexShader.cgProgram);
		cgGLBindProgram(vertexShader.cgProgram);
	}
	else
	{
		cgGLBindProgram( getShader(filename).cgProgram );
	}
}

void Texturing::setFragmentShader(string filename, char *function)
{
	
	if(!isShaderLoaded(filename, false))
	{
		cgShader fragmentShader;
		fragmentShader.filename = filename;
		filename = "../Shaders/Fragment/" + filename + ".cg";
		fragmentShader.cgProgram = cgCreateProgramFromFile(cgContext, CG_SOURCE, filename.c_str(), fragmentProfile, function, 0);
		fragmentShaders.push_back(fragmentShader);

		cgGLLoadProgram(fragmentShader.cgProgram);
		cgGLBindProgram(fragmentShader.cgProgram);
	}
	else
	{
		cgGLBindProgram( getShader(filename).cgProgram );
	}
}

void Texturing::setShaderParam4f(string filename, char *paramName, float param[4], bool update)
{
	cgShader shader = getShader(filename);

	if(!isParamSet(shader, paramName))
	{
		shader.cgParam.push_back(cgGetNamedParameter(shader.cgProgram, paramName));
		shader.cgParamName.push_back(paramName);
		if (!shader.cgParam[shader.cgParam.size()-1])
		{
			printf("Unable to retrieve program parameters, Param4f...\n");
			//exit(0);
		}
		update = true;
	}
	if(update)
	{
		cgGLSetParameter4f(getShaderParam(shader, paramName), param[0], param[1], param[2], param[3]);
	}
}

void Texturing::setShaderParam3f(string filename, char *paramName, float param[3], bool update)
{
	cgShader shader = getShader(filename);

	if(!isParamSet(shader, paramName))
	{
		shader.cgParam.push_back(cgGetNamedParameter(shader.cgProgram, paramName));
		shader.cgParamName.push_back(paramName);
		if (!shader.cgParam[shader.cgParam.size()-1])
		{
			printf("Unable to retrieve program parameters, Param4f...\n");
			//exit(0);
		}
		update = true;
	}
	if(update)
	{
		cgGLSetParameter3f(getShaderParam(shader, paramName), param[0], param[1], param[2]);
	}
}

void Texturing::setShaderParam1f(string filename, char *paramName, float param, bool update)
{
	cgShader shader = getShader(filename);

	if(!isParamSet(shader, paramName))
	{
		shader.cgParam.push_back(cgGetNamedParameter(shader.cgProgram, paramName));
		shader.cgParamName.push_back(paramName);
		if (!shader.cgParam[shader.cgParam.size()-1])
		{
			printf("Unable to retrieve program parameters, Param4f...\n");
			//exit(0);
		}
		update = true;
	}
	if(update)
	{
		cgGLSetParameter1f(getShaderParam(shader, paramName), param);
	}
}

void Texturing::setShaderTexture(char* filename, char *paramName, char* texture, bool update)
{
	cgShader shader = getShader(filename);
	unsigned int textureID;

	if(!isParamSet(shader, paramName))
	{
		shader.cgParam.push_back(cgGetNamedParameter(shader.cgProgram, paramName));
		shader.cgParamName.push_back(paramName);
		if (!shader.cgParam[shader.cgParam.size()-1])
		{
			printf("Unable to retrieve program parameters, Texture...\n");
			//exit(0);
		}
		update = true;
	}
	if(update)
	{
		if(!this->isLoaded(texture))
		{
			this->loadTexture(texture, this->texture_names.size());
		}
		textureID = getTextureByName(texture);
		glBindTexture(GL_TEXTURE_2D, textureID);
		cgGLSetTextureParameter(getShaderParam(shader, paramName), textureID);
	}
}

void Texturing::setShaderStateMatrix(string filename, char *paramName, CGGLenum matrix, CGGLenum transform)
{
	cgShader shader = getShader(filename);

	if(!isParamSet(shader, paramName))
	{
		shader.cgParam.push_back(cgGetNamedParameter(shader.cgProgram, paramName));
		shader.cgParamName.push_back(paramName);
		if (!shader.cgParam[shader.cgParam.size()-1])
		{
			printf("Unable to retrieve program parameters, StateMatrix...\n");
			//exit(0);
		}
	}
	cgGLSetStateMatrixParameter(getShaderParam(shader, paramName), matrix, transform);
}

cgShader Texturing::getShader(string filename)
{
	cgShader shader;
	for(unsigned int i = 0; i < vertexShaders.size(); i++)
	{
		if(vertexShaders[i].filename == filename)
		{
			shader = vertexShaders[i];
			break;
		}
	}
	for(unsigned int i = 0; i < fragmentShaders.size(); i++)
	{
		if(fragmentShaders[i].filename == filename)
		{
			shader = fragmentShaders[i];
			break;
		}
	}
	return shader;
}

bool Texturing::isShaderLoaded(string filename, bool identify)
{
	vector<cgShader> shaders = identify ? vertexShaders : fragmentShaders;

	for(unsigned int i = 0; i < shaders.size(); i++)
	{
		if(shaders[i].filename == filename)
		{
			return TRUE;
		}
	}
	return FALSE;
}

bool Texturing::isParamSet(cgShader shader, char *param)
{
	for(unsigned int a = 0; a < shader.cgParamName.size(); a++)
	{
		if(shader.cgParamName[a] == param)
		{
			return TRUE;
		}
	}
	return FALSE;
}

CGparameter Texturing::getShaderParam(cgShader shader, char *param)
{
	CGparameter cgParam;
	for(unsigned int a = 0; a < shader.cgParamName.size(); a++)
	{
		if(shader.cgParamName[a] == param)
		{
			 cgParam = shader.cgParam[a];
			 break;
		}
	}
	return cgParam;
}

void Texturing::cgErrorCallback(void)
{
  CGerror lastError = cgGetError();
  if (lastError) {
    printf("%s\n",cgGetErrorString(lastError));
    //printf("%s\n", cgGetLastListing(cgContext));
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

void Texturing::loadTexture(char *filename, unsigned int slot)
{
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
	const char* errorstr = iluErrorString(ilGetError());
	const unsigned char* glErrStr = gluErrorString(glGetError());
	this->texture_names.push_back(filename);
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
