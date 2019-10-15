#pragma once

#include <array>

#include "core/ErrorSystem.h"
#include "core/LinkedList.h"

namespace tetrad {

template <typename T, size_t N>
class PriorityLinkedList : public LinkedList<T>
{
  static_assert(N > 0, "A Prioritylinkedlist must have at least 1 priority!");

 public:
  PriorityLinkedList() {}

  bool Initialize()
  {
    m_pPriorityTail.fill(&this->m_Head);
#ifdef _DEBUG
    m_Initialized = true;
#endif
    return true;
  }

  inline LinkedNode<T> *First(size_t priority = 0) const
  {
    DEBUG_ASSERT(priority < N);
    DEBUG_ASSERT(m_Initialized);
    if (priority == 0)
    {
      return this->HeadToNull(this->m_Head.pNext);
    }
    return this->HeadToNull(m_pPriorityTail[priority - 1]->pNext);
  }

  inline LinkedNode<T> *Last(size_t priority = N - 1) const
  {
    DEBUG_ASSERT(priority < N);
    DEBUG_ASSERT(m_Initialized);
    return this->HeadToNull(m_pPriorityTail[priority]);
  }

  void PushBack(LinkedNode<T> &node, size_t priority);
  void Remove(LinkedNode<T> &node, size_t priority);

 private:
  void PushFront(LinkedNode<T> &node);
  void PushAfter(LinkedNode<T> &anchorNode, LinkedNode<T> &addNode);
  void PushBack(LinkedNode<T> &node);
  void Remove(LinkedNode<T> &node);

  std::array<LinkedNode<T> *, N> m_pPriorityTail;

#ifdef _DEBUG
  bool m_Initialized = false;
#endif
};

template <typename T, size_t N>
void PriorityLinkedList<T, N>::PushBack(LinkedNode<T> &node, size_t priority)
{
  DEBUG_ASSERT(priority < N);
  DEBUG_ASSERT(m_Initialized);

  LinkedList<T>::PushAfter(*m_pPriorityTail[priority], node);
  DEBUG_ASSERT(m_pPriorityTail[priority]->pNext == &node);

  auto prevTail = m_pPriorityTail[priority];
  do  // Set tails
  {
    m_pPriorityTail[priority++] = &node;
  } while (priority < N && m_pPriorityTail[priority] == prevTail);
}

template <typename T, size_t N>
void PriorityLinkedList<T, N>::Remove(LinkedNode<T> &node, size_t priority)
{
  DEBUG_ASSERT(priority < N);
  DEBUG_ASSERT(m_Initialized);

  LinkedNode<T> *pPrevTail = node.pPrev;
  while (m_pPriorityTail[priority] == &node)
  {
    m_pPriorityTail[priority++] = pPrevTail;

    if (priority >= N)
    {
      break;
    }
  }

  LinkedList<T>::Remove(node);
}

}  // namespace tetrad
