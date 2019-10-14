#include "engine/physics/PhysicsComponent.h"

#include "engine/ecs/EntityManager.h"
#include "engine/transform/MovableComponent.h"

namespace tetrad
{

#define DEFAULT_MOVEMENT_SPEED 2.f

float PhysicsComponent::s_Gravity = -6.45f;
float PhysicsComponent::s_ImpulseSpeed = 2.85f;
float PhysicsComponent::s_TerminalSpeedXZ = 2.75f;
float PhysicsComponent::s_TerminalSpeedY = 4.75f;

float PhysicsComponent::s_ImpulseWaitTime = 0.0f;

PhysicsComponent::PhysicsComponent(Entity entity)
    : IComponent(entity),
      m_IsGravityOn(true),
      m_MovementDir(0),
      m_MovementSpeed(DEFAULT_MOVEMENT_SPEED),
      m_ImpulseWait(0.f),
      m_Velocity(0, 0, 0),
      m_Movement(0, 0, 0),
      m_pMover(nullptr)
{
}

void PhysicsComponent::Refresh()
{
  m_pMover = EntityManager::GetComponent<MovableComponent>(m_Entity);
}

void PhysicsComponent::Tick(deltaTime_t dt)
{
  m_ImpulseWait -= dt;

  if (m_Velocity[1] >= -s_TerminalSpeedY)
  {
    m_Velocity[1] += m_IsGravityOn * s_Gravity * dt;
  }

  m_pMover->Move(m_Velocity * dt);
  m_pMover->Move(m_Movement * dt, EMoveType::LOCAL);
}

bool PhysicsComponent::Impulse()
{
  if (m_ImpulseWait > 0.f)
  {
    return false;
  }

  m_ImpulseWait = s_ImpulseWaitTime;
  m_Velocity[1] = s_ImpulseSpeed;
  return true;
}

void PhysicsComponent::UpdateMovement(int direction, bool on)
{
  // Use m_MovementDir as a 2-element int8_t array
  int8_t *moveDir = (int8_t *)&m_MovementDir;

  int neg = (2 * on - 1) * (1 - 2 * (direction & 1));
  moveDir[0] += neg * !(direction & 2);
  moveDir[1] += neg * !!(direction & 2);

  if (m_MovementDir != 0)
  {
    int8_t magnitude = abs(moveDir[0]) + abs(moveDir[1]);
    m_Movement[0] = moveDir[0];
    m_Movement[2] = moveDir[1];
    m_Movement *= (m_MovementSpeed / magnitude);
  }
  else
  {
    m_Movement[0] = 0;
    m_Movement[2] = 0;
  }
}

}  // namespace tetrad
