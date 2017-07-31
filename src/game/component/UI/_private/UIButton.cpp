#include "UI/UIButton.h"
#include "ResourceManager.h"

UIButton::UIButton(Entity entity) :
	UIComponent(entity), m_DefaultTex(0),
	m_HoverTex(0), m_PressTex(0)
{

}

UIButton::~UIButton()
{
}

void UIButton::SetTextures(const std::string &defaultTex,
						   const std::string &hoverTex,
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
	DEBUG_LOG("Button hover enter\n");
}

void UIButton::OnHoverLeave()
{
	m_CurrTex = m_DefaultTex;
	DEBUG_LOG("Button hover leave\n");
}

void UIButton::OnTouchEnter()
{
	m_CurrTex = m_PressTex;
}

void UIButton::OnTouchLeave()
{
	m_CurrTex = m_HoverTex;
}
