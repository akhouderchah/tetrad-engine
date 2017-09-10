#pragma once

#include "IAction.h"

/**
 * @brief System observer action to pause the game
 */
class Action_PauseGame : public IAction
{
public:
	Action_PauseGame(Game *pGame);
	virtual ~Action_PauseGame(){}

	virtual bool operator()(EEventAction);

private:
	Game *m_pGame;
	bool m_ShouldPause;
};
