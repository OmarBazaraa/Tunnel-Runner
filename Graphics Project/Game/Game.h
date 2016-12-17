#pragma once

// STL Includes
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <time.h>
#include <fstream>
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
	GEM,
	ITEMS_COUNT
};

/*
	Defines several game states
*/
enum GameState {
	RUNNING,
	PAUSED,
	LOST
};


// Forward class declaration
class GameEngine;

// Scene constants
const int LANES_X_COUNT = 3;
const int LANES_Y_COUNT = 4;
const int LANES_Z_COUNT = 20;
const int BLOCKS_COUNT = 5;
const double LANE_WIDTH = 1.5f;
const double LANE_HEIGHT = 1.0f;
const double LANE_DEPTH = 1.5f;
const double SCENE_WIDTH = LANES_X_COUNT * LANE_WIDTH;
const double SCENE_HEIGHT = (LANES_Y_COUNT + 2) * LANE_HEIGHT;
const double SCENE_DEPTH = LANES_Z_COUNT * LANE_DEPTH;
const double CUBE_WIDTH = LANE_WIDTH;
const double CUBE_HEIGHT = LANE_HEIGHT;
const double CUBE_DEPTH = LANE_DEPTH;
const double GEM_SIZE = 0.6f;
const double SPHERE_RADIUS = 0.5f;
const double COIN_SIZE = 0.2;
const double RING_RADIUS = 0.5;
const double RING_DEPTH = 0.2;

// Camera constants
const double GRAVITY_POS = LANE_HEIGHT;
const double CHARACTER_OFFSET = LANE_DEPTH * 1.5;
const double CAMERA_SPEED_INIT = 4;
const double CAMERA_JUMP_OFFSET = LANE_HEIGHT;
const double CAMERA_ACCELERATION = 0.1f;
const glm::vec3 CAMERA_POSITION_INIT = glm::vec3(0.0f, GRAVITY_POS, 0.0f);

// Font constants
const double FONT_SIZE = 48.0f;
const double FONT_MARGIN = 25.0f;
const double FONT_SCALE = 1.0f;
const double MENU_FONT_SCALE = 0.6f;
const double TITLE_FONT_SCALE = 1.2f;
const glm::vec3 FONT_COLOR = glm::vec3(0.5, 0.8f, 0.2f);

// Menu constants
const string GAME_TITLE = "Tunnel Runner";
const string MENU_MSG = "Press Q to quit, R to replay";
const string GAME_OVER_MSG = "Game Over";
const string SCORE_LABEL = "Score: ";
const string TIME_LABEL = "Time: ";
const string FPS_LABEL = "FPS: ";
const string GEM_LABEL = "GEM (x2): ";

// Game constants
const int COIN_VALUE = 1;
const double DOUBLE_SCORE_DURATION = 10.0f;


/*
	Class containing all our game logic and drawing
*/
class Game
{
private:
	// Engines
	GameEngine* mEngine;
	ISoundEngine* mSoundEngine;

	// Models
	Model* mScene;
	Model* mSphere;
	Model* mCoin;
	Model* mCube;
	Model* mRing;
	Model* mGem;
	// Shaders
	Shader* mShader;
	Shader* mTextShader;
	// Camera
	Camera* mCamera;
	// Light sources
	LightSource* mLight;
	// Text renderers
	TextRenderer* mTextRenderer;
	double mGameTitleLabelWidth;
	double mGameOverMsgWidth;
	double mMenuMsgWidth;
	double mGemLabelWidth;
	
	// Scene variables
	deque<GameItem> mGrid[LANES_Y_COUNT][LANES_X_COUNT];
	GameItem mSceneBlocks[BLOCKS_COUNT][LANES_Z_COUNT][LANES_Y_COUNT][LANES_X_COUNT];
	GameItem mCharacterGrid[LANES_Y_COUNT][LANES_X_COUNT];
	GameItem mBorderLeft;
	GameItem mBorderRight;
	int mBlockId;
	int mGridIndexZ;
	int mBlockSliceIdx;

	// Game properties and variables
	GameState mGameState;
	int mScore;
	int mCoinValue;
	double mGameTime;
	double mDoubleScoreTime;
	bool mDoubleScore;
	bool mEscReleased = true;
	
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

	/* Renders the text of the game */
	void RenderText();

	/* Processes inputs from keyboard */
	void ProcessKeyInput();

	/* Processes inputs from mouse */
	void ProcessMouseInput();

	/* Detects the collision with the character and returns the colliding item */
	void DetectCollision(glm::vec3 characterPos/*, GameItem** grid*/);

	/* Gets a slice from the game grid at certain offset in Z lanes*/
	void GetSlice(int idx);

	/* Edits a slice after collision */
	void EditSlice(int idx);

	/* Executes actions according to different types of collision with game items */
	void Collide(GameItem item);

	/* Generates all of the scene items */
	void GenerateSceneItems();

	/* Clears the passed scene items from the grid */
	void ClearGrid();

	/* Resets the game initial values */
	void ResetGame();

	/* Initializes the game sounds and background music */
	void InitSounds();

	/* Initializes the game models */
	void InitModels();

	/* Initialezies the game blocks */
	void InitGameBlocks();

	/* Initializes the game shaders */
	void InitShaders();

	/* Initializes the game camera */
	void InitCamera();

	/* Initializes the game light sources */
	void InitLightSources();

	/* Initializes the game text renderers */
	void InitTextRenderers();
};