#pragma once

#include "Base.h"
#include <list>

class UIElement;

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

	void InformCreated(UIElement*);
	void InformMoved(UIElement*);
	void InformDeleted(UIElement*);

	UIElement *FindElementAt(double x, double y);

private:
	std::list<UIElement*> m_pElementList;
};
