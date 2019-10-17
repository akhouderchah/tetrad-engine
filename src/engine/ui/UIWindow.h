#pragma once

#include "core/Log.h"
#include "core/Reflection.h"
#include "engine/ui/UIComponent.h"

namespace tetrad {

COMPONENT()
class UIWindow : public UIComponent
{
 public:
  UIWindow(Entity entity) : UIComponent(entity) {}

  void OnHoverEnter() override { LOG_DEBUG("Entered window\n"); }
};

}  // namespace tetrad
