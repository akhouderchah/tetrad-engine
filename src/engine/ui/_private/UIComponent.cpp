#include "engine/ui/UIComponent.h"

#include "engine/ecs/EntityManager.h"
#include "engine/render/MaterialComponent.h"
#include "engine/resource/ResourceManager.h"
#include "engine/screen/Screen.h"
#include "engine/transform/MovableComponent.h"
#include "engine/transform/TransformComponent.h"
#include "engine/ui/TextComponent.h"

namespace tetrad {

UIComponent::UIComponent(Entity entity)
    : IComponent(entity),
      m_PartitionRectangle(0, 0, 0, 0),
      m_CurrTex(0),
      m_IsMovable(false),
      m_bFollowCursor(false),
      m_Priority(UI_PRIORITY_DEFAULT),
      m_pTransformComp(nullptr),
      m_pMover(nullptr),
      m_pMaterialComp(nullptr),
      m_pTextComp(nullptr)
{}

void UIComponent::Init(Screen &screen) { screen.Inform(this, Screen::EIT_CREATED); }

void UIComponent::Init(Screen &screen, uint8_t priority)
{
  DEBUG_ASSERT(priority < UI_PRIORITY_COUNT);
  m_Priority = priority;

  screen.Inform(this, Screen::EIT_CREATED);
}

void UIComponent::Refresh()
{
  m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
  m_pMover = EntityManager::GetComponent<MovableComponent>(m_Entity);
  m_IsMovable = (m_pMover->GetEntity() != kNullEntity);

  m_pMaterialComp = EntityManager::GetComponent<MaterialComponent>(m_Entity);
  m_pTextComp = EntityManager::GetComponent<TextComponent>(m_Entity);
}

void UIComponent::SetCurrentTexture(const std::string &tex, TextureType type)
{
  m_CurrTex = ResourceManager::LoadTexture(tex, type);
}

void UIComponent::OnHoverEnter() { LOG_DEBUG("Hover Enter\n"); }

void UIComponent::OnHoverLeave() { LOG_DEBUG("Hover Leave\n"); }

void UIComponent::OnTouchEnter()
{
  m_bFollowCursor = m_IsMovable;
  LOG_DEBUG("Touch Enter\n");
}

void UIComponent::OnTouchLeave()
{
  m_bFollowCursor = false;
  LOG_DEBUG("Touch Leave\n");
}

screenBound_t UIComponent::GetScreenBounds() const
{
  const glm::vec3 &startPos = m_pTransformComp->GetAbsolutePosition();
  const glm::vec3 &endPos = startPos + m_pTransformComp->GetAbsoluteScale();
  return screenBound_t(startPos[0], startPos[1], endPos[0], endPos[1]);
}

std::vector<UIComponent *> &UIComponent::GetChildren()
{
  return m_pTransformComp->m_pChildUI;
}

}  // namespace tetrad
