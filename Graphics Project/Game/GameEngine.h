#pragma once

// GL Includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Other includes
#include "../Game/Game.h"
#include "../Utils/FrameTimer.h"


/*
	Game engine responsible of running the given game
*/
class GameEngine
{
	friend class Game;

private:
	GLFWwindow* mWind;
	Game* mGame;
	FrameTimer* mTimer;

public:
	/* Constructs a new game engine with all related objects and components */
	GameEngine(int width, int height, bool fullscreen = false);

	/* Destructs the game engine and free resources */
	~GameEngine();

	/* Registers the game that the engine is going to run */
	void RegisterGame(Game* game, const char* title);

	/* Starts the main game loop */
	void Run();

private:
	/* Receives user input and processes it for the next frame */
	void ProcessInput();

	/* Updates the game for the next frame  */
	void Update();

	/* Clears the screen and draws the new frame */
	void Render();

	/* Initializes the game window */
	void InitWindow(int width, int height, const char* title, bool fullscreen);
};