#pragma once

#include "core/GlTypes.h"
#include "core/Timer.h"
#include "engine/ecs/System.h"
#include "engine/event/Constants.h"
#include "engine/screen/Screen.h"

namespace tetrad {

enum class MouseMode
{
  NORMAL = GLFW_CURSOR_NORMAL,
  HIDDEN = GLFW_CURSOR_HIDDEN,
  DISABLED = GLFW_CURSOR_DISABLED
};

/** @brief Struct filled by user to initialize game with proper attributes. */
struct GameAttributes
{
  GameAttributes(ScreenAttributes mainWindowAttr,
                 MouseMode mouseMode = MouseMode::NORMAL);

  ScreenAttributes m_MainWindowAttr;
  MouseMode m_MouseMode;
};

/** @brief Highest-level abstraction of a game.
 *
 * Should be used as a base class for specific games, but generally
 * contains window dimensions/attributes, global game data, game systems, etc.
 */
class Game
{
 public:
  Game();
  virtual ~Game() = default;

  /** @brief Initialize the game (starting with the underlying engine).
   *
   * Initialize the component system, create any needed systems, allocate memory,
   * create the needed window(s), etc.
   *
   * A game object can call very few methods before being successfully initialized.
   *
   * @return true unless the initialization fails, in which case the engine will
   * be in an uninitialized state until a successful Initialize occurs.
   */
  bool Initialize(const GameAttributes &attributes);

  /** @brief Shut down the game engine.
   *
   * Move a game object from the initialized state to uninitialized.
   * Only needs to be called for a Game object whose Initialize method returns true.
   */
  void Shutdown();

  /** @brief Run the game loop.
   *
   * This consists mainly of running the game systems.
   */
  void Run();

  /** @brief Pause the game.
   *
   * @note Calling this while a game is paused will not do anything
   *
   * @return true if the game was not initially paused. False otherwise
   */
  bool Pause();

  /** @brief Un-pause the game.
   *
   * @note Calling this while a game is not paused will not do anything
   *
   * @return true if the game was initially paused. False otherwise
   */
  bool Resume();

  /** @brief Set an initialized game object back to its initial state.
   *
   * Acts as if someone called a Shutdown() followed by an Initialize() on
   * the object (but can be implemented more efficiently).
   */
  void Reset();

  inline void SetCurrentState(EGameState state) { m_CurrentState = state; }
  inline EGameState GetCurrentState() const { return m_CurrentState; }

  inline Screen &GetCurrentScreen() { return m_MainScreen; }

 protected:
  /** @brief Add a single system to the system list. */
  inline void AppendSystem(System *pSystem) { m_pSystems.push_back(pSystem); }

  /** @brief Called after base initialization is complete.
   *
   * Can be used by child classes to define additional initialization
   * steps. Note that the system list has been populated by the time this
   * method is called.
   */
  virtual void OnInitialized() = 0;

  /** @brief Overloaded by children to add systems to the system list. */
  virtual void AddSystems() = 0;

  /** @brief Called when the game is successfully paused. */
  virtual void OnPause() = 0;

  /** @brief Called when the game is successfully resumed. */
  virtual void OnResume() = 0;

  Screen m_MainScreen;

 private:
  Timer m_Timer;

  EGameState m_CurrentState;
  EGameState m_PrevState;  // Used to restore state after pausing game.

  deltaTime_t m_DeltaAvg;    // EMWA of tick delta times.
  deltaTime_t m_DeltaAlpha;  // Alpha value for delta time EMWA calculation.

  deltaTime_t m_JitterAvg;
  deltaTime_t m_JitterAlpha;

  std::vector<System *> m_pSystems;
};

}  // namespace tetrad
