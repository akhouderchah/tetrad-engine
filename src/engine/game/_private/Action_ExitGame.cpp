#include "engine/game/Action_ExitGame.h"

#include "engine/screen/Screen.h"

namespace tetrad {

Action_ExitGame::Action_ExitGame(Screen *pScreen) : m_pScreen(pScreen) {}

bool Action_ExitGame::operator()(EEventAction)
{
  m_pScreen->Shutdown();

  return true;
}

}  // namespace tetrad
