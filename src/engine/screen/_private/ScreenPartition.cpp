#include "ScreenPartition.h"
#include "UIComponent.h"
#include "Screen.h"

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
		screenBound_t &&screenBound = pElem->GetScreenBounds();
		if(screenBound.points[0].X <= x && x <= screenBound.points[1].X)
		{
			if(screenBound.points[0].Y <= y && y <= screenBound.points[1].Y)
			{
				return pElem;
			}
		}
	}

	return nullptr;
}
