#pragma once

#include "core/IAction.h"

namespace tetrad {

/**
 * @brief System observer action to pause the game
 */
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
