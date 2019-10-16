#pragma once

#include "core/Reflection.h"
#include "engine/ui/UIComponent.h"

namespace tetrad {

class CameraComponent;
class Screen;

/** @brief UI element that acts as a viewport.
 *
 * @note This class diverges from the typical UIComponent child class in that
 * UIViewports are NOT placed in the same ComponentManager as the other
 * UIComponent types. For this UI class specifically, one would want to call
 * EntityManager::GetAll<UIViewport>();
 */
COMPONENT()
class UIViewport : public UIComponent
{
 public:
  UIViewport(Entity entity);

  void Refresh() override;

  void Init(Screen &) override;
  void Init(Screen &screen, uint8_t priority) override;

  inline Screen *GetScreen() { return m_pScreen; }

  void SetCamera(CameraComponent *pCamera) { m_pCamera = pCamera; }
  inline CameraComponent *GetCamera() { return m_pCamera; }
  inline const CameraComponent *GetCamera() const { return m_pCamera; }

 private:
  CameraComponent *m_pCamera;
  Screen *m_pScreen;
};

}  // namespace tetrad
