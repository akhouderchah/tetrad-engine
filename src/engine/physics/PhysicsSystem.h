#pragma once

#include "core/ConstVector.h"
#include "engine/ecs/System.h"

namespace tetrad {

class PhysicsComponent;

/** @brief System to perform physics simulations on relevant components.
 *
 * The main relevant components are:
 * * Physics Component - for rigid bodies
 * * Collision Component - for objects that will block entities with physics components
 *
 * This system will do the necessary calculations involved to make the above components
 * act as they should.
 *
 * @TODO Use some sort of space partitioning so that the work involved for this system is
 * decreased (and also more easily parallelizable).
 */
class PhysicsSystem : public System
{
 public:
  PhysicsSystem();

  void Tick(deltaTime_t dt) override;

 private:
  ConstVector<PhysicsComponent *> m_pPhysicsComponents;
};

}  // namespace tetrad
