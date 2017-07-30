#include "ScreenPartition.h"
#include "UIElement.h"

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

void ScreenPartition::InformUpdated(UIElement *pElem)
{
	m_pElementList.remove(pElem);
	m_pElementList.push_front(pElem);
}

void ScreenPartition::InformDeleted(UIElement *pElem)
{
	m_pElementList.remove(pElem);
}

UIElement *ScreenPartition::FindElementAt(double x, double y)
{
	for(UIElement *pElem : m_pElementList)
	{
		if(pElem->isAt(x, y))
		{
			return pElem;
		}
	}

	return nullptr;
}
