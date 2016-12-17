#include "Game.h"

/* Constructs a new game with all related objects and components */
Game::Game(GameEngine* engine, const char* title) {
	this->mEngine = engine;
	this->mEngine->RegisterGame(this, title);

	InitSounds();
	InitCamera();
	InitShaders();
	InitModels();
	InitGameBlocks();
	InitLightSources();
	InitTextRenderers();

	ResetGame();
}

/* Destructs the game and free resources */
Game::~Game() {
	// Destroy engines
	delete this->mSoundEngine;

	// Destroy camera
	delete this->mCamera;

	// Destroy shaders
	delete this->mShader;
	delete this->mTextShader;

	// Destroy models
	delete this->mScene;
	delete this->mSphere;
	delete this->mCube;
	delete this->mCoin;
	delete this->mRing;

	// Destroy light sources
	delete this->mLight;

	// Destroy text renderers
	delete this->mTextRenderer;
}

/* Receives user input and processes it for the next frame */
void Game::ProcessInput() {
	this->ProcessKeyInput();
	//this->ProcessMouseInput();
}

/* Updates objects' information needed to apply effects on the next frame  */
void Game::Update() {
	// Return if game is not running
	if (this->mGameState != RUNNING)
		return;

	// Update camera to give animation effects
	this->mCamera->MoveStep(FORWARD, LANE_DEPTH);
	this->mCamera->Update(this->mEngine->mTimer->ElapsedFramesTime);

	// Update light sources
	/*
	this->mColorValue += 0.005f;
	if (this->mColorValue > 2.5f) {
		this->mColorValue = 0;
	}
	double r = abs(sin(this->mColorValue) / 2.0f) + 0.5f;
	double g = abs(cos(this->mColorValue) / 2.0f) + 0.5f;
	double b = abs(tan(this->mColorValue) / 2.0f) + 0.5f;
	this->mLight->SpecularColor = glm::vec3(r, g, b);
	this->mLight->DiffuseColor = this->mLight->SpecularColor * 0.9f;
	this->mLight->AmbientColor = this->mLight->SpecularColor * 0.3f;
	*/
	this->mLight->Position = this->mCamera->GetPosition();
	this->mLight->Position -= this->mCamera->GetFront();

	// Move the scene with the camera to make it feel infinite
	this->mScene->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5 * SCENE_HEIGHT, this->mCamera->GetPosition().z - CAMERA_POSITION_INIT.z - 0.5 * SCENE_DEPTH));
	this->mScene->ModelMatrix = glm::scale(this->mScene->ModelMatrix, glm::vec3(SCENE_WIDTH, SCENE_HEIGHT, SCENE_DEPTH));

	// Detect collisions
	this->DetectCollision(this->mCamera->GetPosition() - CAMERA_POSITION_INIT);
	
	// Update the scene items to be rendered
	this->GenerateSceneItems();
}

