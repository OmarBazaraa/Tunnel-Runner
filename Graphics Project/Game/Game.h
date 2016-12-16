#pragma once

// STL Includes
#include <string>
#include <vector>
#include <queue>
#include <time.h>
using namespace std;

// GL Includes
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Sound Library Include
#include <irrklang/irrKlang.h>
using namespace irrklang;

// Other includes
#include "../Game/GameEngine.h"
#include "../Components/Shader.h"
#include "../Components/Texture.h"
#include "../Components/Model.h"
#include "../Components/Camera.h"
#include "../Components/LightSource.h"
#include "../Components/TextRenderer.h"


/*
	Defines several game items used in filling the game grid
*/
enum GameItem {
	EMPTY,
	BLOCK,
	COIN,
	SPHERE,
	RING,
	ITEMS_COUNT
};

// Forward class declaration
class GameEngine;

// Scene constants
const int LANES_X_COUNT = 5;
const int LANES_Y_COUNT = 3;
const int LANES_Z_COUNT = 20;
const int BLOCKS_COUNT = 5;
const double LANE_SIZE = 1.5f;
const double SCENE_WIDTH = LANES_X_COUNT * LANE_SIZE;
const double SCENE_HEIGHT = LANES_Y_COUNT * LANE_SIZE;
const double SCENE_DEPTH = LANES_Z_COUNT * LANE_SIZE;
const double CUBE_SIZE = LANE_SIZE;
const double CUBE_HEIGHT = 1.0;
const double SPHERE_RADIUS = 0.5f;
const double COIN_SIZE = 0.2;
const double RING_RADIUS = 1.0;
const double RING_DEPTH = 0.2;

// Font constants
const double FONT_SIZE = 48.0f;
const double FONT_MARGIN = 25.0f;
const glm::vec3 FONT_COLOR = glm::vec3(0.5, 0.8f, 0.2f);

// Menu constants
const string MENU_MSG = "Press ENTER to quit, ESQ to resume";

// Camera constants
const glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 1.0f, 0.0f);
const double CAMERA_ACCELERATION = 0.01;
const double CAMERA_SPEED_MAX = 15;
const double JUMP_OFFSET = 1.5f;


/*
	Class containing all our game logic and drawing
*/
class Game
{
private:
	// Game engine
	GameEngine* mEngine;

	// Sound engine
	ISoundEngine* mSoundEngine;

	// Shaders
	Shader* mShader;
	Shader* mTextShader;

	// Models
	Model* mScene;
	Model* mSphere;
	Model* mCoin;
	Model* mCube;
	Model* mRing;

	// Light sources
	LightSource* mLight;

	// Text renderers
	TextRenderer* mTextRenderer;

	// Game properties and variables
	GameItem mSceneBlocks[BLOCKS_COUNT][LANES_Z_COUNT][LANES_Y_COUNT][LANES_X_COUNT];
	queue<GameItem> mGrid[LANES_Y_COUNT][LANES_X_COUNT];
	int mBlockId;
	int mScore = 0;

	// Menu variables
	bool mIsPaused = false;
	bool mEscReleased = true;

	// Camera
	double mCameraSpeed = 4;
	int mZGridIndex = 0, mBlockSliceIdx = 0;
	Camera* mCamera;
	GameItem mColliding;
	
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

	/* Processes inputs for the game menu while the game is paused */
	void ProcessMenuInput();

	/* Processes inputs from mouse */
	void ProcessMouseInput();

	/* Initializes the game sounds and background music */
	void InitSounds();

	/* Initializes the game camera */
	void InitCamera();

	/* Initializes the game shaders */
	void InitShaders();

	/* Initializes the game models */
	void InitModels();

	/* Initializes the game light sources */
	void InitLightSources();

	/* Initialezies the game blocks */
	void InitGameBlocks();

	/* Initializes the game text renderers */
	void InitTextRenderers();

	/* Generates all of the scene items */
	void GenerateSceneItems();

	/* Clears the grid queue from extra scenes that will not be drawn */
	void ClearGrid();
};