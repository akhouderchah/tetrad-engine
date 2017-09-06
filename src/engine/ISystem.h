#pragma once

#include "IComponent.h"
#include "ComponentManager.h"

#include <cstddef>

class Screen;

/**
 * @brief Interface for all systems
 */
class ISystem
{
public:
	ISystem() : m_pGame(nullptr){}
	virtual ~ISystem(){};
	virtual bool Initialize(Game *pGame){ m_pGame = pGame; return true; }
	virtual void Shutdown() = 0;

	virtual void Tick(deltaTime_t dt) = 0;

	/**
	 * @brief Used to allow systems interact with the window if needed.
	 *
	 * @see Game::Initialize
	 */
	static void SetScreen(Screen *pScreen){ s_pScreen = pScreen; }

protected:
	static Screen *s_pScreen;
	Game *m_pGame;
};

