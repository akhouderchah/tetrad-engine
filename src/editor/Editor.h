#pragma once

#include "engine/ecs/Entity.h"
#include "engine/game/Game.h"

namespace tetrad {

class EventSystem;
class DrawSystem;
class ObserverComponent;

/** @brief Game editor built using the engine. */
class Editor : public Game
{
 public:
  Editor();

 private:
  void OnInitialized() override;
  void AddSystems() override;
  void OnPause() override {}
  void OnResume() override {}

 private:
  EventSystem *m_pInputSystem;
  Entity m_CameraEntity;

  DrawSystem *m_pDrawSystem;
  ObserverComponent *m_pSystemObserver;
};

}  // namespace tetrad
