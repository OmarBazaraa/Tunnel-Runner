#include "Game.h"

/* Constructs a new game with all related objects and components */
Game::Game(GameEngine* engine, const char* title) {
	this->mEngine = engine;
	this->mEngine->RegisterGame(this, title);

	InitCamera();
	InitShaders();
	InitModels();
	InitLightSources();
	InitGameBlocks();
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

	// Update models


}

/* Renders the new frame */
void Game::Render() {
	this->mShader->Use();
	this->mCamera->ApplyEffects(*mShader);
	this->mLight->ApplyEffects(*mShader);

	// Move the scene with the camera to make it feel endless
	glm::vec3 cameraPosition = this->mCamera->GetPosition();
	this->mScene->ModelMatrix = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.5 * SCENE_HEIGHT, cameraPosition.z - 0.4 * SCENE_DEPTH - CAMERA_POSITION.z));
	this->mScene->ModelMatrix = glm::scale(this->mScene->ModelMatrix, glm::vec3(SCENE_WIDTH, SCENE_HEIGHT, SCENE_DEPTH));

	//Draw the scene
	this->mScene->Draw(*this->mShader);

	//Draw the block
	for (int z = 0; z < LANES_Z_COUNT; ++z) {
		for (int y = 0; y < LANES_Y_COUNT; ++y) {
			for (int x = 0; x < LANES_X_COUNT; ++x) {
				switch (this->mGrid[mBlockId][z][y][x])
				{
				case BLOCK:
					this->mCube->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - 2) * LANE_SIZE, 0.5f * CUBE_HEIGHT + y * LANE_SIZE, -z * LANE_SIZE));
					this->mCube->ModelMatrix = glm::scale(this->mCube->ModelMatrix, glm::vec3(CUBE_SIZE, CUBE_HEIGHT, CUBE_SIZE));
					this->mCube->Draw(*this->mShader);
					break;
				case COIN:
					this->mCoin->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - 2) * LANE_SIZE, COIN_SIZE + y * LANE_SIZE, -z * LANE_SIZE));
					this->mCoin->ModelMatrix = glm::scale(this->mCoin->ModelMatrix, glm::vec3(COIN_SIZE, COIN_SIZE, COIN_SIZE));
					this->mCoin->ModelMatrix = glm::rotate(this->mCoin->ModelMatrix, (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
					this->mCoin->Draw(*this->mShader);
					break;
				case SPHERE:
					this->mSphere->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - 2) * LANE_SIZE, SPHERE_RADIUS + y * LANE_SIZE, -z * LANE_SIZE));
					this->mSphere->ModelMatrix = glm::scale(this->mSphere->ModelMatrix, glm::vec3(SPHERE_RADIUS, SPHERE_RADIUS, SPHERE_RADIUS));
					this->mSphere->Draw(*this->mShader);
					break;
				case RING:
					this->mRing->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - 2) * LANE_SIZE, RING_RADIUS + y * LANE_SIZE, -z * LANE_SIZE));
					this->mRing->ModelMatrix = glm::scale(this->mRing->ModelMatrix, glm::vec3(RING_RADIUS, RING_RADIUS, RING_DEPTH));
					this->mRing->Draw(*this->mShader);
					break;
				default:
					break;
				}
			}
		}
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
		this->mCamera->Move(LEFT, this->mEngine->mTimer->ElapsedFramesTime);

	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_W) == GLFW_PRESS)
		this->mCamera->StartAnimation(MOVE_FORWARD, LANE_SIZE);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_S) == GLFW_PRESS)
		this->mCamera->StartAnimation(MOVE_BACKWARD, LANE_SIZE);*/

	/* Moves the camera forward every frame by default */
	this->mCamera->SetMoveSpeed(mCameraSpeed);
	this->mCamera->StartAnimation(MOVE_FORWARD, LANE_SIZE);
	mCameraSpeed += CAMERA_ACCELERATION;

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
	this->mCamera->ChangeDirection(xpos, ypos, this->mEngine->mTimer->ElapsedFramesTime);
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
	// Load scene model and correctly scale it
	this->mScene = new Model("Models/scene/scene.obj");
	this->mScene->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5 * SCENE_HEIGHT, -0.5 * SCENE_DEPTH));
	this->mScene->ModelMatrix = glm::scale(this->mScene->ModelMatrix, glm::vec3(SCENE_WIDTH, SCENE_HEIGHT, SCENE_DEPTH));

	// Load game items
	this->mCube = new Model("Models/cube/cube.obj");
	this->mCoin = new Model("Models/coin/coin.obj");
	this->mSphere = new Model("Models/sphere/sphere.obj");
	this->mRing = new Model("Models/ring/ring.obj");

	// Randomlly gets the game item block
	srand(time(NULL));
	this->mBlockId = rand() % BLOCKS_COUNT;
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

/* Initialezies the game blocks */
void Game::InitGameBlocks() {
	for (int b = 0; b < BLOCKS_COUNT; b++) {
		for (int z = 0; z < LANES_Z_COUNT; ++z) {
			for (int y = 0; y < LANES_Y_COUNT; ++y) {
				for (int x = 0; x < LANES_X_COUNT; ++x) {
					mGrid[b][z][y][x] = (GameItem)(b);
				}
			}
		}
	}
}
