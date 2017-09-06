#pragma once

#include "Platform.h"

#define linked_node_owner(nodePtr, ownerType, member) \
	((ownerType*)((char*)(nodePtr)-(unsigned long)(&((ownerType*)0)->member)))

/**
 * @brief Struct representing a node of a linked list
 *
 * @note Type of LinkedNode is simply used such that it is more clear
 *       when two nodes do not belong to the same list (i.e. they
 *       are LinkedNodes of different types)
 */
template <typename T>
struct LinkedNode
{
	#ifdef _DEBUG
	LinkedNode() : pNext(nullptr), pPrev(nullptr){}
	LinkedNode(LinkedNode *n, LinkedNode *p) : pNext(n), pPrev(p){}
	#endif

	LinkedNode *pNext;
	LinkedNode *pPrev;
};

/**
 * @brief Circular intrinsic doubly-linked list of LinkedNode<T> nodes
 */
template <typename T>
class LinkedList
{
public:
	LinkedList() : m_Head({&m_Head, &m_Head}){}

	inline LinkedNode<T> *Begin() const{ if(m_Head.pNext == &m_Head){ return nullptr; } return m_Head.pNext; }
	inline LinkedNode<T> *Next(LinkedNode<T> &node) const{ if(node.pNext == &m_Head){ return nullptr; } return node.pNext; }

	void PushFront(LinkedNode<T> &node);
	void PushBack(LinkedNode<T> &node);
	void Remove(LinkedNode<T> &node);

private:
	LinkedNode<T> m_Head;
};

template <typename T>
void LinkedList<T>::PushFront(LinkedNode<T> &node)
{
	DEBUG_ASSERT(node.pNext == nullptr);
	DEBUG_ASSERT(node.pPrev == nullptr);

	node.pNext = m_Head->pNext;
	node.pNext->pPrev = &node;

	node.pPrev = &m_Head;
	m_Head.pNext = &node;
}

template <typename T>
void LinkedList<T>::PushBack(LinkedNode<T> &node)
{
	DEBUG_ASSERT(node.pNext == nullptr);
	DEBUG_ASSERT(node.pPrev == nullptr);

	node.pNext = &m_Head;
	node.pPrev = m_Head.pPrev;

	m_Head.pPrev->pNext = &node;
	m_Head.pPrev = &node;
}

template <typename T>
void LinkedList<T>::Remove(LinkedNode<T> &node)
{
	DEBUG_ASSERT(node.pNext);
	DEBUG_ASSERT(node.pPrev);

	node.pNext->pPrev = node.pPrev;
	node.pPrev->pNext = node.pNext;

#ifdef _DEBUG
	node.pPrev = nullptr;
	node.pNext = nullptr;
#endif
}
