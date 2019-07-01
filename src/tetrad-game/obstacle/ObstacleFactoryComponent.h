#pragma once

#include "core/LinkedList.h"
#include "engine/ecs/IComponent.h"

class GameplaySystem;
class TransformComponent;

/**
 * @brief Component that enables an object to generate obstacles
 */
class ObstacleFactoryComponent : public IComponent
{
public:
	ObstacleFactoryComponent(Entity);
	~ObstacleFactoryComponent();

	void Refresh();

	bool Enable();
	void Disable();

private:
	bool GenerateObstacle();

private:
	static LinkedList<ObstacleFactoryComponent> s_TimeList;
	friend class GameplaySystem;

	TransformComponent *m_pTransformComp;
	glm::vec3 m_ObstacleVelocity;

	LinkedNode<ObstacleFactoryComponent> m_Node;
	deltaTime_t m_TimeRemaining;

	float m_MinDelay;
	float m_MaxDelay;

	bool m_bEnabled;
};
