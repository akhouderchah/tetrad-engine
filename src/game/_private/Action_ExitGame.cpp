#include "Action_ExitGame.h"
#include "Screen.h"

Action_ExitGame::Action_ExitGame(Screen *pScreen) :
	m_pScreen(pScreen)
{}

bool Action_ExitGame::operator()(EEventAction)
{
	m_pScreen->Shutdown();

	return true;
}
