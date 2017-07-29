#include "ScreenPartition.h"

ScreenPartition::ScreenPartition()
{
}

ScreenPartition::~ScreenPartition()
{
}

void ScreenPartition::InformCreated(UIElement *pElem)
{
	m_pElementList.push_front(pElem);
}

void ScreenPartition::InformMoved(UIElement *pElem)
{
	// TODO Place in front of list or remove, depending on
	// whether or not the element is still in this partition
}

void ScreenPartition::InformDeleted(UIElement *pElem)
{
	m_pElementList.remove(pElem);
}
