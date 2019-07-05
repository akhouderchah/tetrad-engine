#pragma once

#include "core/LogSystem.h"
#include "engine/ui/UIComponent.h"

namespace tetrad {

class UIWindow : public UIComponent
{
public:
	UIWindow(Entity entity) : UIComponent(entity){}

	void OnHoverEnter() override{ DEBUG_LOG("Entered window\n"); }
};

}  // namespace tetrad
