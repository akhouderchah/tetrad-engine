#include "engine/game/Action_PauseGame.h"

#include "engine/game/Game.h"

namespace tetrad
{

Action_PauseGame::Action_PauseGame(Game *pGame) : m_pGame(pGame), m_ShouldPause(true) {}

bool Action_PauseGame::operator()(EEventAction action)
{
  if (action != EEventAction::ON)
  {
    return false;
  }

  if (m_ShouldPause)
  {
    m_pGame->Pause();
  }
  else
  {
    m_pGame->Resume();
  }
  m_ShouldPause = !m_ShouldPause;

  return true;
}

}  // namespace tetrad
