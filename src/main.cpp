#include "Game.h"

int main (int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	Game game;

	if (!game.Initialize()) {
		SDL_Log("Failed to initialize game");
		return 1;
	}

	game.Run();

	// Shutdown is called automatically by the destructor
	
	return 0;
}