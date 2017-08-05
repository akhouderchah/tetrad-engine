#include "UIComponent.h"
#include "TransformComponent.h"
#include "MovableComponent.h"

using namespace glm;

UIComponent::UIComponent(Entity entity) :
	IComponent(entity),
	m_PartitionRectangle(0,0,0,0),
	m_CurrTex(0), m_IsMovable(false), m_bFollowCursor(false),
	m_pTransformComp(nullptr),
	m_pMover(nullptr)
{
}

UIComponent::~UIComponent()
{
}

void UIComponent::Refresh()
{
	m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
	m_pMover = EntityManager::GetComponent<MovableComponent>(m_Entity);
	m_IsMovable = (m_pMover->GetEntity() != nullEntity);
}

void UIComponent::SetCurrentTexture(const std::string &tex, TextureType type)
{
	m_CurrTex = ResourceManager::LoadTexture(tex, type);
}

void UIComponent::OnHoverEnter()
{
	DEBUG_LOG("Hover Enter\n");
}

void UIComponent::OnHoverLeave()
{
	DEBUG_LOG("Hover Leave\n");
}

void UIComponent::OnTouchEnter()
{
	m_bFollowCursor = m_IsMovable;
	DEBUG_LOG("Touch Enter\n");
}

void UIComponent::OnTouchLeave()
{
	m_bFollowCursor = false;
	DEBUG_LOG("Touch Leave\n");
}

screenBound_t UIComponent::GetScreenBounds() const
{
	const vec3 &startPos = m_pTransformComp->GetAbsolutePosition();
	const vec3 &endPos = startPos + m_pTransformComp->GetAbsoluteScale();
	return screenBound_t(startPos[0], startPos[1], endPos[0], endPos[1]);
}

std::vector<UIComponent*> &UIComponent::GetChildren()
{
	return m_pTransformComp->m_pChildUI;
}
