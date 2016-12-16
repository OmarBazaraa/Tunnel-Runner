#include "Game.h"

/* Constructs a new game with all related objects and components */
Game::Game(GameEngine* engine, const char* title) {
	this->mEngine = engine;
	this->mEngine->RegisterGame(this, title);

	InitSounds();
	InitCamera();
	InitShaders();
	InitGameBlocks();
	InitModels();
	InitLightSources();
	InitTextRenderers();

	ResetGame();

	mColliding.Right = EMPTY;
	mColliding.Left = EMPTY;
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
	if (this->mGameState != RUNNING) {
		return;
	}

	// Update camera
	this->mCamera->Update(this->mEngine->mTimer->ElapsedFramesTime);

	// Update light sources
	this->mLight->Position = this->mCamera->GetPosition();
	this->mLight->Position -= this->mCamera->GetFront();

	// Moves the camera forward every frame by default
	this->mCamera->SetMoveSpeed(mCameraSpeed);
	this->mCamera->MoveStep(FORWARD, LANE_DEPTH);
	//this->mCameraSpeed += CAMERA_ACCELERATION;

	// Move the scene with the camera to make it feel endless
	glm::vec3 cameraPosition = this->mCamera->GetPosition();
	this->mScene->ModelMatrix = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.5 * SCENE_HEIGHT, cameraPosition.z - 0.4 * SCENE_DEPTH - CAMERA_POSITION.z));
	this->mScene->ModelMatrix = glm::scale(this->mScene->ModelMatrix, glm::vec3(SCENE_WIDTH, SCENE_HEIGHT, SCENE_DEPTH));

	Collide(this->mCamera->GetPosition() - CAMERA_POSITION);
	
	// Populate the game items (mGrid) to be rendered
	GenerateSceneItems();



	// Update models

}

void Game::Collide(glm::vec3 character) {
	GameItem colliding;
	mColliding.Left = mColliding.Right = EMPTY;
	int x = character.x / LANE_WIDTH + (LANES_X_COUNT - 1) / 2;
	int y = character.y / LANE_HEIGHT;
	if (y >= 0 && y < LANES_Y_COUNT && x >= 0 && x < LANES_X_COUNT && !mGrid[y][x].empty()) {
		colliding = this->mGrid[y][x].front();

		if (x == 0)mColliding.Left = BLOCK;
		else  mColliding.Left = this->mGrid[y][x - 1].front();

		if (x + 1 == LANES_X_COUNT)mColliding.Right = BLOCK;
		else mColliding.Right = this->mGrid[y][x + 1].front();

		for (int i = y; i >= 0; --i)
			if (i == 0 || this->mGrid[i - 1][x].front() == BLOCK) {
				this->mCamera->SetGravityPosition((i + 1)*LANE_HEIGHT);
				break;
			}
		
		

		if (colliding == COIN) {
			mScore++;
			this->mGrid[y][x].front() = EMPTY;
		}
		else if (colliding == BLOCK) {
			this->mGameState = LOST;
		}
	}
}
/* Renders the new frame */
void Game::Render() {
	// Apply effect to the shader
	this->mShader->Use();
	this->mCamera->ApplyEffects(*mShader);
	this->mLight->ApplyEffects(*mShader);

	// Draw the scene
	this->mScene->Draw(*this->mShader);

	// Draw the block
	for (int y = 0; y < LANES_Y_COUNT; ++y) {
		for (int x = 0; x < LANES_X_COUNT; ++x) {
			for (int z = 0; z < this->mGrid[y][x].size(); ++z) {
				GameItem cell = this->mGrid[y][x].front();
				this->mGrid[y][x].pop();
				this->mGrid[y][x].push(cell);

				switch (cell)
				{
				case BLOCK:
					this->mCube->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - (int)(LANE_WIDTH + 1) / 2) * LANE_WIDTH, 0.5f * CUBE_HEIGHT + y * LANE_HEIGHT, -(z + mZGridIndex) * LANE_DEPTH));
					this->mCube->ModelMatrix = glm::scale(this->mCube->ModelMatrix, glm::vec3(CUBE_SIZE, CUBE_HEIGHT, CUBE_DEPTH));
					this->mCube->Draw(*this->mShader);
					break;
				case COIN:
					this->mCoin->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - (int)(LANE_WIDTH + 1) / 2) * LANE_WIDTH, COIN_SIZE + y * LANE_HEIGHT, -(z + mZGridIndex) * LANE_DEPTH));
					this->mCoin->ModelMatrix = glm::scale(this->mCoin->ModelMatrix, glm::vec3(COIN_SIZE, COIN_SIZE, COIN_SIZE));
					this->mCoin->ModelMatrix = glm::rotate(this->mCoin->ModelMatrix, (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
					this->mCoin->Draw(*this->mShader);
					break;
				case SPHERE:
					this->mSphere->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - (int)(LANE_WIDTH + 1) / 2) * LANE_WIDTH, SPHERE_RADIUS + y * LANE_HEIGHT, -(z + mZGridIndex) * LANE_DEPTH));
					this->mSphere->ModelMatrix = glm::scale(this->mSphere->ModelMatrix, glm::vec3(SPHERE_RADIUS, SPHERE_RADIUS, SPHERE_RADIUS));
					this->mSphere->Draw(*this->mShader);
					break;
				case RING:
					this->mRing->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - (int)(LANE_WIDTH + 1) / 2) * LANE_WIDTH, RING_RADIUS + y * LANE_HEIGHT, -(z + mZGridIndex) * LANE_DEPTH));
					this->mRing->ModelMatrix = glm::scale(this->mRing->ModelMatrix, glm::vec3(RING_RADIUS, RING_RADIUS, RING_DEPTH));
					this->mRing->Draw(*this->mShader);
					break;
				}
			}
		}
	}

	this->RenderText();
}

