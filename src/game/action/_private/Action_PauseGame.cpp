#include "Action_PauseGame.h"
#include "MaterialComponent.h"

Entity Action_PauseGame::s_FadeEntity = nullEntity;
bool Action_PauseGame::s_bFadeOut = false;

Action_PauseGame::Action_PauseGame()
{}

bool Action_PauseGame::operator()(EEventAction action)
{
	if(action != EEventAction::ON){ return false; }

	MaterialComponent *fader = EntityManager::GetComponent<MaterialComponent>(s_FadeEntity);
	if(s_bFadeOut)
	{
		fader->FadeOut(.25f);
	}
	else
	{
		fader->FadeIn(.5f);
	}
	s_bFadeOut = !s_bFadeOut;
	return true;
}
