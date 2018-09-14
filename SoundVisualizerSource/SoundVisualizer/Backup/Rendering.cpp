#include "Rendering.h"
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Initializes the graphic-engine
 */

// Constants
static const int WINDOW_WIDTH  = 1600;
static const int WINDOW_HEIGHT = 900;

// Camera movement
static float cameraX = 0.0f;
static float cameraY = 0.0f;
static float cameraZ = -200.0f;

static float cameraRotateY = 0.0f;
static float cameraRotateZ = 0.0f;

static const float movementFactor = 0.6f;

static bool keyArrowUp    = FALSE;
static bool keyArrowDown  = FALSE;
static bool keyArrowRight = FALSE;
static bool keyArrowLeft  = FALSE;

static bool mouseLeft   = FALSE;
static bool mouseRight  = FALSE;
static bool mouseMiddle = FALSE;

static int mouseX = 0;
static int mouseY = 0;

// FPS
static float fps = 0;
static int frameCount = 0;
static int currentTime = 0, previousTime = 0;

static GLvoid *font_style = GLUT_BITMAP_HELVETICA_18;

// Lights
static GLfloat light1[]         = { 0.0f, 20.0f, -10.0f, 0.0f };
static GLfloat ambientLight1[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
static GLfloat diffuseLight1[]  = { 0.8f, 0.8f, 0.8, 1.0f };
static GLfloat specularLight1[] = { 0.5f, 0.5f, 0.5f, 1.0f };

static GLfloat light2[]         = { 0.0f, 20.0f, 50.0f, 0.0f };
static GLfloat ambientLight2[]  = { 0.3f, 0.3f, 0.3f, 1.0f };
static GLfloat diffuseLight2[]  = { 0.8f, 0.8f, 0.8, 1.0f };
static GLfloat specularLight2[] = { 0.5f, 0.5f, 0.5f, 1.0f };

static float red[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static float blue[] = { 1.0f, 1.0f, 1.0f, 1.0f };

static float lightPos[] = {2,10,100};
static float viewPos[] = {0,1,4};

static float shiniess = 1;

// Action
static bool action1Positive = FALSE;
static bool action1Negative = FALSE;

static bool action2Positive = FALSE;
static bool action2Negative = FALSE;

static bool action3Positive = FALSE;
static bool action3Negative = FALSE;

/*
 *	Controls: LeftClick and RightClick for scaling
 *			  MouseMiddle for translating
 */

// Constructor
Rendering::Rendering(void)
{
	producing = new Producing();
	texturing = new Texturing();
}

// Destructor
Rendering::~Rendering(void)
{
}

// Initializes the rendering
void Rendering::initialize(void)
{
	std::cout << "initializing..." << endl;
	glfwInit();

	// Create window
	glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);
	glfwSetWindowTitle("SoundVisualizer");

	std::cout << "load C for Graphics..." << endl;
	this->texturing->initCG();

	// Bind event handler
	glfwSetKeyCallback(handleKeyboardInput);
	glfwSetMouseButtonCallback(handleMouseInput);
	glfwSetMouseWheelCallback(handleMouseWheel);
	glfwSetMousePosCallback(handleMousePosition);

	std::cout << "starting main loop..." << endl;
	this->startMainLoop();
}

// Starts the main loop
void Rendering::startMainLoop(void)
{
	while(true)
	{
		// Calculate scene action
		this->calculateAction();

		// Calculate FPS
		this->calculateFPS();

		// Switch on lighting (or you don't see anything)
		this->updateLightingAndProjection();

		// Set background color
		glClearColor(0.1, 0.1, 0.12, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw the scene
		this->updateScene();

		// Make it appear (before this, it's hidden in the rear buffer)
		glFlush();
		glfwSwapBuffers();

		// Break loop when windows have been closed
		if(!glfwGetWindowParam(GLFW_OPENED))
		{
			break;
		}
	}
	// Quit openGL mode
	glfwTerminate();
}

// Set lighting and camera
void Rendering::updateLightingAndProjection()
{
	glEnable( GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	glEnable(GL_LIGHTING);

	glRotated(90, 0,0,1);

	glLightfv(GL_LIGHT1, GL_POSITION, light1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight1);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specularLight1);
	glEnable(GL_LIGHT1);

	glLightfv(GL_LIGHT2, GL_POSITION, light2);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambientLight2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuseLight2);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specularLight2);
	glEnable(GL_LIGHT2);

	glClearColor(1,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // init viewport to canvassize
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Camera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)WINDOW_WIDTH/(GLfloat)WINDOW_HEIGHT,0.1f,400.0f);

	glRotated(cameraRotateY, 0, 1, 0);
	glRotated(cameraRotateZ, 1, 0, 0);

	glTranslated(cameraX, cameraY, cameraZ);

	glMatrixMode(GL_MODELVIEW);
}

