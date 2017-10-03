#pragma once

#include "Base.h"

#include <vector>

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
	~ShaderProgram();

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

	// Don't copy programs
	ShaderProgram(const ShaderProgram&);
	ShaderProgram& operator=(const ShaderProgram&);
private:
	std::vector<std::pair<GLenum,std::string>> m_Shaders;
};

