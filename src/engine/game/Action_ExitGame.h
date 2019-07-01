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

	bool operator()(EEventAction) override;

private:
	Screen *m_pScreen;
};
