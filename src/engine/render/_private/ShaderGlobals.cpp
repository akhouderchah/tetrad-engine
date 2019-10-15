#include "engine/render/ShaderGlobals.h"

#include "core/ErrorSystem.h"

namespace tetrad {

#define ELEM_TO_GET_LOC(elem)                                                       \
  ELEM_TO_SHADER_NAME(elem) = glGetUniformLocation(program, ELEM_TO_UNIFORM(elem)); \
  if (ELEM_TO_SHADER_NAME(elem) == 0xFFFFFFFF)                                      \
  {                                                                                 \
    LOG_ERROR("Failed to get location of uniform: '"                                \
              << ELEM_TO_UNIFORM(elem) << "' in program: " << program << "\n");     \
    return false;                                                                   \
  }

bool BaseShaderGlobals::GetLocations(GLuint program)
{
  // Get locations of SHADER_BASE uniforms
  SHADER_BASE(ELEM_TO_GET_LOC)

  return true;
}

bool WorldShaderGlobals::GetLocations(GLuint program)
{
  if (!BaseShaderGlobals::GetLocations(program))
  {
    return false;
  }

  SHADER_WORLD(ELEM_TO_GET_LOC)

  return true;
}

bool UIShaderGlobals::GetLocations(GLuint program)
{
  if (!BaseShaderGlobals::GetLocations(program))
  {
    return false;
  }

  SHADER_UI(ELEM_TO_GET_LOC)

  return true;
}

bool TextShaderGlobals::GetLocations(GLuint program)
{
  if (!BaseShaderGlobals::GetLocations(program))
  {
    return false;
  }

  SHADER_TEXT(ELEM_TO_GET_LOC)

  return true;
}

}  // namespace tetrad
