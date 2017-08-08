#include "UI/UIViewport.h"

UIViewport::UIViewport(Entity entity) :
	UIComponent(entity),
	m_pCamera(nullptr), m_pScreen(nullptr)
{
}
