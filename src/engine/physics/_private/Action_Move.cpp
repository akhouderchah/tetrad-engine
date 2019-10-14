#include "engine/physics/Action_Move.h"

#include "core/LogSystem.h"
#include "engine/ecs/EntityManager.h"
#include "engine/physics/PhysicsComponent.h"

namespace tetrad
{

Action_Move::Action_Move(Entity entity, EMoveDirection direction)
    : m_Entity(entity), m_Direction(direction)
{
}

bool Action_Move::operator()(EEventAction action)
{
  PhysicsComponent *pPhys = m_Entity.GetAs<PhysicsComponent>();
  if (!pPhys)
  {
    DEBUG_LOG("Entity " << static_cast<ObjectHandle>(m_Entity).GetID()
                        << " has no PhysicsComponent, and thus can't be moved\n");
    return false;
  }
  pPhys->UpdateMovement(m_Direction, (bool)action);

  return true;
}

}  // namespace tetrad
