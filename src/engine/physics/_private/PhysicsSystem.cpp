#include "engine/physics/PhysicsSystem.h"

#include "engine/ecs/EntityManager.h"
#include "engine/game/Game.h"
#include "engine/physics/PhysicsComponent.h"

namespace tetrad
{

PhysicsSystem::PhysicsSystem()
    : m_pPhysicsComponents(EntityManager::GetAll<PhysicsComponent>())
{
}

void PhysicsSystem::Tick(deltaTime_t dt)
{
  if (m_pGame->GetCurrentState() != EGameState::STARTED)
  {
    return;
  }

  for (size_t i = 1; i < m_pPhysicsComponents.size(); ++i)
  {
    m_pPhysicsComponents[i]->Tick(dt);
  }
}

}  // namespace tetrad
