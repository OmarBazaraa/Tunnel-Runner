#include "GameEngine.h"

/* Constructs a new game engine with all related objects and components */
GameEngine::GameEngine(int width, int height, bool fullscreen) {
	this->mGame = NULL;
	this->mTimer = new FrameTimer();
	InitWindow(width, height, "", fullscreen);
}

/* Destructs the game engine and free resources */
GameEngine::~GameEngine() {
	// Destroy window
	glfwDestroyWindow(this->mWind);
	glfwTerminate();
}

/* Registers the game that the engine is going to run */
void GameEngine::RegisterGame(Game* game, const char* title) {
	this->mGame = game;
	glfwSetWindowTitle(this->mWind, title);
}

/* Starts the main game loop */
void GameEngine::Run() {
	if (this->mGame == NULL)
		return;

	this->mTimer->FramesCount = 0;
	this->mTimer->LastTime = this->mTimer->LastFrameTime = glfwGetTime();

	while (glfwWindowShouldClose(this->mWind) == GL_FALSE) {
		this->mTimer->ProcessFrameTime(glfwGetTime());
		this->ProcessInput();
		this->Update();
		this->Render();
	}
}

/* Receives user input and processes it for the next frame */
void GameEngine::ProcessInput() {
	glfwPollEvents();
	this->mGame->ProcessInput();
}

/* Updates the game for the next frame  */
void GameEngine::Update() {
	this->mGame->Update();
}

/* Clears the screen and draws the new frame */
void GameEngine::Render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	this->mGame->Render();
	glfwSwapBuffers(mWind);
}

/* Initializes the game window */
void GameEngine::InitWindow(int width, int height, const char* title, bool fullscreen) {
	// Init GLFW
	if (!glfwInit()) {
		cout << "Failed to initialize GLFW" << endl;
		return;
	}

	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWmonitor* monitor = fullscreen ? glfwGetPrimaryMonitor() : NULL;
	mWind = glfwCreateWindow(width, height, title, monitor, NULL);
	if (mWind == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(mWind);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;

	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLFW" << std::endl;
		glfwTerminate();
		return;
	}

	// Define viewport's dimensions as Window's size
	glViewport(0, 0, width, height);

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Setup OpenGL options
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);		// Accept fragment if it closer to the camera than the former one
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Specify the polygon drawing mode: fill, line or point
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Set the color used when clearing the screen
	glClearColor(0.1f, 0.4f, 0.6f, 1.0f);

	// Disable the mouse cursor
	glfwSetInputMode(this->mWind, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
