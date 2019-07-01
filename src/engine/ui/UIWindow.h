#pragma once

#include "core/LogSystem.h"
#include "engine/ui/UIComponent.h"

class UIWindow : public UIComponent
{
public:
	UIWindow(Entity entity) : UIComponent(entity){}

	void OnHoverEnter() override{ DEBUG_LOG("Entered window\n"); }
};
