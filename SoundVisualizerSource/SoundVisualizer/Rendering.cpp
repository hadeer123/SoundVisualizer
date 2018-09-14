#include "Rendering.h"
/*
 *	Authors: Gunnar Busch, Hadeer Khaled and Philipp Petzold
 *	Description: Initializes the graphic-engine and build the scene
 */

// ------------------------------------
// Window Size params
// ------------------------------------
static const int WINDOW_WIDTH  = 1920; 
static const int WINDOW_HEIGHT = 1080;

// ------------------------------------
// Camera Movement and other action params
// ------------------------------------
static float view[] = { -48, -11, -172.5 };

static float cameraRotateY = -39.0f;
static float cameraRotateZ = 0.0f;

static float cameraRotateY_temp = 0.0f;

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

static bool action1Positive = FALSE;
static bool action1Negative = FALSE;

static bool action2Positive = FALSE;
static bool action2Negative = FALSE;

static bool action3Positive = FALSE;
static bool action3Negative = FALSE;

// ------------------------------------
// Camera Motion
// ------------------------------------

static bool isCamMotion     = FALSE;
static bool isCamCalibrated = TRUE;

static float camMotionStartingPoint[] = { -30, 10, -155 };

static int camXDirection = 1;
static int camYDirection = 1;
static int camZDirection = 1;

static int camRotYDirection = 1;
static int camRotZDirection = 1;

// ------------------------------------
// Audio params
// ------------------------------------
static bool play_1      = FALSE;
static bool isPlaying_1 = FALSE;

static bool play_2      = FALSE;
static bool isPlaying_2 = FALSE;

static bool play_3      = FALSE;
static bool isPlaying_3 = FALSE;

// ------------------------------------
// FPS Counter params
// ------------------------------------
static float fps = 0;

static int frameCount = 0;
static int currentTime = 0, previousTime = 0;

static GLvoid *font_style = GLUT_BITMAP_HELVETICA_18;

// ------------------------------------
// Lightning with all relevant attributes
// ------------------------------------
static float globalAmbientLight[] = { 0.08, 0.08, 0.08, 1 };

// Light 1
static float lightPosition_1[]  = { 0, -20, 80, 0 };
static float lightColor_1[]     = { 1, 0.9, 0.8, 1 };
static float lightDirection_1[] = { 0,-1, 0 };

// Material
static float defaultMaterialAmbient[]	= { 0.004, 0.004, 0.004, 1 };
static float defaultMaterialAmbient2[]	= { 0.05, 0.05, 0.05, 1 };
static float defaultMaterialEmissive[]	= { 0.02, 0.02,  0.02, 1 };
static float defaultMaterialDiffuse[]	= { 0.7,0.7,0.7 };
static float defaultMaterialSpecular[]	= { 0.9,0.9,0.9 };
static float defaultShiniess = 1.2;

static float emissiveBoost = 2;

// Some Colors
static float white[] = { 0.95, 0.95, 0.95, 1 };
static float color1[] = {1.0,0.0,0.0};
static float color2[] = {0.0,1.0,0.0};
static float color3[] = {0.0,0.0,1.0};
static float color4[] = {1.0,1.0,0.0};
static float color5[] = {0.0,1.0,1.0};

// the enviroument mapping attributes (discoball)
static float fresnelBias  = 0.3f;
static float fresnelScale = 0.4f;
static float fresnelPower = 0.5f;
static float etaRatio[]   = { 0.45, 0.49, 0.52 };
static float Ior = 1.33f;
static float reflection = 0.0f;

// ------------------------------------
// Animation attributes
// ------------------------------------
static float discoBallRotation = 0;
static bool isDiscoLightOn = FALSE;
static int time = 0;
static char* glowTexture = "../Assets/orange.bmp";
static int random = 0;

// ------------------------------------
// other attributes
// ------------------------------------
static float tiles = 4; //Tiles of the Dance-Floor

// Constructor
Rendering::Rendering(void)
{
	audio = new Audio();
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
	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 8, 8, 8, 8, 24, 0, GLFW_WINDOW);
	glfwSetWindowTitle("SoundVisualizer");

	// Bind event handler
	glfwSetKeyCallback(handleKeyboardInput);
	glfwSetMouseButtonCallback(handleMouseInput);
	glfwSetMouseWheelCallback(handleMouseWheel);
	glfwSetMousePosCallback(handleMousePosition);

	std::cout << "load CG..." << endl;
	this->texturing->initCG();

	std::cout << "Compile and load shaders..." << endl;
	this->loadShaders();

	// init audio
	audio->initializeAudioSystem();
	audio->loadingSound("../Media/epic.mp3", 1);
	audio->loadingSound("../Media/fool.mp3", 2);
	audio->loadingSound("../Media/liberty.mp3", 3);
	bassL = (float*)malloc(SPECLEN*sizeof(float));
	bassR = (float*)malloc(SPECLEN*sizeof(float));

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
		// Update sound
		this->updateSound();
		//Update animation
		this->updateAnimation();
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
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);	

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glClearColor(1,1,1,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
	glEnable(GL_LIGHTING);

	GLfloat lp1[4]  = { -50, 5, 60, 0};
	GLfloat full[4] = { 1, 1, 1, 1};
	glLightfv(GL_LIGHT1, GL_POSITION,  lp1);
	glLightfv( GL_LIGHT1, GL_DIFFUSE,  full);
	glLightfv( GL_LIGHT1, GL_SPECULAR, full);
	glEnable(GL_LIGHT1);

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

	glTranslated(view[0], view[1], view[2]);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_MULTISAMPLE); 
}

