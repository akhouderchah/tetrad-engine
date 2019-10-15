#include "tetrad-game/obstacle/ObstacleFactoryComponent.h"

#include "core/Paths.h"
#include "core/Rand.h"
#include "engine/ecs/EntityManager.h"
#include "engine/physics/PhysicsComponent.h"
#include "engine/render/DrawComponent.h"
#include "engine/transform/MovableComponent.h"
#include "engine/transform/TransformComponent.h"

namespace tetrad {

LinkedList<ObstacleFactoryComponent> ObstacleFactoryComponent::s_TimeList;

#define DEFAULT_MIN_DELAY .5f
#define DEFAULT_MAX_DELAY 1.5f

ObstacleFactoryComponent::ObstacleFactoryComponent(Entity entity)
    : IComponent(entity),
      m_TimeRemaining(0.0),
      m_MinDelay(DEFAULT_MIN_DELAY),
      m_MaxDelay(DEFAULT_MAX_DELAY),
      m_bEnabled(false)
{}

ObstacleFactoryComponent::~ObstacleFactoryComponent() { Disable(); }

void ObstacleFactoryComponent::Refresh()
{
  m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
}

bool ObstacleFactoryComponent::Enable()
{
  if (m_bEnabled)
  {
    return false;
  }

  // Generate time remaining
  m_TimeRemaining = Random::GetGlobalInstance().GetRand(m_MinDelay, m_MaxDelay);

  // Find place in list & insert
  LinkedNode<ObstacleFactoryComponent> *pNode = s_TimeList.First();
  if (!pNode)
  {
    s_TimeList.PushFront(m_Node);
  }
  else
  {
    while (pNode)
    {
      ObstacleFactoryComponent *pComp =
          linked_node_owner(pNode, ObstacleFactoryComponent, m_Node);

      auto nodeTime = pComp->m_TimeRemaining;
      if (nodeTime > m_TimeRemaining)
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
  if (m_bEnabled)
  {
    s_TimeList.Remove(m_Node);
    m_bEnabled = false;
  }
}

bool ObstacleFactoryComponent::GenerateObstacle()
{
  DEBUG_LOG("Generating obstacle\n");
  DEBUG_ASSERT(m_pTransformComp->GetID() != 0);

  Entity entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(m_pTransformComp->GetAbsolutePosition(),
                                         glm::vec3(.2f, .2f, .2f));
  entity.Add<MovableComponent>();
  auto pDraw = entity.Add<DrawComponent>();
  pDraw->SetGeometry(ShapeType::CUBE);
  pDraw->SetTexture(TEXTURE_PATH + "Black.tga", TextureType::RGB);
  auto pPhys = entity.Add<PhysicsComponent>();
  pPhys->SetGravity(false);
  pPhys->SetVelocity(glm::vec3(-1, 0, 0));

  return true;
}

}  // namespace tetrad