/* Renders the new frame */
void Game::Render() {
	// Apply effect to the shader
	this->mShader->Use();
	this->mCamera->ApplyEffects(*mShader);
	this->mLight->ApplyEffects(*mShader);

	// Draw the scene
	this->mScene->Draw(*this->mShader);

	// Draw game items
	for (int y = 0; y < LANES_Y_COUNT; ++y) {
		for (int x = 0; x < LANES_X_COUNT; ++x) {
			for (int z = 0; z < this->mGrid[y][x].size(); ++z) {
				GameItem cell = this->mGrid[y][x].front();
				this->mGrid[y][x].pop();
				this->mGrid[y][x].push(cell);

				switch (cell)
				{
				case BLOCK:
					this->mCube->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - (int)(LANE_WIDTH + 1) / 2) * LANE_WIDTH, 0.5f * CUBE_HEIGHT + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mCube->ModelMatrix = glm::scale(this->mCube->ModelMatrix, glm::vec3(CUBE_SIZE, CUBE_HEIGHT, CUBE_DEPTH));
					this->mCube->Draw(*this->mShader);
					break;
				case COIN:
					this->mCoin->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - (int)(LANE_WIDTH + 1) / 2) * LANE_WIDTH, COIN_SIZE + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mCoin->ModelMatrix = glm::scale(this->mCoin->ModelMatrix, glm::vec3(COIN_SIZE, COIN_SIZE, COIN_SIZE));
					this->mCoin->ModelMatrix = glm::rotate(this->mCoin->ModelMatrix, (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
					this->mCoin->Draw(*this->mShader);
					break;
				case SPHERE:
					this->mSphere->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - (int)(LANE_WIDTH + 1) / 2) * LANE_WIDTH, SPHERE_RADIUS + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mSphere->ModelMatrix = glm::scale(this->mSphere->ModelMatrix, glm::vec3(SPHERE_RADIUS, SPHERE_RADIUS, SPHERE_RADIUS));
					this->mSphere->Draw(*this->mShader);
					break;
				case RING:
					this->mRing->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - (int)(LANE_WIDTH + 1) / 2) * LANE_WIDTH, RING_RADIUS + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mRing->ModelMatrix = glm::scale(this->mRing->ModelMatrix, glm::vec3(RING_RADIUS, RING_RADIUS, RING_DEPTH));
					this->mRing->Draw(*this->mShader);
					break;
				}
			}
		}
	}

	// Draw game information
	this->RenderText();
}

/* Renders the text of the game */
void Game::RenderText() {
	int w, h, x, y;
	stringstream ss;
	glfwGetWindowSize(this->mEngine->mWind, &w, &h);

	// Score
	ss << SCORE_LABEL << this->mScore;
	x = FONT_MARGIN;
	y = h - FONT_MARGIN - FONT_SIZE;
	this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);

	// Time
	ss.clear();
	ss.str("");
	ss << TIME_LABEL << (int)this->mEngine->mTimer->CurrentFrameTime - this->mGameStartTime;
	x = w - FONT_MARGIN * 8;
	y = h - FONT_MARGIN - FONT_SIZE;
	this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);

	// FPS
	ss.clear();
	ss.str("");
	ss << FPS_LABEL << this->mEngine->mTimer->FPS;
	x = FONT_MARGIN;
	y = FONT_MARGIN;
	this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);

	// Game over label
	if (this->mGameState == LOST) {
		int textWidth = this->mTextRenderer->GetTextWidth(GAME_OVER_MSG, MENU_FONT_SCALE);
		x = (w - textWidth) / 2;
		y = h / 2 - FONT_SIZE * MENU_FONT_SCALE + FONT_MARGIN * 2;
		this->mTextRenderer->RenderText(*this->mTextShader, GAME_OVER_MSG, x, y, MENU_FONT_SCALE, FONT_COLOR);
	}

	// Quit and reply message
	if (this->mGameState != RUNNING) {
		int textWidth = this->mTextRenderer->GetTextWidth(MENU_MSG, MENU_FONT_SCALE);
		x = (w - textWidth) / 2;
		y = h / 2 - FONT_SIZE * MENU_FONT_SCALE;
		this->mTextRenderer->RenderText(*this->mTextShader, MENU_MSG, x, y, MENU_FONT_SCALE, FONT_COLOR);
	}
}

/* Processes inputs from keyboard */
void Game::ProcessKeyInput() {
	// Pause/Resume
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_ESCAPE) == GLFW_PRESS && this->mEscReleased && this->mGameState != LOST) {
		this->mGameState = (this->mGameState == PAUSED) ? RUNNING : PAUSED;
		this->mEscReleased = false;
	}

	// Detect when ESC is released
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_ESCAPE) == GLFW_RELEASE)
		this->mEscReleased = true;

	// Quit
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->mEngine->mWind, GL_TRUE);

	// Replay
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_R) == GLFW_PRESS)
		this->ResetGame();

	// Return if game is not running
	if (this->mGameState != RUNNING)
		return;

	// Game control
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_A) == GLFW_PRESS && mBorderLeft != BLOCK)
		this->mCamera->MoveStep(LEFT, LANE_WIDTH);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_D) == GLFW_PRESS && mBorderRight != BLOCK)
		this->mCamera->MoveStep(RIGHT, LANE_WIDTH);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_SPACE) == GLFW_PRESS)
		this->mCamera->Jump(CAMERA_JUMP_OFFSET);
}

