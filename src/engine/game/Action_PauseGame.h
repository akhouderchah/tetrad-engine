#pragma once

#include "engine/event/IAction.h"

namespace tetrad
{

class Game;

/** @brief System observer action to pause the game. */
class Action_PauseGame : public IAction
{
 public:
  Action_PauseGame(Game *pGame);

  bool operator()(EEventAction) override;

 private:
  Game *m_pGame;
  bool m_ShouldPause;
};

}  // namespace tetrad
