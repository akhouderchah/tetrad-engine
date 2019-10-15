#include "engine/event/EventSystem.h"

#include "core/GlTypes.h"
#include "core/LogSystem.h"
#include "engine/event/Constants.h"
#include "engine/event/ObserverComponent.h"

namespace tetrad {

EventSystem* EventSystem::s_pInputSystem = nullptr;
double EventSystem::s_MouseSensitivity = 1.0;

void EventSystem::Tick(deltaTime_t dt)
{
  (void)dt;

  // Get events and such
  glfwPollEvents();

  Event event = m_EventQueue.Consume();
  while (event.event != EGE_END)
  {
    for (size_t i = 0; i < m_pObservers.size(); ++i)
    {
      m_pObservers[i]->Notify(event);
    }
    event = m_EventQueue.Consume();
  }
}

bool EventSystem::MakeInputSystem()
{
  if (s_pInputSystem)
  {
    return false;
  }

  s_pInputSystem = this;

  return true;
}

void EventSystem::UnmakeInputSystem()
{
  if (s_pInputSystem == this)
  {
    s_pInputSystem = nullptr;
  }
}

bool EventSystem::RegisterObserver(ObserverComponent& observer)
{
// In debug mode, check to see if we're adding an observer multiple times
#ifdef _DEBUG
  for (size_t i = 0; i < m_pObservers.size(); ++i)
  {
    if (m_pObservers[i] == &observer)
    {
      LOG_SPECIAL(
          "WARNING",
          "The observer at memory location: "
              << &observer
              << " is being registered multiple times to the same EventSystem!\n");
    }
  }
#endif

  m_pObservers.push_back(&observer);

  return true;
}

void EventSystem::UnregisterObserver(ObserverComponent& observer)
{
  for (size_t i = 0; i < m_pObservers.size(); ++i)
  {
    if (m_pObservers[i] == &observer)
    {
      std::swap(m_pObservers[i], m_pObservers.back());
      m_pObservers.pop_back();
      return;
    }
  }
}

void EventSystem::Inform(const Event& event)
{
#ifdef _DEBUG
  bool success =
#endif
      m_EventQueue.PushEvent(event);

#ifdef _DEBUG
  if (!success)
  {
    DEBUG_LOG(
        "Failed to push event;"
        "consider making the EventQueue size larger!\n");
  }
#endif
}

void EventSystem::SetMouseSensitivity(double sensitivity)
{
  s_MouseSensitivity = sensitivity;
}

void EventSystem::OnShutdown() { UnmakeInputSystem(); }

}  // namespace tetrad
