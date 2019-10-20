/**
 * @file ShaderGlobals.h
 *
 * Provides functionality to load and store shader global variables (known as
 * uniforms in GLSL).
 */

#pragma once

#include "core/GlTypes.h"

namespace tetrad {

#define ELEM_TO_UNIFORM(elem)       "g" #elem
#define ELEM_TO_SHADER_NAME(elem)   m_##elem##Loc
#define ELEM_TO_SHADER_MEMBER(elem) GLuint ELEM_TO_SHADER_NAME(elem);

#define SHADER_BASE(f) \
  f(World)             \
  f(Texture)

#define SHADER_WORLD(f) \
  f(AddColor)           \
  f(MultColor)          \
  f(Time)

#define SHADER_UI(f) \
  f(TopMult)         \
  f(AddColor)        \
  f(MultColor)       \
  f(DitherTexture)

#define SHADER_TEXT(f) f(TextColor)

/** @brief Struct containing the globals for all shaders.
 *
 * @note Can be extended for specific shaders in order to contain more globals
 */
struct BaseShaderGlobals
{
  bool GetLocations(GLuint program);

  SHADER_BASE(ELEM_TO_SHADER_MEMBER)
};

/** @brief Struct containing the globals for the world shader. */
struct WorldShaderGlobals : public BaseShaderGlobals
{
  bool GetLocations(GLuint program);

  SHADER_WORLD(ELEM_TO_SHADER_MEMBER)
};

/** @brief Struct containing the globals for the UI shader. */
struct UIShaderGlobals : public BaseShaderGlobals
{
  bool GetLocations(GLuint program);

  SHADER_UI(ELEM_TO_SHADER_MEMBER)
};

/** @brief Struct containing the globals for the text shader. */
struct TextShaderGlobals : public BaseShaderGlobals
{
  bool GetLocations(GLuint program);

  SHADER_TEXT(ELEM_TO_SHADER_MEMBER)
};

}  // namespace tetrad
