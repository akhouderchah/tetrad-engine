#pragma once

#include <vector>

namespace tetrad {

/**
 * @brief Redefines a limited interface with the std::vector class.
 *
 * Conceptually, this defines a constant vector that may or may not
 * contain const elements. That is, the vector itself can't be changed
 * (no adding or removing elements, etc), but elements can be accessed
 * if they are not also of a const type.
 */
template <typename T, typename U = std::allocator<T>>
class ConstVector
{
 public:
  explicit ConstVector(std::vector<T, U>& vector) : m_pVector(&vector) {}

  T& at(std::size_t index)
  {
    assert(m_pVector);
    return m_pVector->at(index);
  }
  T& operator[](std::size_t index) { return (*m_pVector)[index]; }

  std::size_t size() const { return m_pVector->size(); }

 private:
  template <typename X, typename Y>
  operator std::vector<X, Y>();  // User cannot cast back to a regular vector

 private:
  std::vector<T, U>* m_pVector;
};

}  // namespace tetrad
