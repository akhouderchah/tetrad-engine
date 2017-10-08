#include "ObstacleFactoryComponent.h"
#include "EntityManager.h"
#include "TransformComponent.h"
#include "MovableComponent.h"
#include "DrawComponent.h"
#include "PhysicsComponent.h"

LinkedList<ObstacleFactoryComponent> ObstacleFactoryComponent::s_TimeList;

#define DEFAULT_MIN_DELAY .1f
#define DEFAULT_MAX_DELAY 1.f

ObstacleFactoryComponent::ObstacleFactoryComponent(Entity entity) :
	IComponent(entity), m_TimeRemaining(0.0),
	m_MinDelay(DEFAULT_MIN_DELAY), m_MaxDelay(DEFAULT_MAX_DELAY),
	m_bEnabled(false)
{
}

ObstacleFactoryComponent::~ObstacleFactoryComponent()
{
	Disable();
}

void ObstacleFactoryComponent::Refresh()
{
	m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
}

bool ObstacleFactoryComponent::Enable()
{
	if(m_bEnabled)
	{
		return false;
	}

	// Generate time remaining
	m_TimeRemaining = Random::GetGlobalInstance().GetRand(m_MinDelay, m_MaxDelay);

	// Find place in list & insert
	LinkedNode<ObstacleFactoryComponent> *pNode = s_TimeList.Begin();
	if(!pNode)
	{
		s_TimeList.PushFront(m_Node);
	}
	else
	{
		while(pNode)
		{
			ObstacleFactoryComponent *pComp =
				linked_node_owner(pNode,
								  ObstacleFactoryComponent,
								  m_Node);

			auto nodeTime = pComp->m_TimeRemaining;
			if(nodeTime > m_TimeRemaining)
			{
				s_TimeList.PushBefore(*pNode, m_Node);
				pComp->m_TimeRemaining -= m_TimeRemaining;
				break;
			}

			pNode = s_TimeList.Next(*pNode);
		}
	}

	m_bEnabled = true;
	return true;
}

void ObstacleFactoryComponent::Disable()
{
	if(m_bEnabled)
	{
		s_TimeList.Remove(m_Node);
		m_bEnabled = false;
	}
}

bool ObstacleFactoryComponent::GenerateObstacle()
{
	DEBUG_LOG("Generating obstacle\n");

	Entity entity = EntityManager::CreateEntity();
	entity.Add<TransformComponent>()->Init(glm::vec3(0.f, 0.f, 1.f),
										   glm::vec3(.2f, .2f, .2f));
	entity.Add<MovableComponent>();
	auto pDraw = entity.Add<DrawComponent>();
	pDraw->SetGeometry(ShapeType::CUBE);
	pDraw->SetGeometry(MODEL_PATH + "suzanne.obj");
	pDraw->SetTexture(TEXTURE_PATH + "Black.tga", TextureType::RGB);
	auto pPhys = entity.Add<PhysicsComponent>();
	pPhys->SetGravity(false);
	pPhys->SetVelocity(glm::vec3(1, 1, 1));

	return true;
}
