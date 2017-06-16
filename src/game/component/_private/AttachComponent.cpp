#include "AttachComponent.h"
#include "TransformComponent.h"

AttachComponent::AttachComponent(Entity entity) :
	IComponent(entity), m_pOwnedTransform(nullptr)
{
}

AttachComponent::~AttachComponent()
{
	if(!EntityManager::InShutdown())
	{
		Unattach();
	}
}

void AttachComponent::Refresh()
{
	m_pOwnedTransform = EntityManager::GetComponent<TransformComponent>(m_Entity);
}

bool AttachComponent::Attach(Entity entity)
{
	if(entity == m_Entity || !m_AttachEntity.IsNull())
	{
		return false;
	}

	m_AttachEntity = entity;
	TransformComponent *pParent = EntityManager::GetComponent<TransformComponent>(m_AttachEntity);

	m_pOwnedTransform->m_pParentTransform = pParent;
	m_pOwnedTransform->MarkDirty();
	pParent->m_ChildEntities.insert(m_Entity);
	return true;
}

void AttachComponent::Unattach()
{
	if(!m_AttachEntity.IsNull())
	{
		TransformComponent *pParent = EntityManager::GetComponent<TransformComponent>(m_AttachEntity);
		if(pParent->GetID() != 0)
		{
			pParent->m_ChildEntities.erase(m_Entity);
		}

		m_pOwnedTransform->m_pParentTransform = nullptr;
		m_AttachEntity = Entity();
	}
}

void AttachComponent::RefreshParent(TransformComponent *pParent)
{
	if(m_pOwnedTransform->GetID() != 0)
	{
		m_pOwnedTransform->m_pParentTransform = pParent;
	}
}
