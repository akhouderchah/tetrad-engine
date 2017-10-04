#pragma once

#include "Game.h"

class EventSystem;
class ObserverComponent;

/**
 * @brief Small game to display engine functionality
 */
class TetradGame : public Game
{
public:
	TetradGame();

	virtual bool Initialize(const GameAttributes& attributes);

	virtual bool Pause() final;
	virtual bool Unpause() final;

protected:
	virtual void AddSystems();

private:
	DrawSystem *m_pDrawSystem;
	ObserverComponent *m_pSystemObserver;
	EventSystem *m_pInputSystem;

	Entity m_FadeScreen;
	Entity m_PauseText;
	Entity m_ExitText;

	double m_PrevX;
	double m_PrevY;
};
