#include "tetrad-game/GameplaySystem.h"

#include "engine/game/Game.h"
#include "tetrad-game/obstacle/ObstacleFactoryComponent.h"

namespace tetrad {

void GameplaySystem::Tick(deltaTime_t dt)
{
  if (m_pGame->GetCurrentState() != EGameState::STARTED)
  {
    return;
  }

  // Update ObstacleFactoryComponent time.
  LinkedNode<ObstacleFactoryComponent> *pNode;
  deltaTime_t timeLeft = -dt;
  while ((pNode = ObstacleFactoryComponent::s_TimeList.First()))
  {
    ObstacleFactoryComponent *pComp =
        linked_node_owner(pNode, ObstacleFactoryComponent, m_Node);

    timeLeft = (pComp->m_TimeRemaining += timeLeft);
    if (0 < timeLeft)
    {
      break;
    }

    if (!pComp->GenerateObstacle())
    {
      LOG("ObstacleFactoryComponent failed to generate obstacle!\n");
    }
    // Re-add to the time list at some random point in the future.
    pComp->Disable();
    pComp->Enable();
  }
}

}  // namespace tetrad
