#pragma once

#include "engine/ecs/Entity.h"
#include "engine/event/IAction.h"

namespace tetrad {

/**
 * @brief Action to cause the provided entity to jump.
 *
 * Obviously relies on the entity containing a PhysicsComponent. Lacking this,
 * the Action will not do anything (@todo perhaps log the event?)
 */
class Action_Jump : public IAction
{
 public:
  Action_Jump(Entity entity);

  bool operator()(EEventAction) override;

 private:
  Entity m_Entity;
};

}  // namespace tetrad
