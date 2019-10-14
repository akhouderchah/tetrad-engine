#pragma once

#include "core/BaseTypes.h"
#include "core/Reflection.h"
#include "engine/ecs/IComponent.h"

namespace tetrad
{

class MovableComponent;
class Action_Move;

/**
 * @brief Component to give physical simulation capabilities.
 *
 * Requires the MovableComponent to function properly.
 */
COMPONENT()
class PhysicsComponent : public IComponent
{
 public:
  PhysicsComponent(Entity entity);

  void Refresh() override;

  void Tick(deltaTime_t dt);
  bool Impulse();  // Returns true only if the impulse was successful

  void SetVelocity(glm::vec3 velocity) { m_Velocity = velocity; }
  void IncrementVelocity(const glm::vec3& velocity) { m_Velocity += velocity; }

  void SetGravity(bool on) { m_IsGravityOn = on; }

  void SetMovementSpeed(float speed) { m_MovementSpeed = speed; }

 private:
  /**
   * @brief Turn on or off a certain direction of movement
   *
   * @note Movement directions are defined in Action_Move::EMoveDirection
   */
  void UpdateMovement(int direction, bool on);

  friend class Action_Move;

 private:
  static float s_Gravity;
  static float s_TerminalSpeedXZ;
  static float s_TerminalSpeedY;
  static float s_ImpulseSpeed;
  static float s_ImpulseWaitTime;

  bool m_IsGravityOn;
  int16_t m_MovementDir;

  float m_MovementSpeed;
  float m_ImpulseWait;

  glm::vec3 m_Velocity;
  glm::vec3 m_Movement;

  MovableComponent* m_pMover;
};

}  // namespace tetrad
