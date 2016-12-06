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
	// Update camera
	this->mCamera->Update(this->mEngine->mTimer->ElapsedFramesTime);

	// Update light sources
	this->mLight->Position = this->mCamera->GetPosition();
	this->mLight->Position -= this->mCamera->GetFront();

	//
	// Update models
	//

	// Coins
	for (unsigned int i = 0; i < 5; ++i) {
		mCoinsModelMatrices[i] = glm::translate(glm::mat4(1.0f), glm::vec3(-LANE_SIZE, COIN_SIZE, i * -LANE_SIZE - LANE_SIZE * 2));
		mCoinsModelMatrices[i] = glm::scale(mCoinsModelMatrices[i], glm::vec3(COIN_SIZE, COIN_SIZE, COIN_SIZE));
		mCoinsModelMatrices[i] = glm::rotate(mCoinsModelMatrices[i], (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	for (unsigned int i = 5; i < 10; ++i) {
		mCoinsModelMatrices[i] = glm::translate(glm::mat4(1.0f), glm::vec3(LANE_SIZE, COIN_SIZE, i * -LANE_SIZE - LANE_SIZE * 4));
		mCoinsModelMatrices[i] = glm::scale(mCoinsModelMatrices[i], glm::vec3(COIN_SIZE, COIN_SIZE, COIN_SIZE));
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

	for (unsigned int i = 0; i < 3; ++i) {
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

	/*if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_W) == GLFW_PRESS)
		this->mCamera->Move(FORWARD, this->mEngine->mTimer->ElapsedFramesTime);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_S) == GLFW_PRESS)
		this->mCamera->Move(BACKWARD, this->mEngine->mTimer->ElapsedFramesTime);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_D) == GLFW_PRESS)
		this->mCamera->Move(RIGHT, this->mEngine->mTimer->ElapsedFramesTime);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_A) == GLFW_PRESS)
		this->mCamera->Move(LEFT, this->mEngine->mTimer->ElapsedFramesTime);*/

	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_A) == GLFW_PRESS)
		this->mCamera->StartAnimation(MOVE_LEFT, LANE_SIZE);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_D) == GLFW_PRESS)
		this->mCamera->StartAnimation(MOVE_RIGHT, LANE_SIZE);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_SPACE) == GLFW_PRESS)
		this->mCamera->StartAnimation(JUMP, JUMP_OFFSET);
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
	this->mScene->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5 * SCENE_HEIGHT, -0.5 * SCENE_DEPTH));
	this->mScene->ModelMatrix = glm::scale(this->mScene->ModelMatrix, glm::vec3(SCENE_WIDTH, SCENE_HEIGHT, SCENE_DEPTH));

	this->mSphere = new Model("Models/sphere/sphere.obj");
	this->mSphere->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, SPHERE_RADIUS, -5 * LANE_SIZE));
	this->mSphere->ModelMatrix = glm::scale(this->mSphere->ModelMatrix, glm::vec3(SPHERE_RADIUS, SPHERE_RADIUS, SPHERE_RADIUS));

	this->mCoin = new Model("Models/coin/coin.obj");

	this->mCube = new Model("Models/cube/cube.obj");
	this->mCubesModelMatrices[0] = glm::translate(glm::mat4(1.0f), glm::vec3(2 * LANE_SIZE, 0.5 * CUBE_HEIGHT, -5 * LANE_SIZE));
	this->mCubesModelMatrices[0] = glm::scale(this->mCubesModelMatrices[0], glm::vec3(CUBE_SIZE, CUBE_HEIGHT, CUBE_SIZE));
	this->mCubesModelMatrices[1] = glm::translate(glm::mat4(1.0f), glm::vec3(-1 * LANE_SIZE, 0.5 * CUBE_HEIGHT, -9 * LANE_SIZE));
	this->mCubesModelMatrices[1] = glm::scale(this->mCubesModelMatrices[1], glm::vec3(CUBE_SIZE, CUBE_HEIGHT, CUBE_SIZE));
	this->mCubesModelMatrices[2] = glm::translate(glm::mat4(1.0f), glm::vec3(-2 * LANE_SIZE, 0.5 * CUBE_HEIGHT, -9 * LANE_SIZE));
	this->mCubesModelMatrices[2] = glm::scale(this->mCubesModelMatrices[2], glm::vec3(CUBE_SIZE, CUBE_HEIGHT, CUBE_SIZE));
	
	this->mRing = new Model("Models/ring/ring.obj");
	this->mRingsModelMatrices[0] = glm::translate(glm::mat4(1.0f), glm::vec3(2 * LANE_SIZE, RING_RADIUS + LANE_SIZE, -5 * LANE_SIZE));
	this->mRingsModelMatrices[0] = glm::scale(this->mRingsModelMatrices[0], glm::vec3(RING_RADIUS, RING_RADIUS, RING_DEPTH));
	this->mRingsModelMatrices[1] = glm::translate(glm::mat4(1.0f), glm::vec3(-2 * LANE_SIZE, RING_RADIUS + LANE_SIZE, -9 * LANE_SIZE));
	this->mRingsModelMatrices[1] = glm::scale(this->mRingsModelMatrices[1], glm::vec3(RING_RADIUS, RING_RADIUS, RING_DEPTH));
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