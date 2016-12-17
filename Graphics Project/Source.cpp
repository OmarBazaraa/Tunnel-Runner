#include "Game/GameEngine.h"
#include "Game/Game.h"


int main() {
	GameEngine MyGameEngine(1280, 720, false);
	Game MyGame(&MyGameEngine, "Tunnel Runner");
	MyGameEngine.Run();
	return 0;
}
