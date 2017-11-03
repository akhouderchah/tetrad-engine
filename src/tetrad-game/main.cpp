#include "Platform.h"
#include "EntityManager.h"
#include "TetradGame.h"

#include <iostream>

using namespace std;

int main()
{
	if(!programInitialize())
	{
		LOG_ERROR("Failed to do platform-specific program initialization!");
		return -1;
	}
	DEBUG_LOG("Platform-specific program initialization successful\n");

	TetradGame game;
	GameAttributes attributes(
		ScreenAttributes(1280, 960, false, false, false, 4, 4, 4,
						 "Tetrad " + VERSION_STRING),
		MouseMode::DISABLED);
	if(!game.Initialize(attributes))
	{
		return -1;
	}
	DEBUG_LOG("Game initialization successful\n");

	game.Run();
	game.Shutdown();

	return 0;
}
