#include "engine/physics/Action_Jump.h"

#include "engine/ecs/EntityManager.h"
#include "engine/physics/PhysicsComponent.h"

namespace tetrad
{

Action_Jump::Action_Jump(Entity entity) : m_Entity(entity) {}

bool Action_Jump::operator()(EEventAction action)
{
  if (action != EEventAction::ON)
  {
    return false;
  }

  PhysicsComponent* pPhys = m_Entity.GetAs<PhysicsComponent>();
  if (pPhys)
  {
    pPhys->Impulse();
  }
  return false;
}

}  // namespace tetrad
