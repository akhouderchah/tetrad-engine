#include "engine/event/EventQueue.h"

#include "engine/event/Constants.h"

namespace tetrad
{

//
// One of these 3 should be uncommented in order to define the EventQueue's
// overflow behavior. By default (i.e. if none of these are defined), the
// OVERFLOW_IGNORE behavior--which is to ignore the event trying to be pushed
// onto the queue--will be chosen.
//
//#define OVERFLOW_REPLACE_OLDEST
//#define OVERFLOW_REPLACE_YOUNGEST
#define OVERFLOW_IGNORE
//

EventQueue::EventQueue() : m_ReadIndex(1), m_WriteIndex(1)
{
  m_Events[0].event = EGE_END;
}

Event& EventQueue::Consume()
{
  // If done reading, index = 0, else index = m_ReadIndex
  bool read = (m_ReadIndex - m_WriteIndex != 0);
  size_t index = m_ReadIndex - m_ReadIndex * (!read);

  // If we read, increment m_ReadIndex
  m_ReadIndex += read;
  // Make sure m_ReadIndex is between 1 and N+1
  m_ReadIndex -= (m_ReadIndex / (N + 1)) * N;

  // Because write index will be > N+1 if we push N elements
  m_WriteIndex -= (m_WriteIndex / (N + 1)) * N;

  return m_Events[index];
}

bool EventQueue::PushEvent(const Event& event)
{
  // Overflow behavior
  if (m_WriteIndex > N)
  {
#if defined(OVERFLOW_REPLACE_OLDEST)
    // "de-elevate" write index
    m_WriteIndex -= N;

    // Push read index forward (as we will over-write the oldest element)
    ++m_ReadIndex;
    m_ReadIndex -= (m_ReadIndex / (N + 1)) * N;
#elif defined(OVERFLOW_REPLACE_YOUNGEST)
    m_WriteIndex -= N + 1;

    // If write index is 0, move to N
    m_WriteIndex = (!!m_WriteIndex) * m_WriteIndex + (!m_WriteIndex) * N;
#else  // OVERFLOW_IGNORE
    return false;
#endif
  }

  // Calculate next write index
  size_t nextIndex = m_WriteIndex + 1;
  nextIndex -= (nextIndex / (N + 1)) * N;

  // Push element here
  m_Events[m_WriteIndex] = event;

  bool last = (nextIndex == m_ReadIndex);

  // "Elevate" the write index if we're pushing to the last element in the queue
  m_WriteIndex = nextIndex + last * N;

  return true;
}

}  // namespace tetrad
