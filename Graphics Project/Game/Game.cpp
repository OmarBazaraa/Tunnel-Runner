#include "Game.h"

/* Constructs a new game with all related objects and components */
Game::Game(GameEngine* engine, const char* title) {
	this->mEngine = engine;
	this->mEngine->RegisterGame(this, title);
	this->mGameTitle = title;

	srand(time(NULL));

	InitSounds();
	InitCamera();
	InitShaders();
	InitGameBlocks();
	InitModels();
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
	delete this->mCube;
	delete this->mCoin;
	delete this->mGemScore;
	delete this->mGemSpeed;
	delete this->mGemCrazy;

	// Destroy light sources
	delete this->mLight;

	// Destroy text renderers
	delete this->mTextRenderer;
}

/* Returns the current game state */
GameState Game::GetGameState() const {
	return this->mGameState;
}

/* Receives user input and processes it for the next frame */
void Game::ProcessInput() {
	this->ProcessKeyInput();
	//this->ProcessMouseInput();
}

/* Updates objects' information needed to apply effects on the next frame  */
void Game::Update() {
	// Update music
	if (!this->mSoundEngine->isCurrentlyPlaying(BACKGROUND_MUSIC[this->mMusicIdx].c_str())) {
		this->mMusicIdx = (this->mMusicIdx + 1) % BACKGROUND_MUSIC_COUNT;
		this->mSoundEngine->play2D(BACKGROUND_MUSIC[this->mMusicIdx].c_str());
	}

	// Return if game is not running
	if (this->mGameState != RUNNING)
		return;

	// Update runnning game time
	this->mGameTime += this->mEngine->mTimer->ElapsedFramesTime;

	// Removes the double score effect after certain amount of time
	if (this->mDoubleScore) {
		this->mDoubleScoreTime += this->mEngine->mTimer->ElapsedFramesTime;

		if (this->mDoubleScoreTime >= DOUBLE_SCORE_DURATION) {
			this->mDoubleScore = false;
			this->mCoinValue /= 2;
		}
	}

	// Removes the increase speed effect after certain amount of time
	if (this->mIncreaseSpeed) {
		this->mIncreaseSpeedTime += this->mEngine->mTimer->ElapsedFramesTime;

		if (this->mIncreaseSpeedTime >= INCREASE_SPEED_DURATION) {
			this->mIncreaseSpeed = false;
			this->mCamera->SetMoveSpeed(this->mCamera->GetCameraSpeed() / INCREASE_SPEED_FACTOR);
		}
	}

	// Update extra coins time
	if (this->mExtraScore) {
		this->mExtraScoreTime += this->mEngine->mTimer->ElapsedFramesTime;

		if (this->mExtraScoreTime >= EXTRA_SCORE_DURATION) {
			this->mExtraScore = false;
		}
	}

	// Update reversed directions effect
	if (this->mDirectionsReversed) {
		this->mDirectionsReversedTime += this->mEngine->mTimer->ElapsedFramesTime;

		if (this->mDirectionsReversedTime >= DIRECTIONS_REVERSED_DURATION) {
			this->mDirectionsReversed = false;
			this->mLight->SpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
			this->mLight->DiffuseColor = this->mLight->SpecularColor * 0.9f;
			this->mLight->AmbientColor = this->mLight->SpecularColor * 0.3f;
		}
	}

	// Update camera to give animation effects
	this->mCamera->MoveStep(FORWARD, LANE_DEPTH);
	this->mCamera->Update(this->mEngine->mTimer->ElapsedFramesTime);

	// Update light sources
	if (this->mDirectionsReversed) {
		double r = abs(sin(this->mGameTime) / 2.0f) + 0.5f;
		double g = abs(cos(this->mGameTime) / 2.0f) + 0.5f;
		double b = 1.0f;// abs(tan(this->mGameTime) / 2.0f) + 0.5f;
		this->mLight->SpecularColor = glm::vec3(r, g, b);
		this->mLight->DiffuseColor = this->mLight->SpecularColor * 0.9f;
		this->mLight->AmbientColor = this->mLight->SpecularColor * 0.3f;
	}
	
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
				this->mGrid[y][x].pop_front();
				this->mGrid[y][x].push_back(cell);

				switch (cell)
				{
				case BLOCK:
					this->mCube->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - LANES_X_COUNT / 2) * LANE_WIDTH, 0.5f * CUBE_HEIGHT + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mCube->ModelMatrix = glm::scale(this->mCube->ModelMatrix, glm::vec3(CUBE_WIDTH, CUBE_HEIGHT, CUBE_DEPTH));
					this->mCube->Draw(*this->mShader);
					break;
				case COIN:
					this->mCoin->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - LANES_X_COUNT / 2) * LANE_WIDTH, COIN_SIZE + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mCoin->ModelMatrix = glm::scale(this->mCoin->ModelMatrix, glm::vec3(COIN_SIZE, COIN_SIZE, COIN_SIZE));
					this->mCoin->ModelMatrix = glm::rotate(this->mCoin->ModelMatrix, (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
					this->mCoin->Draw(*this->mShader);
					break;
				case GEM_DOUBLE_SCORE:
					this->mGemScore->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - LANES_X_COUNT / 2) * LANE_WIDTH, GEM_SIZE + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mGemScore->ModelMatrix = glm::scale(this->mGemScore->ModelMatrix, glm::vec3(GEM_SIZE, GEM_SIZE, GEM_SIZE));
					this->mGemScore->ModelMatrix = glm::rotate(this->mGemScore->ModelMatrix, (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
					this->mGemScore->Draw(*this->mShader);
					break;
				case GEM_SPEED:
					this->mGemSpeed->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - LANES_X_COUNT / 2) * LANE_WIDTH, GEM_SIZE + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mGemSpeed->ModelMatrix = glm::scale(this->mGemSpeed->ModelMatrix, glm::vec3(GEM_SIZE, GEM_SIZE, GEM_SIZE));
					this->mGemSpeed->ModelMatrix = glm::rotate(this->mGemSpeed->ModelMatrix, (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
					this->mGemSpeed->Draw(*this->mShader);
					break;
				case GEM_EXTRA_SCORE:
				case GEM_REVERSED_MODE:
					this->mGemCrazy->ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3((x - LANES_X_COUNT / 2) * LANE_WIDTH, GEM_SIZE + y * LANE_HEIGHT, -(z + mGridIndexZ) * LANE_DEPTH));
					this->mGemCrazy->ModelMatrix = glm::scale(this->mGemCrazy->ModelMatrix, glm::vec3(GEM_SIZE, GEM_SIZE, GEM_SIZE));
					this->mGemCrazy->ModelMatrix = glm::rotate(this->mGemCrazy->ModelMatrix, (float)this->mEngine->mTimer->CurrentFrameTime, glm::vec3(0.0f, 1.0f, 0.0f));
					this->mGemCrazy->Draw(*this->mShader);
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

	// High score
	ss.clear();
	ss.str("");
	ss << HIGHSCORE_LABEL << this->mHighScore;
	x = FONT_MARGIN;
	y = h - FONT_MARGIN * 3 - FONT_SIZE;
	this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);


	// Time
	ss.clear();
	ss.str("");
	int m = (int)this->mGameTime / 60;
	int s = (int)this->mGameTime % 60;
	ss << TIME_LABEL;
	if (m < 10) ss << 0; ss << m << ".";
	if (s < 10) ss << 0; ss << s;
	x = w - FONT_MARGIN * 10;
	y = h - FONT_MARGIN - FONT_SIZE;
	this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);

	// FPS
	ss.clear();
	ss.str("");
	ss << FPS_LABEL << this->mEngine->mTimer->FPS;
	x = FONT_MARGIN;
	y = FONT_MARGIN;
	this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);

	// Gem score percentage
	if (this->mGameState == RUNNING && this->mDoubleScore) {
		ss.clear();
		ss.str("");
		ss << GEM_SCORE_LABEL << (int)(((DOUBLE_SCORE_DURATION - this->mDoubleScoreTime) / DOUBLE_SCORE_DURATION) * 100) << "%";
		x = (w - this->mGemScoreLabelWidth) / 2;
		y = h - FONT_MARGIN - FONT_SIZE;
		this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);
	}

	// Gem speed percentage
	if (this->mGameState == RUNNING && this->mIncreaseSpeed) {
		ss.clear();
		ss.str("");
		ss << GEM_SPEED_LABEL << (int)(((INCREASE_SPEED_DURATION - this->mIncreaseSpeedTime) / INCREASE_SPEED_DURATION) * 100) << "%";
		x = (w - this->mGemSpeedLabelWidth) / 2;
		y = h - FONT_MARGIN - FONT_SIZE;

		if (this->mDoubleScore) {
			y -= FONT_MARGIN * 3;
		}

		this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);
	}

	// Extra score gift
	if (this->mGameState == RUNNING && this->mExtraScore) {
		x = (w - this->mExtraScoreLabelWidth) / 2;
		y = h / 2 + FONT_MARGIN * FONT_MARGIN * this->mExtraScoreTime;

		this->mTextRenderer->RenderText(*this->mTextShader, GEM_EXTRA_SCORE_LABEL, x, y, FONT_SCALE, FONT_COLOR);
	}

	// Directions reversed percentage
	if (this->mGameState == RUNNING && this->mDirectionsReversed) {
		ss.clear();
		ss.str("");
		ss << GEM_REVERSED_MODE_LABEL << (int)(((DIRECTIONS_REVERSED_DURATION - this->mDirectionsReversedTime) / DIRECTIONS_REVERSED_DURATION) * 100) << "%";
		x = (w - this->mReversedLabelWidth) / 2;
		y = FONT_MARGIN + FONT_SIZE;

		this->mTextRenderer->RenderText(*this->mTextShader, ss.str(), x, y, FONT_SCALE, FONT_COLOR);
	}

	// Game over label
	if (this->mGameState == LOST) {
		x = (w - this->mGameOverMsgWidth) / 2;
		y = h / 2 - FONT_SIZE * MENU_FONT_SCALE + FONT_MARGIN * 2;
		this->mTextRenderer->RenderText(*this->mTextShader, GAME_OVER_MSG, x, y, MENU_FONT_SCALE, FONT_COLOR);
	}

	// Game paused messages
	if (this->mGameState != RUNNING) {
		// Title
		x = (w - this->mGameTitleLabelWidth) / 2;
		y = h / 2 - FONT_SIZE * TITLE_FONT_SCALE + FONT_MARGIN * 7;
		this->mTextRenderer->RenderText(*this->mTextShader, this->mGameTitle, x, y, TITLE_FONT_SCALE, FONT_COLOR);

		// Quit and replay
		x = (w - mMenuMsgWidth) / 2;
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

	// Return if game is not running
	if (this->mGameState != RUNNING) {
		// Quit
		if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_Q) == GLFW_PRESS)
			glfwSetWindowShouldClose(this->mEngine->mWind, GL_TRUE);

		// Replay
		if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_R) == GLFW_PRESS)
			this->ResetGame();

		return;
	}
		

	// Game control
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_A) == GLFW_PRESS && (this->mDirectionsReversed ? mBorderRight : mBorderLeft) != BLOCK)
		this->mCamera->MoveStep(this->mDirectionsReversed ? RIGHT : LEFT, LANE_WIDTH);
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_D) == GLFW_PRESS && (this->mDirectionsReversed ? mBorderLeft : mBorderRight) != BLOCK)
		this->mCamera->MoveStep(this->mDirectionsReversed ? LEFT : RIGHT, LANE_WIDTH);

	// Jump
	if (glfwGetKey(this->mEngine->mWind, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if (!this->mCamera->JumpingOffset()) {
			this->mSoundEngine->play2D("Sounds/Jump.wav");
		}

		this->mCamera->Jump(CAMERA_JUMP_OFFSET);
	}
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
	int z = CHARACTER_OFFSET / LANE_DEPTH;

	if (x <= 0)
		this->mBorderLeft = BLOCK;
	if (x + 1 >= LANES_X_COUNT)
		this->mBorderRight = BLOCK;

	if (this->mCamera->IsMovingRight())
		++x;
	if (this->mCamera->IsJumping() && this->mCamera->JumpingOffset() < LANE_HEIGHT)
		++y;

	// Check if out of range
	if (y < 0 || y > LANES_Y_COUNT || x < 0 || x >= LANES_X_COUNT) {
		return;
	}

	this->GetSlice(z);

	// Set left and right borders
	if (y < LANES_Y_COUNT) {
	this->mBorderLeft = (x <= 0) ? BLOCK : mCharacterGrid[y][x - 1];
	this->mBorderRight = (x + 1 >= LANES_X_COUNT) ? BLOCK : mCharacterGrid[y][x + 1];
	}

	// Set gravity position
	for (int i = y; i >= 0; --i) {
		if (i == 0 || mCharacterGrid[i - 1][x] == BLOCK) {
			this->mCamera->SetGravityPosition(i * LANE_HEIGHT + GRAVITY_POS);
			break;
		}
	}

	// Detected collision
	if (y < LANES_Y_COUNT && mCharacterGrid[y][x] != EMPTY) {
		this->Collide(mCharacterGrid[y][x]);

		if (mCharacterGrid[y][x] != BLOCK) {
			mCharacterGrid[y][x] = EMPTY;
		}
	}

	this->EditSlice(z);
}

