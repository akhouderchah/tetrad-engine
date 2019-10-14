#include "tetrad-game/GameplaySystem.h"

#include "engine/ecs/EntityManager.h"
#include "engine/game/Game.h"
#include "tetrad-game/obstacle/ObstacleFactoryComponent.h"

namespace tetrad
{

void GameplaySystem::Tick(deltaTime_t dt)
{
  if (m_pGame->GetCurrentState() == EGameState::STARTED)
  {
    // Update ObstacleFactoryComponent time
    LinkedNode<ObstacleFactoryComponent> *pNode;

    deltaTime_t timeLeft = -dt;
    while ((pNode = ObstacleFactoryComponent::s_TimeList.First()))
    {
      ObstacleFactoryComponent *pComp =
          linked_node_owner(pNode, ObstacleFactoryComponent, m_Node);

      timeLeft = (pComp->m_TimeRemaining += timeLeft);
      if (timeLeft <= 0)
      {
        if (!pComp->GenerateObstacle())
        {
          LOG("ObstacleFactoryComponent failed to generate obstacle!\n");
        }
        pComp->Disable();
        pComp->Enable();
      }
      else
      {
        break;
      }
    }
  }
}

}  // namespace tetrad
