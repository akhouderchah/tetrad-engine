#pragma once

#include "Base.h"
#include "ISystem.h"
#include "Timer.h"
#include "Screen.h"
#include "engine/event/Constants.h"

enum class MouseMode
{
	NORMAL = GLFW_CURSOR_NORMAL,
	HIDDEN = GLFW_CURSOR_HIDDEN,
	DISABLED = GLFW_CURSOR_DISABLED
};

/**
 * @brief Struct filled by user to initialize the game engine with proper attributes
 */
struct GameAttributes
{
	GameAttributes(
		ScreenAttributes mainWindowAttr,
		MouseMode mouseMode = MouseMode::NORMAL
		);

	ScreenAttributes m_MainWindowAttr;
	MouseMode m_MouseMode;
};

/**
 * @brief Highest-level abstraction of a game
 *
 * Should be used as a base class for specific games, but generally
 * contains window dimensions/attributes, global game data, game systems, etc.
 */
class Game
{
public:
	Game();
	~Game();

	/**
	 * @brief Initialize the game (starting with the underlying engine)
	 *
	 * Initializes the component system, creates any needed systems, allocates memory,
	 * creates the needed window(s), etc.
	 *
	 * A game object can call very few methods before being successfully initialized.
	 *
	 * @return true unless the initialization fails, in which case the engine will
	 * be in an uninitialized state until a successful Initialize occurs.
	 */
	virtual bool Initialize(const GameAttributes& attributes);

	/**
	 * @brief Shuts down the game engine
	 *
	 * Moves a game object from the initialized state to uninitialized.
	 * Only needs to be called for a Game object whose Initialize method returns true.
	 */
	virtual void Shutdown();

	/**
	 * @brief Runs the game loop, which consists mainly of running systems as needed
	 */
	virtual void Run();

	/**
	 * @brief Pauses the game
	 *
	 * @note Calling this while a game is paused will not do anything
	 *
	 * @return true if the game was not initially paused. False otherwise
	 */
	virtual bool Pause();

	/**
	 * @brief Un-pauses the game
	 *
	 * @note Calling this while a game is not paused will not do anything
	 *
	 * @return true if the game was initially paused. False otherwise
	 */
	virtual bool Unpause();

	/**
	 * @brief Sets an initialized game object back to its initial state
	 *
	 * Acts as if someone called a Shutdown() followed by an Initialize() on
	 * the object (but can be implemented more efficiently if needed).
	 */
	virtual void Reset();

	void SetCurrentState(EGameState state){ m_CurrentState = state; }
	inline EGameState GetCurrentState() const{ return m_CurrentState; }

	inline Screen &GetCurrentScreen(){ return m_MainScreen; }

protected:
	/**
	 * @brief Overloaded by children to add all systems to m_pSystems
	 */
	virtual void AddSystems() = 0;

protected:
	Timer m_Timer;
	Screen m_MainScreen;

	EGameState m_CurrentState;
	EGameState m_PrevState; // Used to restore state after pausing game

	deltaTime_t m_DeltaAvg; // EMWA of tick delta times
	deltaTime_t m_DeltaAlpha; // alpha value for delta time EMWA calculation

	deltaTime_t m_JitterAvg;
	deltaTime_t m_JitterAlpha;

	std::vector<ISystem*> m_pSystems;
};

