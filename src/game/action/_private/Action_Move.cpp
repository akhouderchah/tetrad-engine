#include "Action_Move.h"
#include "PhysicsComponent.h"

Action_Move::Action_Move(Entity entity, EMoveDirection direction) :
	m_Entity(entity), m_Direction(direction)
{
}

Action_Move::~Action_Move()
{
}

bool Action_Move::operator()(EEventAction action)
{
	PhysicsComponent *pPhys = m_Entity.GetAs<PhysicsComponent>();
	if(!pPhys)
	{
		DEBUG_LOG("Entity " << static_cast<ObjHandle>(m_Entity).GetID() << " has no PhysicsComponent, and thus can't be moved\n");
		return false;
	}
	pPhys->UpdateMovement(m_Direction, (bool)action);

	return true;
}

