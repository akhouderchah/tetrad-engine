#include "UIComponent.h"
#include "TransformComponent.h"

UIComponent::UIComponent(Entity entity) :
	IComponent(entity),
	m_PartitionRectangle(0,0,0,0)
{
}

UIComponent::~UIComponent()
{
}

void UIComponent::Refresh()
{
	m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
}

bool UIComponent::IsAt(double x, double y)
{
	(void)x; (void)y;
	// TODO
	return false;
}