/* Processes inputs from mouse */
void Game::ProcessMouseInput() {
	// Return if game is not running
	if (this->mGameState != RUNNING)
		return;

	double xpos, ypos;
	glfwGetCursorPos(this->mEngine->mWind, &xpos, &ypos);
	this->mCamera->ChangeDirection(xpos, ypos, this->mEngine->mTimer->ElapsedFramesTime);
}

/* Detects the collision with the character and returns the colliding item */
void Game::DetectCollision(glm::vec3 characterPos) {
	this->mBorderLeft = this->mBorderRight = EMPTY;

	int x = (characterPos.x) / LANE_WIDTH + (LANES_X_COUNT - 1) / 2;
	int y = (characterPos.y) / LANE_HEIGHT;

	if (x == 0)
		this->mBorderLeft = BLOCK;
	if (x + 1 == LANES_X_COUNT)
		this->mBorderRight = BLOCK;

	if (characterPos.x - int(characterPos.x / LANE_WIDTH) * LANE_WIDTH)
		++x;
	if (characterPos.y - int(characterPos.y / LANE_HEIGHT) * LANE_HEIGHT)
		++y;

	// Check if out of range
	if (y < 0 || y >= LANES_Y_COUNT || x < 0 || x >= LANES_X_COUNT || mGrid[y][x].empty()) {
		return;
	}

	/*for (int y = 0; y < LANES_Y_COUNT; y++) {
		for (int x = 0; x < LANES_X_COUNT; x++) {
			GameItem front = this->mGrid[y][x].front();
			this->mGrid[y][x].pop();
			this->mGrid[y][x].push(front);
		}
	}*/

	// Set left and right borders
	this->mBorderLeft = (x <= 0) ? BLOCK : this->mGrid[y][x - 1].front();
	this->mBorderRight = (x + 1 >= LANES_X_COUNT) ? BLOCK : this->mGrid[y][x + 1].front();

	// Set gravity position
	for (int i = y; i >= 0; --i) {
		if (i == 0 || this->mGrid[i - 1][x].front() == BLOCK) {
			this->mCamera->SetGravityPosition(i * LANE_HEIGHT + GRAVITY_POS);
			break;
		}
	}

	// Detected collision
	if (this->mGrid[y][x].front() != EMPTY) {
		this->Collide(this->mGrid[y][x].front());
		this->mGrid[y][x].front() = EMPTY;
	}

	/*for (int z = 1; z < LANES_Z_COUNT; z++) {
		for (int y = 0; y < LANES_Y_COUNT; y++) {
			for (int x = 0; x < LANES_X_COUNT; x++) {
				GameItem front = this->mGrid[y][x].front();
				this->mGrid[y][x].pop();
				this->mGrid[y][x].push(front);
			}
		}
	}*/
}

/* Executes actions according to different types of collision with game items */
void Game::Collide(GameItem item) {
	switch (item)
	{
	case BLOCK:
		this->mGameState = LOST;
		break;
	case COIN:
		this->mScore += COIN_VALUE;
		this->mSoundEngine->play2D("Sounds/Coin.mp3");
		break;
	}
}

/* Generates all of the scene items */
void Game::GenerateSceneItems() {
	// Clear the grid's first slice if we exceeded the whole tile
	ClearGrid();

	while (this->mGrid[0][0].size() < LANES_Z_COUNT) {
		// if we consumed the whole block then get a new one
		if (mBlockSliceIdx >= LANES_Z_COUNT) {
			// Randomlly get a new game block
			this->mBlockId = rand() % (BLOCKS_COUNT - 1) + 1;
			this->mBlockSliceIdx = 0;

			// Increase camera speed every new block
			this->mCamera->AccelerateSpeed();
		}

		// fills the queue with the slice items
		for (int y = 0; y < LANES_Y_COUNT; ++y) {
			for (int x = 0; x < LANES_X_COUNT; ++x) {
				this->mGrid[y][x].push(mSceneBlocks[mBlockId][mBlockSliceIdx][y][x]);
			}
		}

		this->mBlockSliceIdx++;
	}
}

