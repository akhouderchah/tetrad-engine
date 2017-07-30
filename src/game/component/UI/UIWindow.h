#pragma once

#include "UIComponent.h"

class UIWindow : public UIComponent
{
public:
	UIWindow(Entity entity) : UIComponent(entity){}
	~UIWindow(){}

	void OnHoverEnter(){ DEBUG_LOG("Entered window\n"); }
};