/* Gets a slice from the game grid at certain offset in Z lanes*/
void Game::GetSlice(int idx) {
	if (mGrid[0][0].empty())
		return;

	stack<GameItem> temp;

	for (int y = 0; y < LANES_Y_COUNT; ++y) {
		for (int x = 0; x < LANES_X_COUNT; ++x) {
			for (int z = 0; z <= idx; ++z) {
				temp.push(this->mGrid[y][x].front());
				this->mGrid[y][x].pop_front();
			}

			mCharacterGrid[y][x] = temp.top();

			while (!temp.empty()) {
				this->mGrid[y][x].push_front(temp.top());
				temp.pop();
			}
		}
	}
}

/* Edits a slice after collision */
void Game::EditSlice(int idx) {
	if (mGrid[0][0].empty())
		return;

	stack<GameItem> temp;

	for (int y = 0; y < LANES_Y_COUNT; ++y) {
		for (int x = 0; x < LANES_X_COUNT; ++x) {
			for (int z = 0; z <= idx; ++z) {
				temp.push(this->mGrid[y][x].front());
				this->mGrid[y][x].pop_front();
			}

			temp.top() = mCharacterGrid[y][x];

			while (!temp.empty()) {
				this->mGrid[y][x].push_front(temp.top());
				temp.pop();
			}
		}
	}
}

