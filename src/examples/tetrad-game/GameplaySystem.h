#pragma once

#include "ISystem.h"
#include "ConstVector.h"

/**
 * @brief System that takes care of gameplay functionality
 */
class GameplaySystem : public ISystem
{
public:
	GameplaySystem();

	virtual bool Initialize(Game *pGame);
	virtual void Shutdown();

	virtual void Tick(deltaTime_t dt);

private:
};
