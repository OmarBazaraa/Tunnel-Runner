#pragma once

// GL Includes
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Other includes
#include "../Game/GameEngine.h"
#include "../Components/Shader.h"
#include "../Components/Texture.h"
#include "../Components/Model.h"
#include "../Components/Camera.h"
#include "../Components/LightSource.h"

// Forward class declaration
class GameEngine;

// Scene constants
const int HORIZONTAL_LANES_COUNT = 5;
const double LANE_SIZE = 1.5f;
const double SCENE_WIDTH = HORIZONTAL_LANES_COUNT * LANE_SIZE;
const double SCENE_HEIGHT = 4.0f;
const double SCENE_DEPTH = 40.0f;
const double CUBE_SIZE = LANE_SIZE;
const double CUBE_HEIGHT = 1.0;
const double SPHERE_RADIUS = 0.5f;
const double COIN_SIZE = 0.2;
const double RING_RADIUS = 1.0;
const double RING_DEPTH = 0.2;

// Camera constants
const glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 1.0f, 0.0f);
const double JUMP_OFFSET = 1.5f;


/*
	Class containing all our game logic and drawing
*/
class Game
{
private:
	// Game engine
	GameEngine* mEngine;

	// Shaders
	Shader* mShader;

	// Models
	Model* mScene;
	Model* mSphere;
	Model* mCoin;
	Model* mCube;
	Model* mRing;

	// Models info
	glm::mat4 mCoinsModelMatrices[10];
	glm::mat4 mCubesModelMatrices[3];
	glm::mat4 mRingsModelMatrices[2];

	// Light sources
	LightSource* mLight;

	// Camera
	Camera* mCamera;
	

public:
	/* Constructs a new game with all related objects and components */
	Game(GameEngine* engine, const char* title);

	/* Destructs the game and free resources */
	~Game();

	/* Receives user input and processes it for the next frame */
	void ProcessInput();

	/* Updates objects' information needed to apply effects to the next frame  */
	void Update();

	/* Renders the new frame */
	void Render();

private:
	/* Processes inputs from keyboard */
	void ProcessKeyInput();

	/* Processes inputs from mouse */
	void ProcessMouseInput();

	/* Initializes the game camera */
	void InitCamera();

	/* Initializes the game shaders */
	void InitShaders();

	/* Initializes the game models */
	void InitModels();

	/* Initializes the game light sources */
	void InitLightSources();
};