/* Executes actions according to different types of collision with game items */
void Game::Collide(GameItem item) {
	switch (item)
	{
	case BLOCK:
		this->mGameState = LOST;
		this->mSoundEngine->play2D("Sounds/game_over.mp3");
		this->SaveHighScore();
		break;
	case COIN:
		this->mScore += this->mCoinValue;
		this->mSoundEngine->play2D("Sounds/coin.mp3");
		break;
	case GEM_DOUBLE_SCORE:
		this->mDoubleScoreTime = 0.0f;
		if (!mDoubleScore) {
			this->mCoinValue *= 2;
			this->mDoubleScore = true;
		}
		this->mSoundEngine->play2D("Sounds/gem.wav");
		break;
	case GEM_SPEED:
		this->mIncreaseSpeedTime = 0.0f;
		if (!mIncreaseSpeed) {
			this->mCamera->SetMoveSpeed(this->mCamera->GetCameraSpeed() * INCREASE_SPEED_FACTOR);
			this->mIncreaseSpeed = true;
		}
		this->mSoundEngine->play2D("Sounds/gem.wav");
		break;
	case GEM_EXTRA_SCORE:
		this->mExtraScoreTime = 0.0f;
		this->mExtraScore = true;
		this->mScore += EXTRA_COINS_VALUE;
		this->mSoundEngine->play2D("Sounds/gem.wav");
		break;
	case GEM_REVERSED_MODE:
		this->mDirectionsReversedTime = 0.0f;
		this->mDirectionsReversed = true;
		this->mSoundEngine->play2D("Sounds/gem.wav");
		break;
	}
}

