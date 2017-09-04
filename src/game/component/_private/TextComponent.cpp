#include "TextComponent.h"

TextComponent::TextComponent(Entity entity) :
	IComponent(entity), m_pFont(nullptr)
{
}

TextComponent::~TextComponent()
{
}

void TextComponent::Refresh()
{
}
