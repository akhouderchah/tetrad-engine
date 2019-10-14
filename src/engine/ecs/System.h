#pragma once

#include "core/BaseTypes.h"

namespace tetrad
{

class Game;

/** @brief Base class for all systems. */
class System
{
 public:
  System() : m_pGame(nullptr) {}
  virtual ~System() {};

  /** @brief Initialize the system. Only returns true if successful. */
  bool Initialize(Game *pGame)
  {
    m_pGame = pGame;
    return OnInitialize();
  }
  /** @brief Shut down the system. */
  void Shutdown() { OnShutdown(); }

  /** @brief Execute system functionality for this game tick. */
  virtual void Tick(deltaTime_t dt) = 0;

 protected:
  virtual bool OnInitialize() { return true; }
  virtual void OnShutdown() {}

  Game *m_pGame;
};

}  // namespace tetrad
