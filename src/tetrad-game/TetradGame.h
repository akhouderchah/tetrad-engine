#pragma once

#include "engine/ecs/Entity.h"
#include "engine/game/Game.h"

namespace tetrad
{

class DrawSystem;
class EventSystem;
class ObserverComponent;

/** @brief Small game to display engine functionality. */
class TetradGame : public Game
{
 public:
  TetradGame();

 private:
  void OnInitialized() override;
  void AddSystems() override;

  void OnPause() override;
  void OnResume() override;

 private:
  DrawSystem *m_pDrawSystem;
  ObserverComponent *m_pSystemObserver;
  EventSystem *m_pInputSystem;

  Entity m_FadeScreen;
  Entity m_PauseText;
  Entity m_ExitText;

  double m_PrevX;
  double m_PrevY;
};

}  // namespace tetrad
