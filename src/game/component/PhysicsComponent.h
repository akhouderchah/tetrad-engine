#pragma once

#include "Base.h"
#include "IComponent.h"

enum class EMoveType : int8_t;
class MovableComponent;

/**
 * @brief Component to give physical simulation capabilities.
 *
 * Requires the MovableComponent to function properly.
 */
class PhysicsComponent : public IComponent
{
public:
	PhysicsComponent(Entity entity);

	void Refresh();

	void Tick(deltaTime_t dt);
	bool Impulse();  // Returns true only if the impulse was successful

	void SetVelocity(glm::vec3 velocity){ m_Velocity = velocity; }
	void IncrementVelocity(const glm::vec3 &velocity){ m_Velocity += velocity; }

	void SetGravity(bool on){ m_IsGravityOn = on; }

	void UpdateMovement(int direction, bool on);
	void SetMovementSpeed(float speed){ m_MovementSpeed = speed; }

private:
	static float s_Gravity;
	static float s_TerminalSpeed;
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
