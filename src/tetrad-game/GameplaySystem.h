#pragma once

#include "core/BaseTypes.h"
#include "engine/ecs/ISystem.h"

namespace tetrad
{

class Game;

/**
 * @brief System that takes care of gameplay functionality
 */
class GameplaySystem : public ISystem
{
 public:
  GameplaySystem();

  virtual bool Initialize(Game *pGame);
  virtual void Shutdown();

  virtual void Tick(deltaTime_t dt);

 private:
};

}  // namespace tetrad