/* Generates all of the scene items */
void Game::GenerateSceneItems() {
	// Clear the grid's first slice if we exceeded the whole tile
	ClearGrid();

	while (this->mGrid[0][0].size() < LANES_Z_COUNT) {
		// If we consumed the whole block then get a new one
		if (mBlockSliceIdx >= LANES_Z_COUNT) {
			// Randomlly get a new game block
			this->mBlockId = rand() % (mBlocksCount - 1) + 1;
			this->mBlockSliceIdx = 0;

			// Increase camera speed every new block
			this->mCamera->AccelerateSpeed();
		}

		// Fills the queue with the slice items
		for (int y = 0; y < LANES_Y_COUNT; ++y) {
			for (int x = 0; x < LANES_X_COUNT; ++x) {
				// Don't always spawn the gem but some times spawn it and sometimes no (for more rarity)
				if (mSceneBlocks[mBlockSliceIdx][y][x][mBlockId] >= GEM_DOUBLE_SCORE && mSceneBlocks[mBlockSliceIdx][y][x][mBlockId] <= GEM_REVERSED_MODE) {
					int random = rand() % 10;

					if (random == 0) {
						if (mSceneBlocks[mBlockSliceIdx][y][x][mBlockId] == GEM_EXTRA_SCORE || mSceneBlocks[mBlockSliceIdx][y][x][mBlockId] == GEM_REVERSED_MODE) {
							if (rand() % 2 == 0)
								mSceneBlocks[mBlockSliceIdx][y][x][mBlockId] = GEM_EXTRA_SCORE;
							else
								mSceneBlocks[mBlockSliceIdx][y][x][mBlockId] = GEM_REVERSED_MODE;
						}

						this->mGrid[y][x].push_back(mSceneBlocks[mBlockSliceIdx][y][x][mBlockId]);
					}
					else {
						this->mGrid[y][x].push_back(COIN);
					}
				}
				else {
					this->mGrid[y][x].push_back(mSceneBlocks[mBlockSliceIdx][y][x][mBlockId]);
				}
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
				this->mGrid[y][x].pop_front();
			}
		}
	}
}

