#include "engine/ui/UIViewport.h"

#include "engine/render/MaterialComponent.h"

namespace tetrad
{

UIViewport::UIViewport(Entity entity)
    : UIComponent(entity), m_pCamera(nullptr), m_pScreen(nullptr)
{
  m_Priority = 0;
}

void UIViewport::Refresh()
{
  UIComponent::Refresh();

  //
  // Bypassing the const-ness because we can't make m_pMaterialComp a const pointer
  // (other UIComponent types may want to modify their material components)
  //
  m_pMaterialComp = (MaterialComponent *)&MaterialComponent::GetDefaultTransparent();
}

void UIViewport::Init(Screen &screen)
{
  UIComponent::Init(screen);

  m_pScreen = &screen;
}

void UIViewport::Init(Screen &screen, uint8_t priority)
{
  UIComponent::Init(screen, priority);

  m_pScreen = &screen;
}

}  // namespace tetrad