// Draw the scene
void Rendering::updateScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	SHADER ON
	this->texturing->enableShaders();
	//	########################################

	//	OBJECT: Background
	this->texturing->setVertexShader("v_diffuse", "vfunc_diffuse");
	this->texturing->setFragmentShader("f_diffuse", "ffunc_diffuse");
	//	----------------------------------------
	this->producing->drawCuboid(Vec3(0,0,-54.5),60,30,1);
	//	----------------------------------------
	this->texturing->setShaderParam4f("v_diffuse", "constantColor", red, false);
	this->texturing->setShaderParam3f("v_diffuse", "lightPos", lightPos, true);
	this->texturing->setShaderParam3f("v_diffuse", "viewPos", viewPos, true);
	this->texturing->setShaderParam1f("f_diffuse", "shiniess", shiniess, false);
	this->texturing->setShaderStateMatrix("v_diffuse", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderStateMatrix("v_diffuse", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->setShaderStateMatrix("v_diffuse", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderTexture("f_diffuse", "tex", "../Assets/wall.bmp", false);
	//	----------------------------------------
	this->texturing->clearVertex("v_diffuse");
	this->texturing->clearFragment("f_diffuse");
	//	----------------------------------------
	
	this->texturing->disableShaders();
	
	this->texturing->enableShaders();

	//	OBJECT: Side left
	this->texturing->setVertexShader("v_specular", "vfunc_specular");
	this->texturing->setFragmentShader("f_specular", "ffunc_specular");
	//	----------------------------------------
	this->producing->drawCuboid(Vec3(-60.2,0,65),1,30,120);
	//	----------------------------------------
	this->texturing->setShaderParam4f("v_specular", "constantColor", red, false);
	this->texturing->setShaderParam3f("v_specular", "lightPos", lightPos, true);
	this->texturing->setShaderParam3f("v_specular", "viewPos", viewPos, true);
	this->texturing->setShaderParam1f("f_specular", "shiniess", shiniess, false);
	this->texturing->setShaderStateMatrix("v_specular", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderStateMatrix("v_specular", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->setShaderStateMatrix("v_specular", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderTexture("f_specular", "tex", "../Assets/wall2.bmp", false);
	//	----------------------------------------
	this->texturing->clearVertex("v_specular");
	this->texturing->clearFragment("f_specular");
	//	----------------------------------------

	//	OBJECT: Side right
	/*this->texturing->setVertexShader("vp_color", "vp_color");
	this->texturing->setFragmentShader("fp_color", "fp_color");
	//	----------------------------------------
	this->producing->drawCuboid(Vec3(60.2,0,65),1,30,120);
	//	----------------------------------------
	this->texturing->setShaderParam4f("vp_color", "constantColor", red, false);
	this->texturing->setShaderParam3f("vp_color", "lightPos", lightPos, true);
	this->texturing->setShaderParam3f("vp_color", "viewPos", viewPos, true);
	this->texturing->setShaderParam1f("fp_color", "shiniess", shiniess, false);
	this->texturing->setShaderStateMatrix("vp_color", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderStateMatrix("vp_color", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->setShaderStateMatrix("vp_color", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderTexture("fp_color", "tex", "../Assets/wall2.bmp", false);
	//	----------------------------------------

	//	OBJECT: Ground
	this->texturing->setVertexShader("vp_color", "vp_color");
	this->texturing->setFragmentShader("fp_color", "fp_color");
	//	----------------------------------------
	this->producing->drawCuboid(Vec3(0,-31,65),61,1,120);
	//	----------------------------------------
	this->texturing->setShaderParam4f("vp_color", "constantColor", blue, false);
	this->texturing->setShaderParam3f("vp_color", "lightPos", lightPos, true);
	this->texturing->setShaderParam3f("vp_color", "viewPos", viewPos, true);
	this->texturing->setShaderParam1f("fp_color", "shiniess", shiniess, false);
	this->texturing->setShaderStateMatrix("vp_color", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderStateMatrix("vp_color", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->setShaderStateMatrix("vp_color", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderTexture("fp_color", "tex", "../Assets/ground.bmp", false);
	//	----------------------------------------

	//	OBJECT: Top
	this->texturing->setVertexShader("vp_color", "vp_color");
	this->texturing->setFragmentShader("fp_color", "fp_color");
	//	----------------------------------------
	this->producing->drawCuboid(Vec3(0,31,65),61,1,120);
	//	----------------------------------------
	this->texturing->setShaderParam4f("vp_color", "constantColor", blue, false);
	this->texturing->setShaderParam3f("vp_color", "lightPos", lightPos, true);
	this->texturing->setShaderParam3f("vp_color", "viewPos", viewPos, true);
	this->texturing->setShaderParam1f("fp_color", "shiniess", shiniess, false);
	this->texturing->setShaderStateMatrix("vp_color", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderStateMatrix("vp_color", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->setShaderStateMatrix("vp_color", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderTexture("fp_color", "tex", "../Assets/ground.bmp", false);
	//	----------------------------------------

	//	OBJECT: Sphere
	this->texturing->setVertexShader("vp_color2", "vp_color");
	this->texturing->setFragmentShader("fp_color2", "fp_color");
	//	----------------------------------------
	//this->producing->drawCuboid(Vec3(0,0,65),3,3,3);
	this->producing->drawSphere(Vec3(0,0,65),5);
	//	----------------------------------------
	this->texturing->setShaderParam4f("vp_color2", "constantColor", blue, false);
	this->texturing->setShaderParam3f("vp_color2", "lightPos", lightPos, true);
	this->texturing->setShaderParam3f("vp_color2", "viewPos", viewPos, false);
	this->texturing->setShaderStateMatrix("vp_color2", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderStateMatrix("vp_color2", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->setShaderStateMatrix("vp_color2", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->setShaderTexture("fp_color2", "tex", "../Assets/red.bmp", false);*/
	//	----------------------------------------

	//	########################################
	this->texturing->disableShaders();
	//	SHADER OFF

	this->producing->drawCube(Vec3(lightPos[0],lightPos[1],lightPos[2]),0.5);

	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(-15, 15, -10, 10, -20, 20);

	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
	this->drawFPS(13, 9.3, 0, "FPS: %4.2f", fps);
}

// Build DisplayLists
void Rendering::buildDisplayLists()
{
	glNewList(ROOM, GL_COMPILE);
	//	BACKGROUND
	this->producing->rotate(Vec3(0,1,0), 180);
	this->producing->drawPlane(Vec3(0,3,-53.5), 60, 30, 1);
	//	----------------------------------------
	//	SIDE LEFT
	this->producing->drawPlane(Vec3(-61,3,5.5), 60, 30, 2);
	//	----------------------------------------
	//	SIDE RIGHT
	this->producing->drawPlane(Vec3(61,3,5.5), 60, 30, 3);
	//	----------------------------------------
	//	GROUND
	this->producing->rotate(Vec3(0,1,0), 180);
	this->producing->drawPlane(Vec3(0,-26,5.5), 60, 60, 0);
	//	----------------------------------------
	//	TOP
	this->producing->rotate(Vec3(1,0,0), 180);
	this->producing->drawPlane(Vec3(0,32,5.5), 60, 60, 0);
	//	----------------------------------------
	glEndList();


	/*
	glNewList(BAR, GL_COMPILE);
	//	BAR1 Top
	this->texturing->setMaterialColor(0, 2, 2, 2);
	this->texturing->setTexture("Assets/bar_table.bmp", 0);
	this->producing->drawCuboid(Vec3(9.7,-11.5,-25), 30, 1, 5);
	//	----------------------------------------
	//	BAR1 Bottom
	this->texturing->setMaterialColor(0, 2, 2, 2);
	this->texturing->setTexture("Assets/bar_root.bmp", 0);
	this->producing->drawAngleCube(Vec3(10,-19,-25), 20, 8, 3, Vec3(0,1,0), 45);
	//	----------------------------------------

	//	BAR1 Bottom
	this->texturing->setMaterialColor(0, 2, 2, 2);
	this->texturing->setTexture("Assets/bar_root.bmp", 0);
	this->producing->drawAngleCube(Vec3(0,-19,0), 3, 8, 20, Vec3(0,1,0), 45);
	//	----------------------------------------
	glEndList();*/

	glNewList(BAR, GL_COMPILE);
	this->producing->drawCube(Vec3(0, 0, 60), 5);
	glEndList();
}

// Calculate scene action
void Rendering::calculateAction()
{
	// Forward and backward
	if(keyArrowUp)
	{
		cameraZ += movementFactor;
	}
	else if(keyArrowDown)
	{
		cameraZ -= movementFactor;
	}

	// Sidewards
	if(keyArrowRight)
	{	
		cameraX -= movementFactor;
	}
	else if(keyArrowLeft)
	{
		cameraX += movementFactor;
	}

	// Rotation Y
	if(mouseLeft)
	{
		if(WINDOW_WIDTH / 2 - mouseX > 20)
		{
			cameraRotateY += -2 * ((WINDOW_WIDTH / 2 - mouseX) / (WINDOW_WIDTH / 1.5));
		}
		else if(WINDOW_WIDTH / 2 - mouseX < -20)
		{
			cameraRotateY -= 2 * ((WINDOW_WIDTH / 2 - mouseX) / (WINDOW_WIDTH / 1.5));
		}
	}

	// Rotation Z
	if(mouseRight)
	{
		if(WINDOW_HEIGHT / 2 - mouseY > 20)
		{
			cameraRotateZ -= 2 * ((WINDOW_HEIGHT / 2 - mouseY) / (WINDOW_HEIGHT / 1.5));
		}
		else if(WINDOW_HEIGHT / 2 - mouseY < -20)
		{
			cameraRotateZ += -2 * ((WINDOW_HEIGHT / 2 - mouseY) / (WINDOW_HEIGHT / 1.5));
		}
	}

	// Action
	if(action1Positive)
	{
		lightPos[0] -= 0.4;
	}
	else if(action1Negative)
	{
		lightPos[0] += 0.4;
	}

	if(action2Positive)
	{
		lightPos[2] += 0.4;
	}
	else if(action2Negative)
	{
		lightPos[2] -= 0.4;
	}

	if(action3Positive)
	{
		lightPos[1] -= 0.4;
	}
	else if(action3Negative)
	{
		lightPos[1] += 0.4;
	}

	viewPos[0] = cameraX;
	viewPos[1] = cameraY;
	viewPos[2] = cameraZ;
}

// Handle keyboard input
void GLFWCALL Rendering::handleKeyboardInput(int key, int action)
{
	if(key == GLFW_KEY_UP)
	{
		if(action == GLFW_PRESS)
		{
			keyArrowUp = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			keyArrowUp = FALSE;
		}
	}
	else if(key == GLFW_KEY_DOWN)
	{
		if(action == GLFW_PRESS)
		{
			keyArrowDown = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			keyArrowDown = FALSE;
		}
	}
	else if(key == GLFW_KEY_RIGHT)
	{
		if(action == GLFW_PRESS)
		{
			keyArrowRight = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			keyArrowRight = FALSE;
		}
	}
	else if(key == GLFW_KEY_LEFT)
	{
		if(action == GLFW_PRESS)
		{
			keyArrowLeft = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			keyArrowLeft = FALSE;
		}
	}
	else if(key == GLFW_KEY_F1)
	{
		if(action == GLFW_PRESS)
		{
			action1Positive = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			action1Positive = FALSE;
		}
	}
	else if(key == GLFW_KEY_F2)
	{
		if(action == GLFW_PRESS)
		{
			action1Negative = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			action1Negative = FALSE;
		}
	}
	else if(key == GLFW_KEY_F3)
	{
		if(action == GLFW_PRESS)
		{
			action2Positive = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			action2Positive = FALSE;
		}
	}
	else if(key == GLFW_KEY_F4)
	{
		if(action == GLFW_PRESS)
		{
			action2Negative = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			action2Negative = FALSE;
		}
	}
	else if(key == GLFW_KEY_F5)
	{
		if(action == GLFW_PRESS)
		{
			action3Positive = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			action3Positive = FALSE;
		}
	}
	else if(key == GLFW_KEY_F6)
	{
		if(action == GLFW_PRESS)
		{
			action3Negative = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			action3Negative = FALSE;
		}
	}
	else if(key == GLFW_KEY_ESC && action == GLFW_PRESS)
	{
		glfwTerminate();
	}
}

// Handle mouse wheel
void GLFWCALL Rendering::handleMouseWheel(int pos)
{
	cameraY = pos;
}

// Handle mouse input
void GLFWCALL Rendering::handleMouseInput(int button, int action)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if(action == GLFW_PRESS)
		{
			mouseLeft = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			mouseLeft = FALSE;
		}
	}
	else if(button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if(action == GLFW_PRESS)
		{
			mouseRight = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			mouseRight = FALSE;
		}
	}
	else if(button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if(action == GLFW_PRESS)
		{
			mouseMiddle = TRUE;
		}
		else if(action == GLFW_RELEASE)
		{
			mouseMiddle = FALSE;
		}
	}
}

// Calculate the FPS
void Rendering::calculateFPS()
{
    frameCount++;

    currentTime = glutGet(GLUT_ELAPSED_TIME);

    int timeInterval = currentTime - previousTime;

    if(timeInterval > 1000)
    {
        fps = frameCount / (timeInterval / 1000.0f);
        previousTime = currentTime;
        frameCount = 0;
    }
}

// Print a string to the given coordinates
void Rendering::drawFPS(float x, float y, float z, char* format, ...)
{
	va_list args;
	int len;
	int i;
	char * text;

	va_start(args, format);
	len = _vscprintf(format, args) + 1; 
	text = (char *)malloc(len * sizeof(char));
	vsprintf_s(text, len, format, args);
	va_end(args);
	glRasterPos3f (x, y, z);
    for (i = 0; text[i] != '\0'; i++)
        glutBitmapCharacter(font_style, text[i]);

	free(text);
}

// Handle mouse postion event
void GLFWCALL Rendering::handleMousePosition(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

// Main-methode
int main()
{
	std::cout << "SoundVisualizer with OpenGL and CG by Gunnar Busch, Hadeer Khaled and Philipp Petzold" << endl;
	Rendering *engine = new Rendering();
	engine->initialize();
	return 0;
}