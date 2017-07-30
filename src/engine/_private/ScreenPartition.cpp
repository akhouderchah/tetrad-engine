#include "ScreenPartition.h"
#include "UIComponent.h"

ScreenPartition::ScreenPartition()
{
}

ScreenPartition::~ScreenPartition()
{
}

void ScreenPartition::InformCreated(UIComponent *pElem)
{
	m_pElementList.push_front(pElem);
}

void ScreenPartition::InformUpdated(UIComponent *pElem)
{
	m_pElementList.remove(pElem);
	m_pElementList.push_front(pElem);
}

void ScreenPartition::InformDeleted(UIComponent *pElem)
{
	m_pElementList.remove(pElem);
}

UIComponent *ScreenPartition::FindElementAt(double x, double y)
{
	for(UIComponent *pElem : m_pElementList)
	{
		if(pElem->IsAt(x, y))
		{
			return pElem;
		}
	}

	return nullptr;
}
