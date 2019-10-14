#pragma once

#include <cstdint>

#include "glm/glm.hpp"

namespace tetrad
{

typedef float deltaTime_t;

/**
 * @brief Defines a region bounded by a rectangle
 *
 * @note The actual interpretation of these values depend on the user. In the
 * UI system, the origin is in the top-left corner of the screen, and
 * (startX, startY) defines the top-left point of the bound.
 */
template <typename T, typename S>
union RectangularBound {
  struct
  {
    T X;
    T Y;
  } points[2];
  S value;

  RectangularBound(T sX, T sY, T eX, T eY)
  {
    points[0] = {sX, sY};
    points[1] = {eX, eY};
  }
};

typedef RectangularBound<float, float> screenBound_t;

enum class ShapeType : uint8_t
{
  PLANE,
  CUBE
};

enum class TextureType : uint8_t
{
  RGB,
  RGBA
};

}  // namespace tetrad
