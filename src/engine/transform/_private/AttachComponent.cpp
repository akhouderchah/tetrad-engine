#include "engine/transform/AttachComponent.h"

#include <algorithm>

#include "engine/ecs/EntityManager.h"
#include "engine/transform/TransformComponent.h"
#include "engine/ui/UIComponent.h"

namespace tetrad
{

AttachComponent::AttachComponent(Entity entity)
    : IComponent(entity), m_pOwnedTransform(nullptr), m_pUIComp(nullptr)
{
}

AttachComponent::~AttachComponent()
{
  if (!EntityManager::InShutdown())
  {
    Unattach();
  }
}

void AttachComponent::Refresh()
{
  m_pOwnedTransform = EntityManager::GetComponent<TransformComponent>(m_Entity);

  UIComponent *pUI = EntityManager::GetComponent<UIComponent>(m_Entity);

  // Update parent transform's UI ptrs
  if (pUI != m_pUIComp)
  {
    TransformComponent *pParent =
        EntityManager::GetComponent<TransformComponent>(m_AttachEntity);
    if (pParent->GetID() != 0)
    {
      // Remove old UI ptr
      std::vector<UIComponent *> &vec = pParent->m_pChildUI;
      vec.erase(std::remove(vec.begin(), vec.end(), m_pUIComp), vec.end());

      // Add new UI ptr
      if (pUI->GetID() != 0)
      {
        pParent->m_pChildUI.push_back(pUI);
      }

      // Update UI ptr
      m_pUIComp = pUI;
    }
  }
}

bool AttachComponent::Attach(Entity entity)
{
  if (entity == m_Entity || !m_AttachEntity.IsNull())
  {
    return false;
  }

  m_AttachEntity = entity;
  TransformComponent *pParent =
      EntityManager::GetComponent<TransformComponent>(m_AttachEntity);

  m_pOwnedTransform->m_pParentTransform = pParent;
  m_pOwnedTransform->MarkDirty();

  pParent->m_ChildEntities.insert(m_Entity);
  UIComponent *pUI = EntityManager::GetComponent<UIComponent>(m_Entity);
  if (pUI->GetID() != 0)
  {
    pParent->m_pChildUI.push_back(pUI);
  }
  return true;
}

void AttachComponent::Unattach()
{
  if (!m_AttachEntity.IsNull())
  {
    TransformComponent *pParent =
        EntityManager::GetComponent<TransformComponent>(m_AttachEntity);
    if (pParent->GetID() != 0)
    {
      pParent->m_ChildEntities.erase(m_Entity);

      // Remove UIComp ptr from parent transform
      if (m_pUIComp->GetID() != 0)
      {
        std::vector<UIComponent *> &vec = pParent->m_pChildUI;
        vec.erase(std::remove(vec.begin(), vec.end(), m_pUIComp), vec.end());
      }
    }

    m_pOwnedTransform->m_pParentTransform = nullptr;
    m_AttachEntity = Entity();
  }
}

void AttachComponent::RefreshParent(TransformComponent *pParent)
{
  if (m_pOwnedTransform->GetID() != 0)
  {
    m_pOwnedTransform->m_pParentTransform = pParent;
  }
}

}  // namespace tetrad
