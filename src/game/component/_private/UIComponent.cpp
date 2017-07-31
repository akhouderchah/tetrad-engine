#include "UIComponent.h"
#include "TransformComponent.h"

using namespace glm;

UIComponent::UIComponent(Entity entity) :
	IComponent(entity),
	m_PartitionRectangle(0,0,0,0),
	m_CurrTex(0), m_pTransformComp(nullptr)
{
}

UIComponent::~UIComponent()
{
}

void UIComponent::Refresh()
{
	m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
}

screenBound_t UIComponent::GetScreenBounds() const
{
	const vec3 &startPos = m_pTransformComp->GetPosition();
	const vec3 &endPos = startPos + m_pTransformComp->GetScale();
	return screenBound_t(startPos[0], startPos[1], endPos[0], endPos[1]);
}
