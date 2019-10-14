#include "engine/event/ObserverComponent.h"

#include <unordered_set>

#include "engine/event/EventSystem.h"

namespace tetrad
{

ObserverComponent::ObserverComponent(Entity entity) : IComponent(entity)
{
  for (size_t i = 0; i < EGE_END; ++i)
  {
    m_pAction[i] = nullptr;
  }
}

ObserverComponent::~ObserverComponent()
{
  // Add action pointers to an unordered set and then delete
  // Allows us to use the same pointer for multiple events
  std::unordered_set<IAction*> pActions;
  for (size_t i = 0; i < EGE_END; ++i)
  {
    pActions.insert(m_pAction[i]);
  }

  for (auto iter = pActions.begin(); iter != pActions.end(); ++iter)
  {
    delete *iter;
  }

  for (size_t i = 0; i < m_pEventSystems.size(); ++i)
  {
    Unsubscribe(*m_pEventSystems[i]);
  }
}

bool ObserverComponent::Subscribe(EventSystem& pSystem)
{
  return pSystem.RegisterObserver(*this);
}

void ObserverComponent::Unsubscribe(EventSystem& pSystem)
{
  pSystem.UnregisterObserver(*this);
}

bool ObserverComponent::AddEvent(const EGameEvent& event, IAction* pAction)
{
  if (m_pAction[event])
  {
    delete pAction;
    return false;
  }

  m_pAction[event] = pAction;
  return true;
}

bool ObserverComponent::UpdateEvent(const EGameEvent& event, IAction* pAction)
{
  if (!m_pAction[event])
  {
    delete pAction;
    return false;
  }

  delete m_pAction[event];
  m_pAction[event] = pAction;
  return true;
}

void ObserverComponent::Notify(const Event& event)
{
  if (m_pAction[event.event])
  {
    (*m_pAction[event.event])(event.action);
  }
}

}  // namespace tetrad
