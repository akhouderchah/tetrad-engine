#pragma once

#include "Game.h"
#include "ErrorSystem.h"

class EventSystem;
class ObserverComponent;

/**
 * @brief Game editor built on top of engine
 */
class Editor : public Game
{
public:
	Editor();

	virtual bool Initialize(const GameAttributes &attributes);

protected:
	virtual void AddSystems();

private:
	EventSystem *m_pInputSystem;
	Entity m_CameraEntity;

	DrawSystem *m_pDrawSystem;
	ObserverComponent *m_pSystemObserver;
};
