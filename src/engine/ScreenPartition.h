#pragma once

#include "Base.h"
#include <list>

class UIComponent;
class Screen;

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
	ScreenPartition(Screen*);
	~ScreenPartition();

	void InformCreated(UIComponent*);
	void InformUpdated(UIComponent*);
	void InformDeleted(UIComponent*);

	UIComponent *FindElementAt(double x, double y);

private:
	std::list<UIComponent*> m_pElementList;

	Screen *m_pScreen;
};
