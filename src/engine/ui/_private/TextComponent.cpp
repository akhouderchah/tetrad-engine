#include "engine/ui/TextComponent.h"

#include "engine/ecs/EntityManager.h"
#include "engine/transform/TransformComponent.h"
#include "engine/ui/UIComponent.h"

namespace tetrad
{

LinkedList<TextComponent> TextComponent::s_FreeTextComps;

TextComponent::TextComponent(Entity entity)
    : IComponent(entity),
      m_pFont(nullptr),
      m_pTransformComp(nullptr),
      m_pUIComp(nullptr),
      m_IsFree(false),
      m_Scale(1.f),
      m_Color(1.f, 1.f, 1.f, 1.f)
{
}

TextComponent::~TextComponent()
{
  if (m_IsFree)
  {
    s_FreeTextComps.Remove(m_FreeTextNode);
    m_IsFree = false;
  }
}

void TextComponent::Refresh()
{
  m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);

  UIComponent *pComp = EntityManager::GetComponent<UIComponent>(m_Entity);
  if (m_pUIComp && m_pUIComp->GetID() == 0 && pComp->GetID() != 0)
  {
    s_FreeTextComps.Remove(m_FreeTextNode);
    m_IsFree = false;
  }
  else if ((!m_pUIComp || m_pUIComp->GetID() != 0) && pComp->GetID() == 0)
  {
    s_FreeTextComps.PushBack(m_FreeTextNode);
    m_IsFree = true;
  }

  m_pUIComp = pComp;
}

}  // namespace tetrad
