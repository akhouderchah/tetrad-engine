#include "PhysicsSystem.h"
#include "EntityManager.h"
#include "Game.h"

PhysicsSystem::PhysicsSystem() :
	m_pPhysicsComponents(EntityManager::GetAll<PhysicsComponent>())
{
}

bool PhysicsSystem::Initialize(Game *pGame)
{
	return ISystem::Initialize(pGame);
}

void PhysicsSystem::Shutdown()
{
}

void PhysicsSystem::Tick(deltaTime_t dt)
{
	if(m_pGame->GetCurrentState() != EGameState::STARTED)
	{
		return;
	}

	for(size_t i = 1; i < m_pPhysicsComponents.size(); ++i)
	{
		m_pPhysicsComponents[i]->Tick(dt);
	}
}