/* Clears the passed scene items from the grid */
void Game::ClearGrid() {
	if (this->mGrid[0][0].empty())
		return;

	int idx = abs(this->mCamera->GetPosition().z / LANE_DEPTH);

	if (this->mGridIndexZ < idx) {
		this->mGridIndexZ = idx;

		for (int y = 0; y < LANES_Y_COUNT; ++y) {
			for (int x = 0; x < LANES_X_COUNT; ++x) {
				this->mGrid[y][x].pop();
			}
		}

		// moves the Z porition of the camera back to it's initial position
		//this->mCamera->SetPosition(glm::vec3(cameraPosition.x, cameraPosition.y, CAMERA_POSITION.z));
	}
}

/* Resets the game initial values */
void Game::ResetGame() {
	this->mScore = 0;
	this->mGameStartTime = (int)glfwGetTime();
	this->mGameState = RUNNING;

	this->mCamera->SetPosition(CAMERA_POSITION_INIT);
	this->mCamera->SetMoveSpeed(CAMERA_SPEED_INIT);
	this->mCamera->StopAnimation();

	this->mBlockId = 0;
	this->mGridIndexZ = 0;
	this->mBlockSliceIdx = 0;
	this->mBorderLeft = EMPTY;
	this->mBorderRight = EMPTY;
	
	for (int y = 0; y < LANES_Y_COUNT; ++y) {
		for (int x = 0; x < LANES_X_COUNT; ++x) {
			while (!mGrid[y][x].empty()) {
				this->mGrid[y][x].pop();
			}
		}
	}
}

/* Initializes the game sounds and background music */
void Game::InitSounds() {
	this->mSoundEngine = createIrrKlangDevice();
	this->mSoundEngine->play2D("Sounds/Conan.mp3", GL_TRUE);
}

/* Initializes the game models */
void Game::InitModels() {
	this->mScene = new Model("Models/scene/scene.obj");
	this->mCube = new Model("Models/cube/cube.obj");
	this->mCoin = new Model("Models/coin/coin.obj");
	this->mSphere = new Model("Models/sphere/sphere.obj");
	this->mRing = new Model("Models/ring/ring.obj");

	this->GenerateSceneItems();
}

/* Initializes the game blocks */
void Game::InitGameBlocks() {
	ifstream fin;
	fin.open("Levels/Level.txt");
	if (!fin.is_open()) {
		std::cout << "GAME::ERROR: Could not load file " << "Levels/Level.txt" << std::endl;
		return;
	}

	string line;

	for (int b = 0; b < BLOCKS_COUNT; ++b) {
		for (int y = 0; y < LANES_Y_COUNT; ++y) {
			for (int x = 0; x < LANES_X_COUNT; ++x) {
				fin >> line;
				// Line is empty or a comment
				if (line.size() == 0 || line[0] == '#') {
					x--;
					continue;
				}

				for (int z = 0; z < LANES_Z_COUNT; ++z) {
					mSceneBlocks[b][z][y][x] = (GameItem)(line[z] - '0');
				}
			}
		}
	}

	fin.close();
}

/* Initializes the game shaders */
void Game::InitShaders() {
	this->mShader = new Shader("Shaders/lighting_vertex.shader", "Shaders/lighting_fragment.shader");
	this->mTextShader = new Shader("Shaders/text_vertex.shader", "Shaders/text_fragment.shader");
}

/* Initializes the game camera */
void Game::InitCamera() {
	int w, h;
	glfwGetWindowSize(this->mEngine->mWind, &w, &h);
	mCamera = new Camera(CAMERA_POSITION_INIT, (double)w / (double)h);
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

/* Initializes the game text renderers */
void Game::InitTextRenderers() {
	int w, h;
	glfwGetWindowSize(this->mEngine->mWind, &w, &h);
	this->mTextRenderer = new TextRenderer("Fonts/nickname.ttf", FONT_SIZE, w, h);
}