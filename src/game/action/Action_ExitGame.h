#pragma once

#include "IAction.h"

/**
 * @brief Action to exit the game
 */
class Action_ExitGame : public IAction
{
public:
	Action_ExitGame(GLFWwindow *pWindow);
	virtual ~Action_ExitGame(){}

	virtual bool operator()(EEventAction);

private:
	GLFWwindow *m_pWindow;
};
