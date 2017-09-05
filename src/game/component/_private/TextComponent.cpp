#include "TextComponent.h"
#include "TransformComponent.h"

TextComponent::TextComponent(Entity entity) :
	IComponent(entity), m_pFont(nullptr),
	m_pTransformComp(nullptr)
{
}

TextComponent::~TextComponent()
{
}

void TextComponent::Refresh()
{
	m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
}
