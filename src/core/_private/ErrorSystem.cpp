#include "core/ErrorSystem.h"

#include "engine/game/Game.h"

namespace tetrad
{
Game *ErrorSystem::s_pGame = nullptr;

void ErrorSystem::SetCurrentGame(Game *pGame) { s_pGame = pGame; }

void ErrorSystem::ForceExitGame()
{
  if (s_pGame)
  {
    s_pGame->Shutdown();
  }
  exit(EXIT_FAILURE);
}

void _assert_exit(std::string cond, const char *file, int line)
{
  // Prevent warnings in release build
  (void)cond;
  (void)file;
  (void)line;

  DEBUG_LOG("ASSERTION FAILED: \'" << cond << "\' in " << file << ":" << line << "\n");
  ErrorSystem::ForceExitGame();
}

}  // namespace tetrad