/* Renders the text of the game */
void Game::RenderText() {
	int w, h, textWidth;
	glfwGetWindowSize(this->mEngine->mWind, &w, &h);
	stringstream ss;

	// Score
	ss << SCORE_LABEL << this->mScore;
	this->mTextRenderer->RenderText(
		*this->mTextShader,
		ss.str(),
		FONT_MARGIN,
		h - FONT_MARGIN - FONT_SIZE,
		FONT_SCALE,
		FONT_COLOR
	);

	// Time
	ss.clear();
	ss.str("");
	ss << TIME_LABEL << (int)this->mEngine->mTimer->CurrentFrameTime;
	textWidth = this->mTextRenderer->GetTextWidth(ss.str(), FONT_SCALE);
	this->mTextRenderer->RenderText(
		*this->mTextShader,
		ss.str(),
		w - FONT_MARGIN * 8,
		h - FONT_MARGIN - FONT_SIZE,
		FONT_SCALE,
		FONT_COLOR
	);

	// FPS
	ss.clear();
	ss.str("");
	ss << FPS_LABEL << this->mEngine->mTimer->FPS;
	this->mTextRenderer->RenderText(
		*this->mTextShader,
		ss.str(),
		FONT_MARGIN,
		FONT_MARGIN,
		FONT_SCALE,
		FONT_COLOR
	);
	//----------------------------------------------

	//
	// Draw Menu
	//
	if (this->mGameState == LOST) {
		textWidth = this->mTextRenderer->GetTextWidth(GAME_OVER_MSG, MENU_FONT_SCALE);

		this->mTextRenderer->RenderText(
			*this->mTextShader,
			GAME_OVER_MSG,
			(w - textWidth) / 2,
			h / 2 - FONT_SIZE * MENU_FONT_SCALE + FONT_MARGIN * 2,
			MENU_FONT_SCALE,
			FONT_COLOR
		);
	}

	if (this->mGameState != RUNNING) {
		textWidth = this->mTextRenderer->GetTextWidth(MENU_MSG, MENU_FONT_SCALE);

		this->mTextRenderer->RenderText(
			*this->mTextShader,
			MENU_MSG,
			(w - textWidth) / 2,
			h / 2 - FONT_SIZE * MENU_FONT_SCALE,
			MENU_FONT_SCALE,
			FONT_COLOR
		);
	}
}

/* Processes inputs from keyboard */
void Game::ProcessKeyInput() {
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_ESCAPE) == GLFW_RELEASE) {
		this->mEscReleased = true;
	}

	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_ESCAPE) == GLFW_PRESS && this->mEscReleased && this->mGameState != LOST) {
		this->mGameState = (this->mGameState == PAUSED) ? RUNNING : PAUSED;
		this->mEscReleased = false;
	}

	if (this->mGameState != RUNNING) {
		this->ProcessMenuInput();
		return;
	}

	if (mColliding.Left != BLOCK && glfwGetKey(this->mEngine->mWind, GLFW_KEY_A) == GLFW_PRESS)
		this->mCamera->MoveStep(LEFT, LANE_WIDTH);
	if (mColliding.Right != BLOCK && glfwGetKey(this->mEngine->mWind, GLFW_KEY_D) == GLFW_PRESS)
		this->mCamera->MoveStep(RIGHT, LANE_WIDTH);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_SPACE) == GLFW_PRESS)
		this->mCamera->Jump(CAMERA_JUMP_OFFSET);

}

