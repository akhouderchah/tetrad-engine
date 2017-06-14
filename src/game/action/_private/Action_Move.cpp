#include "Action_Move.h"
#include "MovableComponent.h"

Action_Move::Action_Move(Entity entity, EMoveDirection direction) :
	m_Entity(entity), m_Direction(direction)
{
	int neg = 1 - 2 * (direction & 1);
	m_MoveVec[0] = neg * !(direction & 2);
	m_MoveVec[1] = 0;
	m_MoveVec[2] = neg * !!(direction & 2);
}

Action_Move::~Action_Move()
{
}

bool Action_Move::operator()()
{
	MovableComponent *pMove = m_Entity.GetAs<MovableComponent>();
	if(!pMove)
	{
		DEBUG_LOG("Entity " << static_cast<ObjHandle>(m_Entity).GetID() << " has no MovableComponent, and thus can't be moved\n");
		return false;
	}
	DEBUG_LOG("Test move: " << m_MoveVec[0] << ", " << m_MoveVec[1] << ", " << m_MoveVec[2] << "\n");
	pMove->Move(m_MoveVec, EMoveType::LOCAL);
	return true;
}

