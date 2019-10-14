#pragma once

struct GLFWwindow;

namespace tetrad
{

class Game;
class Screen;
class UIComponent;

class CallbackContext
{
 public:
  CallbackContext() = delete;

  static void Resize_Default(GLFWwindow *, int width, int height);

  static void Cursor_GUI(GLFWwindow *, double currX, double currY);
  static void Cursor_3DCamera(GLFWwindow *, double currX, double currY);

  static void Keyboard_3DCamera(GLFWwindow *, int, int, int, int);

  static void MouseButton_Viewport(GLFWwindow *, int, int, int);

  static void SetScreen(Screen *pScreen);
  static void SetGame(Game *pGame);

  inline static UIComponent *GetCachedUI() { return s_pPrevValidUI; }
  inline static void ClearCachedUI() { s_pPrevUI = s_pPrevValidUI = nullptr; }

 private:
  static Screen *s_pCurrentScreen;
  static Game *s_pCurrentGame;
  static double s_PrevX;
  static double s_PrevY;

  static UIComponent *s_pPrevUI;
  static UIComponent *s_pPrevValidUI;
};

}  // namespace tetrad
