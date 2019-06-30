#pragma once

#include "Base.h"
#include "PriorityLinkedList.h"
#include "ui/UIBase.h"
#include <list>

class Screen;
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
	/**
	 * @brief Finds the node in m_SearchList containing the UIComponent*
	 *
	 * TODO - ideally, this should be placed in LinkedList<T>, but this
	 * requires knowledge of the ExternalLinkedList classes. Perhaps a new
	 * list class is in order?
	 */
	LinkedNode<UIComponent> *FindNode(UIComponent *);

private:
	friend class Screen;
	PriorityLinkedList<UIComponent, UI_PRIORITY_COUNT> m_SearchList;
};
