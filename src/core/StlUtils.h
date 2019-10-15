#pragma once

#include <array>
#include <utility>

namespace tetrad {

// Create a std::array from a set of values without manually specifying the size
// of the array. Note that unlike the make_array likely to make its way into
// C++20, this function always requires the user to specify ElementType. This
// is done so that users are not surprised by the element type of resulting
// arrays when std::common_type is used.
template <typename ElementType, typename... T>
constexpr auto make_array(T&&... values)
{
  return std::array<ElementType, sizeof...(T)>{
      static_cast<ElementType>(std::forward<T>(values))...};
}

}  // namespace tetrad
