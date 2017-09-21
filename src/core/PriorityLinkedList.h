#pragma once

#include <array>

#include "LinkedList.h"
#include "ErrorSystem.h"

template <typename T, size_t N>
class PriorityLinkedList : public LinkedList<T>
{
	static_assert(N > 0, "A Prioritylinkedlist must have at least 1 priority!");
	using LinkedList<T>::m_Head;

public:
	PriorityLinkedList(){ m_pPriorityTail.fill(&m_Head); }

	void PushBack(LinkedNode<T> &node, size_t priority);
	void Remove(LinkedNode<T> &node, size_t priority);

private:
	void PushFront(LinkedNode<T> &node);
	void PushAfter(LinkedNode<T> &anchorNode, LinkedNode<T> &addNode);
	void PushBack(LinkedNode<T> &node);
	void Remove(LinkedNode<T> &node);

	std::array<LinkedNode<T>*, N> m_pPriorityTail;
};

template <typename T, size_t N>
void PriorityLinkedList<T, N>::PushBack(LinkedNode<T> &node, size_t priority)
{
	DEBUG_ASSERT(priority < N);

	LinkedList<T>::PushAfter(*m_pPriorityTail[priority], node);

	do //Set tails
	{
		m_pPriorityTail[priority++] = &node;
	} while(priority < N && m_pPriorityTail[priority] == &m_Head);
}

template <typename T, size_t N>
void PriorityLinkedList<T, N>::Remove(LinkedNode<T> &node, size_t priority)
{
	DEBUG_ASSERT(priority < N);

	LinkedNode<T> *pPrevTail = node.pPrev;
	while(m_pPriorityTail[priority] == &node)
	{
		m_pPriorityTail[priority++] = pPrevTail;

		if(priority >= N)
		{
			break;
		}
	}

	LinkedList<T>::Remove(node);
}
