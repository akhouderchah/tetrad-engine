#pragma once

#include <glm/vec3.hpp>

#include "engine/ecs/Entity.h"
#include "engine/event/IAction.h"

namespace tetrad {

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

  /**
   * @brief Execute the movement
   *
   * @return true unless the entity could not be moved
   */
  bool operator()(EEventAction) override;

 private:
  Entity m_Entity;
  EMoveDirection m_Direction;
  glm::vec3 m_MoveVec;
};

}  // namespace tetrad
