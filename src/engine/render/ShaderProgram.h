#pragma once

#include <string>
#include <vector>

#include "core/GlTypes.h"

namespace tetrad
{

/**
 * @brief Abstraction of the OpenGL shader program.
 *
 * Provides simple interface to add shaders to a program
 * and to compile the final program for use in the DrawSystem.
 */
class ShaderProgram
{
 public:
  ShaderProgram(size_t expectedShaders = 2);

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&&) = delete;
  ShaderProgram& operator=(const ShaderProgram&) = delete;
  ShaderProgram& operator=(ShaderProgram&&) = delete;

  /**
   *@brief - Add a shader source file to the shader program
   */
  void PushShader(GLenum shaderType, std::string shaderPath);

  /**
   * @brief - Remove the most recently added shader program
   */
  void PopShader();

  /**
   * @brief Compile and link shaders, then return the program object
   *
   * @return The program object if no error, else GL_FALSE
   */
  GLuint Compile();

 private:
  std::string GetSource(std::string shaderPath);
  GLuint CompileShader(GLenum shaderType, std::string shaderSource);

 private:
  std::vector<std::pair<GLenum, std::string>> m_Shaders;
};

}  // namespace tetrad
