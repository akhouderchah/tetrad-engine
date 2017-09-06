#include "Action_PauseGame.h"
#include "MaterialComponent.h"
#include "Game.h"

Action_PauseGame::Action_PauseGame(Game *pGame, Entity fadeScreen) :
	m_pGame(pGame), m_FadeEntity(fadeScreen),
	m_bFadeOut(false)
{}

bool Action_PauseGame::operator()(EEventAction action)
{
	if(action != EEventAction::ON){ return false; }

	MaterialComponent *fader = EntityManager::GetComponent<MaterialComponent>(m_FadeEntity);
	if(m_bFadeOut)
	{
		fader->FadeOut(.25f);
		m_pGame->SetCurrentState(m_PrevState);
	}
	else
	{
		fader->FadeIn(.5f);
		m_PrevState = m_pGame->GetCurrentState();
		m_pGame->SetCurrentState(EGameState::PAUSED);
	}
	m_bFadeOut = !m_bFadeOut;
	return true;
}
