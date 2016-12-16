#include "Game/GameEngine.h"
#include "Game/Game.h"


int main() {
	GameEngine MyGameEngine(1280, 720, false);
	Game MyGame(&MyGameEngine, "Jumper Ball");
	MyGameEngine.Run();
	return 0;
}