/* Processes inputs for the game menu while the game is paused */
void Game::ProcessMenuInput() {
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_Q) == GLFW_PRESS)
		glfwSetWindowShouldClose(this->mEngine->mWind, GL_TRUE);

	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_R) == GLFW_PRESS)
		this->ResetGame();
}

/* Processes inputs from mouse */
void Game::ProcessMouseInput() {
	if (this->mGameState != RUNNING) {
		return;
	}

	double xpos, ypos;
	glfwGetCursorPos(this->mEngine->mWind, &xpos, &ypos);
	this->mCamera->ChangeDirection(xpos, ypos, this->mEngine->mTimer->ElapsedFramesTime);
}

/* Resets the game initial values */
void Game::ResetGame() {
	this->mScore = 0;
	this->mGameState = RUNNING;
	this->mBlockSliceIdx = 0;
	this->mCameraSpeed = CAMERA_SPEED_INIT;
	this->mZGridIndex = 0;
	this->mCamera->SetPosition(CAMERA_POSITION);
	this->mBlockSliceIdx = 0;
	this->mBlockId = 0;
	for (int y = 0; y < LANES_Y_COUNT; ++y) {
		for (int x = 0; x < LANES_X_COUNT; ++x) {
			while (!mGrid[y][x].empty())
				this->mGrid[y][x].pop();
		}
	}
}

/* Initializes the game sounds and background music */
void Game::InitSounds() {
	this->mSoundEngine = createIrrKlangDevice();
	this->mSoundEngine->play2D("Sounds/Conan.mp3", GL_TRUE);
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
	this->mTextShader = new Shader("Shaders/text_vertex.shader", "Shaders/text_fragment.shader");
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

	this->GenerateSceneItems();
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

/* Initializes the game blocks */
void Game::InitGameBlocks() {
	ifstream in;
	in.open("Levels/Level.txt");
	if (!in.is_open()) {
		std::cout << "couldn't load the level file\n";
		return;
	}
	string line;

	for (int b = 0; b < BLOCKS_COUNT; b++) {
		for (int y = 0; y < LANES_Y_COUNT; ++y) {
			for (int x = 0; x < LANES_X_COUNT; ++x) {
				in >> line;
				//line is empty or a comment
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

	in.close();
}

/* Initializes the game text renderers */
void Game::InitTextRenderers() {
	int w, h;
	glfwGetWindowSize(this->mEngine->mWind, &w, &h);
	this->mTextRenderer = new TextRenderer("Fonts/nickname.ttf", FONT_SIZE, w, h);
}

/* Generates all of the scene items */
void Game::GenerateSceneItems() {
	//remove from the grid all of the hidden slices
	ClearGrid();

	while (this->mGrid[0][0].size() < LANES_Z_COUNT) {

		// if the block is entirely put into the queue then we need to get a new one
		if (mBlockSliceIdx == LANES_Z_COUNT) {
			// Randomlly gets the game item block
			this->mBlockId = rand() % (BLOCKS_COUNT - 1) + 1;

			this->mBlockSliceIdx = 0;
			// increase the speed of the camera
			this->mCameraSpeed = min(CAMERA_ACCELERATION + mCameraSpeed, CAMERA_SPEED_MAX);
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

/* Clears the grid queue from extra scenes that will not be seen */
void Game::ClearGrid() {
	if (this->mGrid[0][0].empty())
		return;

	glm::vec3 cameraPosition = this->mCamera->GetPosition();
	int newZIndexPos = abs(cameraPosition.z / LANE_DEPTH);
	if (newZIndexPos > this->mZGridIndex) {
		this->mZGridIndex = newZIndexPos;
		for (int y = 0; y < LANES_Y_COUNT; ++y) {
			for (int x = 0; x < LANES_X_COUNT; ++x) {
				this->mGrid[y][x].pop();
			}
		}
		// moves the Z porition of the camera back to it's initial position
		//this->mCamera->SetPosition(glm::vec3(cameraPosition.x, cameraPosition.y, CAMERA_POSITION.z));
	}
}