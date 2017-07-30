#pragma once

#include "UIComponent.h"

class UIButton : public UIComponent
{
public:
	UIButton(Entity entity) : UIComponent(entity){}
	~UIButton(){}

	void OnHoverEnter(){ DEBUG_LOG("Hovered button\n"); }
};
