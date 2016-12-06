#include "Game.h"

/* Constructs a new game with all related objects and components */
Game::Game(GameEngine* engine, const char* title) {
	this->mEngine = engine;
	this->mEngine->RegisterGame(this, title);

	InitCamera();
	InitShaders();
	InitModels();
	InitLightSources();
}

/* Destructs the game and free resources */
Game::~Game() {
	// Destroy camera
	delete this->mCamera;

	// Destroy shaders
	delete this->mShader;

	// Destroy models
	delete this->mScene;
	delete this->mSphere;
	delete this->mCube;
	delete this->mCoin;
	delete this->mRing;

	// Destroy light sources
	delete this->mLight;
}

/* Receives user input and processes it for the next frame */
void Game::ProcessInput() {
	this->ProcessKeyInput();
	this->ProcessMouseInput();
}

/* Updates objects' information needed to apply effects on the next frame  */
void Game::Update() {
	// Update light sources
	this->mLight->Position = this->mCamera->GetPosition();
	this->mLight->Position -= this->mCamera->GetFront();

	//
	// Update models
	//

	// Coins
	for (unsigned int i = 0; i < 5; ++i) {
		mCoinsModelMatrices[i] = glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, i * -1.0f - 2.0f));
		mCoinsModelMatrices[i] = glm::scale(mCoinsModelMatrices[i], glm::vec3(0.2f, 0.2f, 0.2f));
		mCoinsModelMatrices[i] = glm::rotate(mCoinsModelMatrices[i], (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	for (unsigned int i = 5; i < 10; ++i) {
		mCoinsModelMatrices[i] = glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, i * -1.0f - 5.0f));
		mCoinsModelMatrices[i] = glm::scale(mCoinsModelMatrices[i], glm::vec3(0.2f, 0.2f, 0.2f));
		mCoinsModelMatrices[i] = glm::rotate(mCoinsModelMatrices[i], (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	// ---------------------------
}

/* Renders the new frame */
void Game::Render() {
	this->mShader->Use();
	this->mCamera->ApplyEffects(*mShader);
	this->mLight->ApplyEffects(*mShader);

	this->mScene->Draw(*this->mShader);
	this->mSphere->Draw(*this->mShader);

	for (unsigned int i = 0; i < 10; ++i) {
		this->mCoin->ModelMatrix = this->mCoinsModelMatrices[i];
		this->mCoin->Draw(*this->mShader);
	}

	for (unsigned int i = 0; i < 2; ++i) {
		this->mCube->ModelMatrix = this->mCubesModelMatrices[i];
		this->mCube->Draw(*this->mShader);
	}

	for (unsigned int i = 0; i < 2; ++i) {
		this->mRing->ModelMatrix = this->mRingsModelMatrices[i];
		this->mRing->Draw(*this->mShader);
	}
}

/* Processes inputs from keyboard */
void Game::ProcessKeyInput() {
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->mEngine->mWind, GL_TRUE);

	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_W) == GLFW_PRESS)
		this->mCamera->Move(FORWARD, this->mEngine->mTimer->ElapsedFramesTime);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_S) == GLFW_PRESS)
		this->mCamera->Move(BACKWARD, this->mEngine->mTimer->ElapsedFramesTime);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_D) == GLFW_PRESS)
		this->mCamera->Move(RIGHT, this->mEngine->mTimer->ElapsedFramesTime);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_A) == GLFW_PRESS)
		this->mCamera->Move(LEFT, this->mEngine->mTimer->ElapsedFramesTime);
}

/* Processes inputs from mouse */
void Game::ProcessMouseInput() {
	double xpos, ypos;
	glfwGetCursorPos(this->mEngine->mWind, &xpos, &ypos);
	this->mCamera->ChangeDirection(xpos, ypos, this->mEngine->mTimer->ElapsedFramesTime, true);
}

/* Initializes the game camera */
void Game::InitCamera() {
	int w, h;
	glfwGetWindowSize(this->mEngine->mWind, &w, &h);
	mCamera = new Camera(CAMERA_POSITION, (double)w / (double)h);
}

/* Initializes the game shaders */
void Game::InitShaders() {
	this->mShader = new Shader("Shaders/lighting_vertex.shader", "Shaders/lighting_fragment.shader");
}

/* Initializes the game models */
void Game::InitModels() {
	this->mScene = new Model("Models/scene/scene.obj");
	this->mScene->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.5f, -20.0f));
	this->mScene->ModelMatrix = glm::scale(this->mScene->ModelMatrix, glm::vec3(6.0f, 4.0f, 40.0f));

	this->mSphere = new Model("Models/sphere/sphere.obj");
	this->mSphere->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0f, -5.0f));
	this->mSphere->ModelMatrix = glm::scale(this->mSphere->ModelMatrix, glm::vec3(0.5f, 0.5f, 0.5f));

	this->mCoin = new Model("Models/coin/coin.obj");

	this->mCube = new Model("Models/cube/cube.obj");
	this->mCubesModelMatrices[0] = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 0.0f, -5.0f));
	this->mCubesModelMatrices[1] = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, -10.0f));
	this->mCubesModelMatrices[1] = glm::scale(mCubesModelMatrices[1], glm::vec3(2.0f, 1.0f, 1.0f));
	
	this->mRing = new Model("Models/ring/ring.obj");
	this->mRingsModelMatrices[0] = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, -10.0f));
	this->mRingsModelMatrices[0] = glm::scale(mRingsModelMatrices[0], glm::vec3(1.2f, 1.2f, 0.5f));
	this->mRingsModelMatrices[1] = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, -15.0f));
	this->mRingsModelMatrices[1] = glm::scale(mRingsModelMatrices[1], glm::vec3(1.2f, 1.2f, 0.5f));
}

/* Initializes the game light sources */
void Game::InitLightSources() {
	this->mLight = new LightSource(0.0f, 0.0f, 0.0f);
	this->mLight->AmbientColor *= 0.3f;
	this->mLight->DiffuseColor *= 0.9f;
	this->mLight->SpecularColor *= 1.0f;
	this->mLight->AttenuationConstant = 0.5f;
	this->mLight->AttenuationLinear = 0.1f;
	this->mLight->AttenuationQuadratic = 0.032f;
}