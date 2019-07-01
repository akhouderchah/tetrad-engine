#pragma once

#include "core/LogSystem.h"
#include "engine/ui/UIComponent.h"

class UIWindow : public UIComponent
{
public:
	UIWindow(Entity entity) : UIComponent(entity){}
	~UIWindow(){}

	void OnHoverEnter(){ DEBUG_LOG("Entered window\n"); }
};