// Set and init shaders
void Rendering::loadShaders(void)
{
	// Diffuse Vertex
	this->texturing->loadVertexShader("v_diffuse", "vfunc_diffuse");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("v_diffuse", "constantColor", lightColor_1);
	this->texturing->setShaderParam3f("v_diffuse", "lightPos", lightPosition_1);
	this->texturing->setShaderParam3f("v_diffuse", "viewPos", view);
	this->texturing->setShaderStateMatrix("v_diffuse", "m2w");
	this->texturing->setShaderStateMatrix("v_diffuse", "m2wIT");
	this->texturing->setShaderStateMatrix("v_diffuse", "mvp");
	// ------------------------------------------------------------------------

	// Diffuse Fragment
	this->texturing->loadFragmentShader("f_diffuse", "ffunc_diffuse");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("f_diffuse", "globalAmbient", globalAmbientLight);
	this->texturing->setShaderParam4f("f_diffuse", "materialAmbient", defaultMaterialAmbient);
	this->texturing->setShaderParam4f("f_diffuse", "materialEmissive", defaultMaterialEmissive);
	this->texturing->setShaderTexture("f_diffuse", "tex");
	// ------------------------------------------------------------------------
	
	// Diffuse Constant Vertex
	this->texturing->loadVertexShader("v_diffuse_constant", "vfunc_diffuse_constant");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("v_diffuse_constant", "constantColor", white);
	this->texturing->setShaderStateMatrix("v_diffuse_constant", "mvp");
	// ------------------------------------------------------------------------

	// Diffuse Constant Fragment
	this->texturing->loadFragmentShader("f_diffuse_constant", "ffunc_diffuse_constant");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("f_diffuse_constant", "globalAmbient", globalAmbientLight);
	this->texturing->setShaderParam4f("f_diffuse_constant", "materialAmbient", defaultMaterialAmbient);
	this->texturing->setShaderParam4f("f_diffuse_constant", "materialEmissive", defaultMaterialEmissive);
	this->texturing->setShaderTexture("f_diffuse_constant", "tex");
	// ------------------------------------------------------------------------

	// Animated Textures Vertex
	this->texturing->loadVertexShader("v_animated_textures", "vfunc_animated_textures");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("v_animated_textures", "constantColor", white);
	this->texturing->setShaderStateMatrix("v_animated_textures", "mvp");
	// ------------------------------------------------------------------------

	// Animated Textures Fragment
	this->texturing->loadFragmentShader("f_animated_textures", "ffunc_animated_textures");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("f_animated_textures", "globalAmbient", globalAmbientLight);
	this->texturing->setShaderParam4f("f_animated_textures", "materialAmbient", defaultMaterialAmbient);
	this->texturing->setShaderParam4f("f_animated_textures", "materialEmissive", defaultMaterialEmissive);
	this->texturing->setShaderTexture("f_animated_textures", "tex1");
	this->texturing->setShaderTexture("f_animated_textures", "tex2");
	this->texturing->setShaderTexture("f_animated_textures", "tex3");
	this->texturing->setShaderParam1f("f_animated_textures", "time", time);
	this->texturing->setShaderParam1f("f_animated_textures", "factor", br1);
	this->texturing->setShaderParam1f("f_animated_textures", "factor2", br2);
	// ------------------------------------------------------------------------

	// Bump Vertex
	this->texturing->loadVertexShader("v_bump", "vfunc_bump");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("v_bump", "constantColor", lightColor_1);
	this->texturing->setShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->setShaderParam3f("v_bump", "viewPos", view);
	this->texturing->setShaderStateMatrix("v_bump", "m2w");
	this->texturing->setShaderStateMatrix("v_bump", "m2wIT");
	this->texturing->setShaderStateMatrix("v_bump", "mvp");
	// ------------------------------------------------------------------------

	// Bump Fragment
	this->texturing->loadFragmentShader("f_bump", "ffunc_bump");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->setShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->setShaderParam4f("f_bump", "materialEmissive", defaultMaterialEmissive);
	this->texturing->setShaderParam3f("f_bump", "materialDiffuse", defaultMaterialDiffuse);

	this->texturing->setShaderParam3f("f_bump", "materialSpecular", defaultMaterialSpecular);
	this->texturing->setShaderParam1f("f_bump", "emissiveBoost", emissiveBoost);
	this->texturing->setShaderParam1f("f_bump", "materialShiniess", defaultShiniess);

	this->texturing->setShaderTexture("f_bump", "colorTex");
	this->texturing->setShaderTexture("f_bump", "normTex");
	// ------------------------------------------------------------------------

	// Specular Vertex
	this->texturing->loadVertexShader("v_specular", "vfunc_specular");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("v_specular", "constantColor", lightColor_1);
	this->texturing->setShaderParam3f("v_specular", "lightPos", lightPosition_1);
	this->texturing->setShaderParam3f("v_specular", "viewPos", view);
	this->texturing->setShaderStateMatrix("v_specular", "m2w");
	this->texturing->setShaderStateMatrix("v_specular", "m2wIT");
	this->texturing->setShaderStateMatrix("v_specular", "mvp");
	// ------------------------------------------------------------------------

	// Specular Fragment
	this->texturing->loadFragmentShader("f_specular", "ffunc_specular");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam1f("f_specular", "shiniess", defaultShiniess);
	this->texturing->setShaderParam4f("f_specular", "globalAmbient", globalAmbientLight);
	this->texturing->setShaderParam4f("f_specular", "materialAmbient", defaultMaterialAmbient);
	this->texturing->setShaderParam4f("f_specular", "materialEmissive", defaultMaterialEmissive);
	this->texturing->setShaderTexture("f_specular", "text");
	// ------------------------------------------------------------------------

	// Spot Lighting Vertex
	this->texturing->loadVertexShader("v_spot_lighting", "vfunc_spot_lighting");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("v_spot_lighting", "constantColor", lightColor_1);
	this->texturing->setShaderParam3f("v_spot_lighting", "lightPos", lightPosition_1);
	this->texturing->setShaderParam3f("v_spot_lighting", "spotDir", lightDirection_1);
	this->texturing->setShaderParam3f("v_spot_lighting", "viewPos", view);
	this->texturing->setShaderStateMatrix("v_spot_lighting", "m2w");
	this->texturing->setShaderStateMatrix("v_spot_lighting", "m2wIT");
	this->texturing->setShaderStateMatrix("v_spot_lighting", "mvp");
	// ------------------------------------------------------------------------

	// Spot Lighting Fragment
	this->texturing->loadFragmentShader("f_spot_lighting", "ffunc_spot_lighting");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("f_spot_lighting", "globalAmbient", globalAmbientLight);
	this->texturing->setShaderParam4f("f_spot_lighting", "materialAmbient", defaultMaterialAmbient);
	this->texturing->setShaderParam4f("f_spot_lighting", "materialEmissive", defaultMaterialEmissive);
	this->texturing->setShaderTexture("f_spot_lighting", "tex");
	// ------------------------------------------------------------------------

	// Discoball Vertex
	this->texturing->loadVertexShader("v_discoball", "vfunc_discoball");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam3f("v_discoball", "etaRatio", etaRatio);
	this->texturing->setShaderParam1f("v_discoball", "fresnelBias", fresnelBias);
	this->texturing->setShaderParam1f("v_discoball", "fresnelPower", fresnelPower);
	this->texturing->setShaderParam1f("v_discoball", "fresnelScale", fresnelScale);
	this->texturing->setShaderParam4f("v_discoball", "constantColor", lightColor_1);
	this->texturing->setShaderParam3f("v_discoball", "lightPos", lightPosition_1);
	this->texturing->setShaderParam3f("v_discoball", "viewPos", view);
	this->texturing->setShaderStateMatrix("v_discoball", "m2w");
	this->texturing->setShaderStateMatrix("v_discoball", "m2wIT");
	this->texturing->setShaderStateMatrix("v_discoball", "mvp");
	// ------------------------------------------------------------------------

	// Discoball Fragment
	this->texturing->loadFragmentShader("f_discoball", "ffunc_discoball");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderCubeTexture("f_discoball", "environmentMap");
	this->texturing->setShaderTexture("f_discoball", "decal");
	this->texturing->setShaderTexture("f_discoball", "pattern");
	this->texturing->setShaderParam1f("f_discoball", "shiniess", defaultShiniess);
	this->texturing->setShaderParam1f("f_discoball", "time", time);
	this->texturing->setShaderParam1f("f_discoball", "soundInput", br2);
	this->texturing->setShaderParam4f("f_discoball", "globalAmbient", globalAmbientLight);
	this->texturing->setShaderParam4f("f_discoball", "materialAmbient", defaultMaterialAmbient);
	this->texturing->setShaderParam4f("f_discoball", "materialEmissive", defaultMaterialEmissive);
	// ------------------------------------------------------------------------

	// Floor Vertex
	this->texturing->loadVertexShader("v_floor", "vfunc_floor");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("v_floor", "constantColor", lightColor_1);
	this->texturing->setShaderParam3f("v_floor", "lightPos", lightPosition_1);
	this->texturing->setShaderParam3f("v_floor", "viewPos", view);
	this->texturing->setShaderStateMatrix("v_floor", "m2w");
	this->texturing->setShaderStateMatrix("v_floor", "m2wIT");
	this->texturing->setShaderStateMatrix("v_floor", "mvp");
	// ------------------------------------------------------------------------

	// Floor Fragment
	this->texturing->loadFragmentShader("f_floor", "ffunc_floor");
	// ---- PARAMS ------------------------------------------------------------
	this->texturing->setShaderParam4f("f_floor", "globalAmbient", globalAmbientLight);
	this->texturing->setShaderParam4f("f_floor", "materialAmbient", defaultMaterialAmbient);
	this->texturing->setShaderParam4f("f_floor", "materialEmissive", defaultMaterialEmissive);
	this->texturing->setShaderParam3f("f_floor", "materialDiffuse", defaultMaterialDiffuse);
	this->texturing->setShaderParam3f("f_floor", "materialSpecular", defaultMaterialSpecular);
	this->texturing->setShaderParam1f("f_floor", "materialShiniess", defaultShiniess);

	this->texturing->setShaderParam1f("f_floor", "reflection", reflection);
	this->texturing->setShaderParam1f("f_floor", "emissiveBoost", emissiveBoost);
	this->texturing->setShaderParam1f("f_floor", "soundInput", br1);
	this->texturing->setShaderParam1f("f_floor", "random", random);
	this->texturing->setShaderParam1f("f_floor", "tiles", tiles);

	this->texturing->setShaderParam3f("f_floor", "color1", color1);
	this->texturing->setShaderParam3f("f_floor", "color2", color2);
	this->texturing->setShaderParam3f("f_floor", "color3", color3);
	this->texturing->setShaderParam3f("f_floor", "color4", color4);
	this->texturing->setShaderParam3f("f_floor", "color5", color5);

	this->texturing->setShaderCubeTexture("f_floor", "cubeSampler");
	this->texturing->setShaderTexture("f_floor", "colorTex");
	this->texturing->setShaderTexture("f_floor", "normTex");
	// ------------------------------------------------------------------------

}

