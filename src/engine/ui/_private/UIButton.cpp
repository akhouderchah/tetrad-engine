#include "engine/ui/UIButton.h"

#include "engine/resource/ResourceManager.h"

namespace tetrad {

UIButton::UIButton(Entity entity)
    : UIComponent(entity), m_DefaultTex(0), m_HoverTex(0), m_PressTex(0)
{}

void UIButton::SetTextures(const std::string &defaultTex, const std::string &hoverTex,
                           const std::string &pressTex)
{
  m_DefaultTex = ResourceManager::LoadTexture(defaultTex, TextureType::RGBA);
  m_CurrTex = m_DefaultTex;

  m_HoverTex = ResourceManager::LoadTexture(hoverTex, TextureType::RGBA);
  m_PressTex = ResourceManager::LoadTexture(pressTex, TextureType::RGBA);
}

void UIButton::OnHoverEnter()
{
  m_CurrTex = m_HoverTex;
  UIComponent::OnHoverEnter();
}

void UIButton::OnHoverLeave()
{
  m_CurrTex = m_DefaultTex;
  UIComponent::OnHoverLeave();
}

void UIButton::OnTouchEnter()
{
  m_CurrTex = m_PressTex;
  UIComponent::OnTouchEnter();
}

void UIButton::OnTouchLeave()
{
  m_CurrTex = m_HoverTex;
  UIComponent::OnTouchLeave();
}

}  // namespace tetrad
