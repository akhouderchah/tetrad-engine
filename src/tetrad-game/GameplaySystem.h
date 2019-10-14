#pragma once

#include "core/BaseTypes.h"
#include "engine/ecs/System.h"

namespace tetrad
{

/** @brief System that takes care of gameplay functionality. */
class GameplaySystem : public System
{
 public:
  virtual void Tick(deltaTime_t dt);
};

}  // namespace tetrad
