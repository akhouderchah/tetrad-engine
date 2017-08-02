#pragma once

#include "Base.h"
#include <list>

class UIComponent;

/**
 * @brief Subdivision of a screen
 *
 * ScreenPartitions are collected in the screen class.
 * At the moment, used for the UI system only, but could
 * be extended for other purposes.
 */
class ScreenPartition
{
public:
	ScreenPartition();
	~ScreenPartition();

	void InformCreated(UIComponent*);
	void InformUpdated(UIComponent*);
	void InformDeleted(UIComponent*);

	UIComponent *FindElementAt(double x, double y);

private:
	std::list<UIComponent*> m_pElementList;
};
