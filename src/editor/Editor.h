#pragma once

#include "engine/ecs/Entity.h"
#include "engine/game/Game.h"

namespace tetrad {

class EventSystem;
class DrawSystem;
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

}  // namespace tetrad
