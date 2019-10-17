#include <iostream>

#include "core/Platform.h"
#include "engine/ecs/EntityManager.h"
#include "tetrad-game/TetradGame.h"

using namespace tetrad;

int main()
{
  if (!programInitialize())
  {
    LOG_ERROR("Failed to do platform-specific program initialization!");
    return -1;
  }
  LOG_DEBUG("Platform-specific program initialization successful\n");

  TetradGame game;
  GameAttributes attributes(ScreenAttributes(1280, 960, false, false, false, 4, 4, 4,
                                             "Tetrad " + kVersionString),
                            MouseMode::DISABLED);
  if (!game.Initialize(attributes))
  {
    return -1;
  }
  LOG_DEBUG("Game initialization successful\n");

  game.Run();
  game.Shutdown();

  return 0;
}
