#pragma once

#include "core/IAction.h"

class Screen;

/**
 * @brief Action to exit the game
 */
class Action_ExitGame : public IAction
{
public:
	Action_ExitGame(Screen *pScreen);
	virtual ~Action_ExitGame(){}

	virtual bool operator()(EEventAction);

private:
	Screen *m_pScreen;
};