// Draw the scene
void Rendering::updateScene(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//	SHADER ON
	this->texturing->enableShaderProfiles();
	//	+++++++++++++++++++++++++++++++++++

	//	OBJECT: Wall - Background
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(0,0,-54.5);
	glScalef(60,30,1);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/wall.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/wall_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Front
	/*this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(0,0,184.4);
	glScalef(60,30,1);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/wall.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/wall_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");*/

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall bg Orange
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(0,-25.55,-54.5);
	glScalef(60.05,1.5,1.1);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall bg Orange top1
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(0,9.5,-54.5);
	glScalef(60.05,0.6,1.1);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall bg Orange top2
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(0,23.5,-54.5);
	glScalef(60.05,0.6,1.1);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Side left
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-60.2,0,65);
	glScalef(1,30,120);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/wall2.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/wall2_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall left Orange
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(-60.2,-25.55,65);
	glScalef(1.05,1.5,120.05);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall left Orange top1
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(-60.2, 9.5,65);
	glScalef(1.05,0.6,120.05);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall left Orange top2
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(-60.2, 23.5,65);
	glScalef(1.05,0.6,120.05);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Side right
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(60.2,0,65);
	glScalef(1,30,120);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/wall2.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/wall2_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall right Orange
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(60.2,-25.55,65);
	glScalef(1.05,1.5,120.05);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall right Orange top1
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(60.2, 9.5,65);
	glScalef(1.05,0.6,120.05);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall right Orange top2
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(60.2, 23.5,65);
	glScalef(1.05,0.6,120.05);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Floor
	this->texturing->bindVertexShader("v_floor");
	this->texturing->bindFragmentShader("f_floor");
	//	----------------------------------------
	glTranslatef(0,-31,80);
	glScalef(26,1.1,52);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_floor", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_floor", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_floor", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_floor", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_floor", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_floor", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderCubeTexture("f_floor", "cubeSampler");
	this->texturing->updateShaderParam4f("f_floor", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_floor", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->updateShaderParam1f("f_floor", "reflection", reflection);
	this->texturing->updateShaderParam1f("f_floor", "soundInput", br1);
	this->texturing->updateShaderParam1f("f_floor", "random", random);
	this->texturing->enableShaderTexture("f_floor", "colorTex", "../Assets/color.png");
	this->texturing->enableShaderTexture("f_floor", "normTex", "../Assets/normal.png");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_floor", "colorTex");
	this->texturing->disableShaderTexture("f_floor", "normTex");
	this->texturing->disableShaderTexture("f_floor", "cubeSampler");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Floor - right
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(26,-31,80);
	glScalef(1,1.5,52);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Floor - left
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-26,-31,80);
	glScalef(1,1.5,52);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Floor - back
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(0,-31,27);
	glScalef(27,1.5,1);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Floor - front
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(0,-31,133);
	glScalef(27,1.5,1);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Ground
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(0,-31,65);
	glScalef(61,1,120);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/ground.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/ground_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Wall - Top
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(0,31,65);
	glScalef(61,1,120);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/ground.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/ground_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Bar - BigBox
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(12,-9,-48);
	glScalef(40,20,3);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/bigbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/bigbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Bar - BigBox
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glRotatef(90,0,1,0);
	glTranslatef(10,-9,55);
	glScalef(40,20,3);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/bar_table.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/bar_table_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Table1 - Top
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(6.7,-14,-25);
	glScalef(30, 1, 5);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/bar_table.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/bar_table_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Table1 - Bottom
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(7,-21.5,-25);
	glScalef(20, 8, 3);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/bar_root.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/bar_root_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Table1 Orange
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(7,-19.23,-25);
	glScalef(20.1, 1.3, 3.1);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Table2 - Top
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glRotatef(90,0,1,0);
	glTranslatef(-5.3,-14,34);
	glScalef(30, 1, 5);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/bar_table.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/bar_table_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Table2 - Box
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(34,-22.4,33.3);
	glScalef(5, 7.5, 2);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/bar_table.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/bar_table_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Table2 - Bottom
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glRotatef(90,0,1,0);
	glTranslatef(-5,-21.5,34);
	glScalef(20, 8, 3);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/bar_root.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/bar_root_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Table1 Orange
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glRotatef(90,0,1,0);
	glTranslatef(-5,-19.23,34);
	glScalef(20.1, 1.3, 3.1);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool1 - Bottom
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-10,-30,-5);
	glScalef(3, 0.5f, 3);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/metal.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/metal_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool1 - Middle
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-10,-25,-5);
	glRotatef(90,1,0,0);
	glScalef(2, 2, 2);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/metal.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/metal_norm.bmp");
	//	----------------------------------------
	this->producing->drawCylinder(15,15,5);
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool1 - Top
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-10,-19,-5);
	glScalef(4, 1, 4);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool2 - Bottom
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(5,-30,-5);
	glScalef(3, 0.5f, 3);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/metal.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/metal_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool2 - Middle
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(5,-25,-5);
	glRotatef(90,1,0,0);
	glScalef(2, 2, 2);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/metal.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/metal_norm.bmp");
	//	----------------------------------------
	this->producing->drawCylinder(15,15,5);
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool2 - Top
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(5,-19,-5);
	glScalef(4, 1, 4);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool3 - Bottom
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(20,-30,-5);
	glScalef(3, 0.5f, 3);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/metal.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/metal_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool3 - Middle
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(20,-25,-5);
	glRotatef(90,1,0,0);
	glScalef(2, 2, 2);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/metal.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/metal_norm.bmp");
	//	----------------------------------------
	this->producing->drawCylinder(15,15,5);
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Barstool3 - Top
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(20,-19,-5);
	glScalef(4, 1, 4);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Disco-Ball
	this->texturing->bindVertexShader("v_discoball");
	this->texturing->bindFragmentShader("f_discoball");
	//	----------------------------------------
	glTranslatef(0,19,80);
	glRotatef(discoBallRotation, 0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_discoball", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_discoball", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_discoball", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_discoball", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_discoball", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_discoball", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_discoball", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_discoball", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderCubeTexture("f_discoball", "environmentMap");
	this->texturing->enableShaderTexture("f_discoball", "decal", "../Assets/discoball.bmp");
	this->texturing->enableShaderTexture("f_discoball", "pattern", "../Assets/discoball_pattern.bmp");
	this->texturing->updateShaderParam1f("f_discoball", "soundInput", br2);
	this->texturing->updateShaderParam1f("f_discoball", "time", time);
	//	----------------------------------------
	this->producing->drawSphere(7);
	//	----------------------------------------
	this->texturing->disableShaderCubeTexture("f_discoball", "environmentMap");
	this->texturing->disableShaderTexture("f_discoball", "decal");
	this->texturing->disableShaderTexture("f_discoball", "pattern");
	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Disco-Ball-Bar
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(0,27.5,80);
	glRotatef(90,1,0,0);
	glScalef(2,2,1);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/bar_table.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/bar_table_norm.bmp");
	//	----------------------------------------
	this->producing->drawCylinder(15,15,5);
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Picture Bar
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(52,-3,-8);
	glScalef(0.1, 11, 33);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/pic4.png");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/pic4_norm.png");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Bar End - Right
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(53,-9,33);
	glScalef(5, 21, 5);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/tallbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/tallbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Bar End - Left
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-33,-9,-49);
	glScalef(5, 21, 5);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/tallbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/tallbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Boxen - Right - Box
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-53,-9,30);
	glScalef(6, 21, 13);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Boxen - Right - Speaker
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-47,-9,30);
	glRotatef(90,0,1,0);
	glScalef(2, 3.5, 0.3 + bl1 + bl2);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djspeaker.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djspeaker_norm.bmp");
	//	----------------------------------------
	this->producing->drawSphere(5);
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Boxen box orange TOP
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(-47,11,30);
	glScalef(1, 1, 9);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Boxen box orange Bottom
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(-47,-28.8,30);
	glScalef(1, 1, 9);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Boxen - Left - Box
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-53,-9,130);
	glScalef(6, 21, 13);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Boxen - Left - Speaker
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-47,-9,130);
	glRotatef(90,0,1,0);
	glScalef(2, 3.5, 0.3 + bl1 + bl2);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djspeaker.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djspeaker_norm.bmp");
	//	----------------------------------------
	this->producing->drawSphere(5);
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Boxen box orange TOP
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(-47,11,130);
	glScalef(1, 1, 9);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Boxen box orange Bottom
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(-47,-28.8,130);
	glScalef(1, 1, 9);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Pult Box
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-35,-22,80);
	glScalef(5, 8, 25);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Pult Turntable - Left
	this->texturing->bindVertexShader("v_diffuse");
	this->texturing->bindFragmentShader("f_diffuse");
	//	----------------------------------------
	glTranslatef(-35,-13.2,90);
	glScalef(3.5, 0.8, 3.5);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_diffuse", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_diffuse", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_diffuse", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_diffuse", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_diffuse", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_diffuse", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_diffuse", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_diffuse", "tex", "../Assets/bar_table.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Pult Turntable - Right
	this->texturing->bindVertexShader("v_diffuse");
	this->texturing->bindFragmentShader("f_diffuse");
	//	----------------------------------------
	glTranslatef(-35,-13.2,70);
	glScalef(3.5, 0.8, 3.5);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_diffuse", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_diffuse", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_diffuse", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_diffuse", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_diffuse", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_diffuse", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_diffuse", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_diffuse", "tex", "../Assets/bar_table.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Pult Turntable - Middle
	this->texturing->bindVertexShader("v_diffuse");
	this->texturing->bindFragmentShader("f_diffuse");
	//	----------------------------------------
	glTranslatef(-35,-13.5,80);
	glScalef(2.8, 0.5, 6);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_diffuse", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_diffuse", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_diffuse", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_diffuse", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_diffuse", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_diffuse", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_diffuse", "materialAmbient", defaultMaterialAmbient2);
	this->texturing->enableShaderTexture("f_diffuse", "tex", "../Assets/bar_table.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	/*//	OBJECT: DJ Pult Turntable - Middle
	this->texturing->bindVertexShader("v_diffuse");
	this->texturing->bindFragmentShader("f_diffuse");
	//	----------------------------------------
	glTranslatef(-35,-13.5,80);
	glScalef(2.8, 0.5, 6);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_diffuse", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_diffuse", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_diffuse", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_diffuse", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_diffuse", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse", "tex", "../Assets/bar_table.bmp");
	//	----------------------------------------
	this->producing->drawCylinder(
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------*/

	//	OBJECT: DJ Orange box
	this->texturing->bindVertexShader("v_diffuse_constant");
	this->texturing->bindFragmentShader("f_diffuse_constant");
	//	----------------------------------------
	glTranslatef(-30,-22.1,80);
	glScalef(2, 8, 10);
	glRotatef(45,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_diffuse_constant", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_diffuse_constant", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->enableShaderTexture("f_diffuse_constant", "tex", glowTexture);
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_diffuse_constant", "tex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: DJ Picture
	this->texturing->bindVertexShader("v_animated_textures");
	this->texturing->bindFragmentShader("f_animated_textures");
	//	----------------------------------------
	glTranslatef(-55,-5,80);
	glScalef(0.1, 11, 33);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_animated_textures", "constantColor", white);
	this->texturing->updateShaderStateMatrix("v_animated_textures", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam1f("f_animated_textures", "time", time);
	this->texturing->updateShaderParam1f("f_animated_textures", "factor", br1);
	this->texturing->updateShaderParam1f("f_animated_textures", "factor2", br2);
	this->texturing->enableShaderTexture("f_animated_textures", "tex1", "../Assets/pic1.png");
	this->texturing->enableShaderTexture("f_animated_textures", "tex2", "../Assets/pic2.png");
	this->texturing->enableShaderTexture("f_animated_textures", "tex3", "../Assets/pic3.png");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_animated_textures", "tex1");
	this->texturing->disableShaderTexture("f_animated_textures", "tex2");
	this->texturing->disableShaderTexture("f_animated_textures", "tex3");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Bench - Backbox
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(56,-22,115);
	glScalef(3, 8, 65);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Bench - Backbox2
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(41.5,-22,182);
	glScalef(18, 8, 3);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Bench - seat box
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(48,-26,115);
	glScalef(5, 4, 65);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Bench - seat box2
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(33.5,-26,175);
	glScalef(10, 4, 5);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Seat box - 
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-31,-26,175);
	glScalef(4, 4, 4);
	glRotatef(35,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	----------------------------------------
	glLoadIdentity();	// NEXT OBJECT
	//	----------------------------------------

	//	OBJECT: Seat box2 -
	this->texturing->bindVertexShader("v_bump");
	this->texturing->bindFragmentShader("f_bump");
	//	----------------------------------------
	glTranslatef(-35,-26,160);
	glScalef(4, 4, 4);
	glRotatef(40,0,1,0);
	//	----------------------------------------
	this->texturing->updateShaderParam3f("v_bump", "lightPos", lightPosition_1);
	this->texturing->updateShaderParam3f("v_bump", "constantColor", lightColor_1);
	this->texturing->updateShaderParam3f("v_bump", "viewPos", view);
	this->texturing->updateShaderStateMatrix("v_bump", "m2w", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderStateMatrix("v_bump", "m2wIT", CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
	this->texturing->updateShaderStateMatrix("v_bump", "mvp", CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	this->texturing->updateShaderParam4f("f_bump", "globalAmbient", globalAmbientLight);
	this->texturing->updateShaderParam4f("f_bump", "materialAmbient", defaultMaterialAmbient);
	this->texturing->enableShaderTexture("f_bump", "colorTex", "../Assets/djbox.bmp");
	this->texturing->enableShaderTexture("f_bump", "normTex", "../Assets/djbox_norm.bmp");
	//	----------------------------------------
	this->producing->drawCube();
	//	----------------------------------------
	this->texturing->disableShaderTexture("f_bump", "colorTex");
	this->texturing->disableShaderTexture("f_bump", "normTex");

	//	+++++++++++++++++++++++++++++++++++
	this->texturing->disableShaderProfiles();
	//	SHADER OFF

	glLoadIdentity();
	// Light Object Visualization
	glTranslatef(lightPosition_1[0], lightPosition_1[1], lightPosition_1[2]);
	glScaled(0.5,0.5,0.5);
	//	----------------------------------------
	this->producing->drawCube();

	// 2D Interface
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	glOrtho(-15, 15, -10, 10, -20, 20);

	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
	this->drawFPS(13, 9.3, 0, "FPS: %4.2f", fps);
}

// Update animation
void Rendering::updateAnimation(void)
{
	if(isPlaying_1 || isPlaying_2 || isPlaying_3)
	{
		discoBallRotation += 0.5;

	}
	else
	{
		
	}

	if(isDiscoLightOn)
	{
		white[0] = 0.98;
		white[1] = 0.98;
		white[2] = 0.98;
		white[3] = 1;

		lightColor_1[0] = ((br2 * 10000000) * 11) / 10000000 + 0.1;
		lightColor_1[1] = (((br2 * 10000000) * 10) / 10000000) / 2 + 0.1;
		lightColor_1[2] = 0.1;

		globalAmbientLight[0] = 0.1;
		globalAmbientLight[1] = 0.1;
		globalAmbientLight[2] = 0.1;

		defaultMaterialAmbient[0] = 0.05;
		defaultMaterialAmbient[1] = 0.05;
		defaultMaterialAmbient[2] = 0.05;

		defaultMaterialAmbient2[0] = 0.02;
		defaultMaterialAmbient2[1] = 0.02;
		defaultMaterialAmbient2[2] = 0.02;

		reflection = br2;

		if(time % 20 == 0) random = rand() % 100;
		time += 1;
	}
	else
	{
		white[0] = 0.65;
		white[1] = 0.65;
		white[2] = 0.65;
		white[3] = 1;
		
		reflection = 0.3;

		lightColor_1[0] = 1;
		lightColor_1[1] = 0.98;
		lightColor_1[2] = 0.85;

		globalAmbientLight[0] = 0.9;
		globalAmbientLight[1] = 0.9;
		globalAmbientLight[2] = 0.9;

		defaultMaterialAmbient[0] = 0.8;
		defaultMaterialAmbient[1] = 0.8;
		defaultMaterialAmbient[2] = 0.8;

		defaultMaterialAmbient2[0] = 0.05;
		defaultMaterialAmbient2[1] = 0.05;
		defaultMaterialAmbient2[2] = 0.05;

		//glowTexture = "../Assets/orange.bmp";
	}

	if(!isCamCalibrated)
	{
		view[0] = camMotionStartingPoint[0];
		view[1] = camMotionStartingPoint[1];
		view[2] = camMotionStartingPoint[2];

		cameraRotateY = -39.0f;
		cameraRotateZ = 0;

		isCamCalibrated = TRUE;
	}

	if(isCamMotion)
	{
		view[2] += camZDirection == 1 ? 0.3 * camZDirection : 0.1 * camZDirection;

		if(camMotionStartingPoint[2] - view[2] < -50)
		{
			camZDirection = -1;
		}
		else if(camMotionStartingPoint[2] - view[2] > 0)
		{
			camZDirection = 1;
		}

		view[0] += camXDirection == 1 ? 0.06 * camXDirection : 0.09 * camXDirection;

		if(camMotionStartingPoint[0] - view[0] < -8)
		{
			camXDirection = -1;
		}
		else if(camMotionStartingPoint[0] - view[0] > 9)
		{
			camXDirection = 1;
		}

		view[1] += camYDirection == 1 ? 0.01 * camYDirection : 0.02 * camYDirection;

		if(camMotionStartingPoint[1] - view[1] < -5)
		{
			camYDirection = -1;
		}
		else if(camMotionStartingPoint[1] - view[1] > 5)
		{
			camYDirection = 1;
		}

		cameraRotateY += camRotYDirection == 1 ? 0.1 * camRotYDirection : 0.2 * camRotYDirection;

		if(cameraRotateY > -30)
		{
			camRotYDirection = -1;
		}
		else if(cameraRotateY < -58)
		{
			camRotYDirection = 1;
		}

		cameraRotateZ += camRotZDirection == 1 ? 0.05 * camRotZDirection : 0.065 * camRotZDirection;

		if(cameraRotateZ > 6.5)
		{
			camRotZDirection = -1;
		}
		else if(cameraRotateZ < -6.5)
		{
			camRotZDirection = 1;
		}
	}

	if(time == 100000) time = 0;
}

// Update sound
void Rendering::updateSound(void)
{
	// Sound control

	// Slot1
	if(play_1 && !isPlaying_1)
	{
		isPlaying_1 = TRUE;
		audio->playSound(1, 2);

		isPlaying_2 = FALSE;
		isPlaying_3 = FALSE;

		play_2 = FALSE;
		play_3 = FALSE;

		isDiscoLightOn = TRUE;
	}
	else if(!play_1 && isPlaying_1)
	{
		isPlaying_1 = FALSE;
		audio->stopSound(1);
	}

	// Slot2
	if(play_2 && !isPlaying_2)
	{
		isPlaying_2 = TRUE;
		audio->playSound(2, 2);

		isPlaying_1 = FALSE;
		isPlaying_3 = FALSE;

		play_1 = FALSE;
		play_3 = FALSE;

		isDiscoLightOn = TRUE;
	}
	else if(!play_2 && isPlaying_2)
	{
		isPlaying_2 = FALSE;
		audio->stopSound(2);
	}

	// Slot3
	if(play_3 && !isPlaying_3)
	{
		isPlaying_3 = TRUE;
		audio->playSound(3, 2);

		isPlaying_1 = FALSE;
		isPlaying_2 = FALSE;

		play_1 = FALSE;
		play_2 = FALSE;

		isDiscoLightOn = TRUE;
	}
	else if(!play_3 && isPlaying_3)
	{
		isPlaying_3 = FALSE;
		audio->stopSound(3);
	}

	// Sound action attributes
	if(isPlaying_1 || isPlaying_2 || isPlaying_3)
	{
		FMOD::Channel *channel = audio->getChannel(1);
	
		channel->getSpectrum( bassL, SPECLEN, 0, FMOD_DSP_FFT_WINDOW_BLACKMAN );
		for(int i = 0; i < sizeof(bassL); i++)
		{
			switch(i)
			{
				case 0:  br1 = bassL[i]; break;
				case 1:  br2 = bassL[i]; break;
				case 2:  br3 = bassL[i]; break;
				default: br4 = bassL[i]; break;
			}
		}
	
		channel->getSpectrum( bassR, SPECLEN, 1, FMOD_DSP_FFT_WINDOW_BLACKMAN );
		for(int i = 0; i < sizeof(bassR); i++)
		{
			switch(i)
			{
				case 0:  bl1 = bassR[i]; break;
				case 1:  bl2 = bassR[i]; break;
				case 2:  bl3 = bassR[i]; break;
				default: bl4 = bassR[i]; break;
			}
		}
	}
	else
	{
		br1 = 0;
		br2 = 0;
		br3 = 0;
		br4 = 0;

		bl1 = 0;
		bl2 = 0;
		bl3 = 0;
		bl4 = 0;
	}
}

// Calculate scene action
void Rendering::calculateAction()
{
	// Forward and backward
	if(keyArrowUp)
	{
		view[2] += movementFactor;
	}
	else if(keyArrowDown)
	{
		view[2] -= movementFactor;
	}

	// Sidewards
	if(keyArrowRight)
	{	
		view[0] -= movementFactor;
	}
	else if(keyArrowLeft)
	{
		view[0] += movementFactor;
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
		lightPosition_1[0] -= 0.4;
	}
	else if(action1Negative)
	{
		lightPosition_1[0] += 0.4;
	}

	if(action2Positive)
	{
		lightPosition_1[2] -= 0.4;
	}
	else if(action2Negative)
	{
		lightPosition_1[2] += 0.4;
	}

	if(action3Positive)
	{
		lightPosition_1[1] += 0.4;
	}
	else if(action3Negative)
	{
		lightPosition_1[1] -= 0.4;
	}
}

// Handle keyboard input
void GLFWCALL Rendering::handleKeyboardInput(int key, int action)
{
	if(!isCamMotion)
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
	}
	
	if(key == 65)
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
	else if(key == 68)
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
	else if(key == 87)
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
	else if(key == 83)
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
	else if(key == 82)
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
	else if(key == 70)
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
	else if(key == 80) // "P"
	{
		
	}
	else if(key == 48) // "0"
	{
		play_1 = FALSE;
		play_2 = FALSE;
		play_3 = FALSE;
		isDiscoLightOn = FALSE;
	}
	else if(key == 79) // "O"
	{
		isDiscoLightOn = TRUE;
	}
	else if(key == 76) // "L"
	{
		isDiscoLightOn = FALSE;
	}
	else if(key == 73) // "I"
	{
		isCamCalibrated = FALSE;
		isCamMotion = TRUE;
		lightPosition_1[0]  = 0;
		lightPosition_1[1]  = -20;
		lightPosition_1[2]  = 80;
	}
	else if(key == 75) // "K"
	{
		isCamCalibrated = FALSE;
		isCamMotion = FALSE;
		lightPosition_1[0]  = 0;
		lightPosition_1[1]  = -20;
		lightPosition_1[2]  = 80;
	}
	else if(key == 55) // "7"
	{
		play_3 = TRUE;
	}
	else if(key == 56) // "8"
	{
		play_2 = TRUE;
	}
	else if(key == 57) // "9"
	{
		play_1 = TRUE;
	}

	//std::cout << key << endl;
}

// Handle mouse wheel
void GLFWCALL Rendering::handleMouseWheel(int pos)
{
	if(!isCamMotion) view[1] = pos;
}

// Handle mouse input
void GLFWCALL Rendering::handleMouseInput(int button, int action)
{
	if(button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if(action == GLFW_PRESS && !isCamMotion)
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
		if(action == GLFW_PRESS && !isCamMotion)
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
		if(action == GLFW_PRESS && !isCamMotion)
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
	std::cout << "SoundVisualizer with OpenGL and CG\nby Gunnar Busch, Hadeer Khaled and Philipp Petzold" << endl;
	Rendering *engine = new Rendering();
	engine->initialize();
	return 0;
}