/* Resets the game initial values */
void Game::ResetGame() {
	this->mScore = 0;
	this->mHighScore = this->ReadHighScore();
	this->mGameTime = 0;
	this->mGameState = RUNNING;
	this->mCoinValue = COIN_VALUE;
	this->mDoubleScore = false;
	this->mIncreaseSpeed = false;
	this->mExtraScore = false;
	this->mDirectionsReversed = false;

	this->mCamera->SetPosition(CAMERA_POSITION_INIT);
	this->mCamera->SetMoveSpeed(CAMERA_SPEED_INIT);
	this->mCamera->StopAnimation();

	this->mLight->SpecularColor = glm::vec3(1.0f, 1.0f, 1.0f);
	this->mLight->DiffuseColor = this->mLight->SpecularColor * 0.9f;
	this->mLight->AmbientColor = this->mLight->SpecularColor * 0.3f;

	this->mBlockId = 0;
	this->mGridIndexZ = 0;
	this->mBlockSliceIdx = 0;
	this->mBorderLeft = EMPTY;
	this->mBorderRight = EMPTY;

	for (int y = 0; y < LANES_Y_COUNT; ++y) {
		for (int x = 0; x < LANES_X_COUNT; ++x) {
			this->mCharacterGrid[y][x] = EMPTY;

			while (!mGrid[y][x].empty()) {
				this->mGrid[y][x].pop_front();
			}
		}
	}
}

