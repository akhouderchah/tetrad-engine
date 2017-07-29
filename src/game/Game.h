#pragma once

#include "Base.h"
#include "DrawSystem.h"
#include "ISystem.h"
#include "Timer.h"
#include "Screen.h"

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
		int32_t width = 1024,
		int32_t height = 768,
		std::string title = "Test Game",
		bool fullscreen = false,
		bool isResizable = false,
		MouseMode mouseMode = MouseMode::NORMAL,
		uint8_t samples = 4,
		uint16_t screenPartitionRows = 4,
		uint16_t screenPartitionCols = 4
		);

	uint32_t m_Width, m_Height;
	std::string m_WindowTitle;

	bool m_Fullscreen;
	bool m_IsResizable;

	MouseMode m_MouseMode;

	uint8_t m_SampleCount;

	uint16_t m_ScreenPartitionRows;
	uint16_t m_ScreenPartitionCols;
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
	 * @brief Sets an initialized game object back to its initial state
	 *
	 * Acts as if someone called a Shutdown() followed by an Initialize() on
	 * the object (but can be implemented more efficiently if needed).
	 */
	virtual void Reset();

protected:
	bool CreatePrimaryWindow(const GameAttributes& attributes);

	/**
	 * @brief Overloaded by children to add all systems to m_pSystems
	 */
	virtual void AddSystems() = 0;

protected:
	Random m_Random;
	Timer m_Timer;
	Screen *m_pScreen;
	GLFWwindow *m_pWindow;

	deltaTime_t m_AvgDelta; // EMWA of tick delta times
	deltaTime_t m_AvgDeltaAlpha; // alpha value for delta time EMWA calculation

	std::vector<ISystem*> m_pSystems;
};

