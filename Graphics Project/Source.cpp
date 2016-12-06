#include "Game/GameEngine.h"
#include "Game/Game.h"


int main() {
	GameEngine MyGameEngine(1280, 720, false);
	Game MyGame(&MyGameEngine, "Graphics Game");
	MyGameEngine.Run();
	return 0;
}
