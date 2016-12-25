#include "Game/GameEngine.h"
#include "Game/Game.h"


int main() {
	GameEngine MyGameEngine(1920, 1080, true);
	Game MyGame(&MyGameEngine, "Tunnel Runner");
	MyGameEngine.Run();
	return 0;
}