/* Saves the high score in a file */
void Game::SaveHighScore() {
	if (this->mScore < this->mHighScore) {
		return;
	}

	ofstream fout("highscore.txt");

	if (!fout.is_open()) {
		std::cout << "GAME::ERROR: Unable to update highscore" << std::endl;
		return;
	}

	fout << this->mScore;
	fout.close();
}

/* Reads the high score from the file */
int Game::ReadHighScore() {
	ifstream fin("highscore.txt");

	// Failed to find high score file
	if (!fin.is_open()) {
		std::cout << "GAME::ERROR: Unable to load highscore" << std::endl;
		return -1;
	}

	int score = 0;
	fin >> score;
	fin.close();

	return score;
}

/* Initializes the game sounds and background music */
void Game::InitSounds() {
	this->mSoundEngine = createIrrKlangDevice();
	this->mSoundEngine->play2D(BACKGROUND_MUSIC[0].c_str());
}

/* Initializes the game models */
void Game::InitModels() {
	this->mScene = new Model("Models/scene/scene.obj");
	this->mCube = new Model("Models/cube/cube.obj");
	this->mCoin = new Model("Models/coin/coin.obj");
	this->mGemScore = new Model("Models/gem_score/gem_score.obj");
	this->mGemSpeed = new Model("Models/gem_speed/gem_speed.obj");
	this->mGemCrazy = new Model("Models/gem_crazy/gem_crazy.obj");

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

	string line = "#";
	while (line[0] == '#' || line.size() == 0) {
		getline(fin, line);
	}
	mBlocksCount = stoi(line);


	for (int b = 0; b < mBlocksCount; ++b) {
		for (int y = 0; y < LANES_Y_COUNT; ++y) {
			for (int x = 0; x < LANES_X_COUNT; ++x) {
				getline(fin, line);
				// Line is empty or a comment
				if (line.size() == 0 || line[0] == '#') {
					x--;
					continue;
				}

				for (int z = 0; z < LANES_Z_COUNT; ++z) {
					mSceneBlocks[z][y][x].resize(mBlocksCount);
					mSceneBlocks[z][y][x][b] = (GameItem)(line[z] - '0');
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
	this->mCamera = new Camera(CAMERA_POSITION_INIT, (double)w / (double)h);
	this->mCamera->SetMoveAcceleration(CAMERA_ACCELERATION);
}

/* Initializes the game light sources */
void Game::InitLightSources() {
	this->mLight = new LightSource(0.0f, 0.0f, 0.0f);
	this->mLight->AttenuationConstant = 0.5f;
	this->mLight->AttenuationLinear = 0.1f;
	this->mLight->AttenuationQuadratic = 0.032f;
}

/* Initializes the game text renderers */
void Game::InitTextRenderers() {
	int w, h;
	glfwGetWindowSize(this->mEngine->mWind, &w, &h);
	this->mTextRenderer = new TextRenderer("Fonts/nickname.ttf", FONT_SIZE, w, h);

	this->mGameTitleLabelWidth = this->mTextRenderer->GetTextWidth(this->mGameTitle, TITLE_FONT_SCALE);
	this->mGameOverMsgWidth = this->mTextRenderer->GetTextWidth(GAME_OVER_MSG, MENU_FONT_SCALE);
	this->mMenuMsgWidth = this->mTextRenderer->GetTextWidth(MENU_MSG, MENU_FONT_SCALE);
	this->mGemScoreLabelWidth = this->mTextRenderer->GetTextWidth(GEM_SCORE_LABEL + "100%", FONT_SCALE);
	this->mGemSpeedLabelWidth = this->mTextRenderer->GetTextWidth(GEM_SPEED_LABEL + "100%", FONT_SCALE);
	this->mReversedLabelWidth = this->mTextRenderer->GetTextWidth(GEM_REVERSED_MODE_LABEL + "100%", FONT_SCALE);
	this->mExtraScoreLabelWidth = this->mTextRenderer->GetTextWidth(GEM_EXTRA_SCORE_LABEL, FONT_SCALE);
}