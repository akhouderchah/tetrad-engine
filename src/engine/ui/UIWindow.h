#pragma once

#include "core/LogSystem.h"
#include "core/Reflection.h"
#include "engine/ui/UIComponent.h"

namespace tetrad {

COMPONENT()
class UIWindow : public UIComponent
{
 public:
  UIWindow(Entity entity) : UIComponent(entity) {}

  void OnHoverEnter() override { DEBUG_LOG("Entered window\n"); }
};

}  // namespace tetrad
