#pragma once

#include "IAction.h"
#include "EntityManager.h"

/**
 * @brief Action to cause the provided entity to move in a specified direction
 *
 * @note Relies on the referenced entity having a MovableComponent. Otherwise,
 * this Action will log an error in debug mode.
 */
class Action_Move : public IAction
{
public:
	enum EMoveDirection
	{
		EMD_RIGHT = 0,
		EMD_LEFT,
		EMD_FORWARDS,
		EMD_BACKWARDS,
	};

	Action_Move(Entity entity, EMoveDirection direction);
	~Action_Move();

	/**
	 * @brief Execute the movement
	 *
	 * @return true unless the entity could not be moved
	 */
	bool operator()(EEventAction);

private:
	Entity m_Entity;
	EMoveDirection m_Direction;
	glm::vec3 m_MoveVec;
};
