#pragma once

class GLFWwindow;
class Screen;
class UIComponent;

class CallbackContext
{
public:
	static void Resize_Default(GLFWwindow*, int width, int height);

	static void Cursor_GUI(GLFWwindow*, double currX, double currY);
	static void Cursor_3DCamera(GLFWwindow*, double currX, double currY);

	static void MouseButton_Viewport(GLFWwindow*, int, int, int);

	static void SetScreen(Screen *pScreen);

private:
	CallbackContext();

	static Screen *s_pCurrentScreen;
	static double s_PrevX;
	static double s_PrevY;

	static UIComponent *s_pPrevUI;
	static UIComponent *s_pPrevValidUI;
};
