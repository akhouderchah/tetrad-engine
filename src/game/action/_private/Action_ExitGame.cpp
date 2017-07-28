#include "Action_ExitGame.h"

Action_ExitGame::Action_ExitGame(GLFWwindow *pWindow) :
	m_pWindow(pWindow)
{}

bool Action_ExitGame::operator()(EEventAction)
{
	glfwSetWindowShouldClose(m_pWindow, GLFW_TRUE);

	return true;
}
