#pragma once

#include <vector>

#include "engine/ecs/System.h"
#include "engine/event/EventQueue.h"

namespace tetrad
{

class ObserverComponent;

/**
 * @brief Takes input from SFML, "converts" it to events, and sends
 * the events out to ObserverComponents.
 *
 * Multiple systems can be created to handle different types of events (since
 * registered Observers will get every event from an EventSystem, regardless of
 * their relevance).
 *
 * ObserverComponents will register themselves with this system, and will
 * then be able to react to game events until unregistering themselves.
 *
 * @note Unlike other systems, the EventSystem handles the registering of
 * the ObserverComponent itself rather than using a ComponentManager. The reason
 * for this is that the ComponentManager works at the level of "these entities have
 * these components", while the EventSystem is concerned instead with which components
 * to notify. Note that the separation of different EventSystems for different types
 * of events would not work well if we simply took every observer from the component
 * manager.
 */
class EventSystem : public System
{
 public:
  void Tick(deltaTime_t dt) override;

  /**
   * @brief Designate this EventSystem as the one to handle input.
   * @return true iff there is no current InputSystem.
   */
  bool MakeInputSystem();

  /**
   * @brief Undesignate this EventSystem as the one to handle input.
   *
   * @note This method only does anything if this EventSystem is currently
   * handling input. Otherwise the call is ignored.
   */
  void UnmakeInputSystem();

  /** @brief Inform all registered observers of an event. */
  void Inform(const Event& event);

  static void SetMouseSensitivity(double);
  inline static double GetMouseSensitivity() { return s_MouseSensitivity; }

  inline static EventSystem* GetInputSystem() { return s_pInputSystem; }

 private:
  friend class ObserverComponent;

  /**
   * @brief Registers an observer with the system. Returs true if there was no error
   *
   * @note Observers will be added multiple times if multiple calls to this
   * function is made with the same observer! Debug builds will check for this
   * and log a warning, but release builds will not perform any such checks.
   */
  bool RegisterObserver(ObserverComponent& observer);

  /**
   * @brief Unregisters an observer with the system
   *
   * @note Only removes the first instance of observer found in the system.
   * If the observer has been registered multiple times, the observer won't be
   * completely unregistered.
   */
  void UnregisterObserver(ObserverComponent& observer);

  // Overrides from System.
  void OnShutdown() override;

 private:
  // This system doesn't necessarily only handle input, but it's the only one
  // that will get input events
  static EventSystem* s_pInputSystem;

  EventQueue m_EventQueue;
  std::vector<ObserverComponent*> m_pObservers;

  static double s_MouseSensitivity;
};

}  // namespace tetrad
