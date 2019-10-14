#pragma once

#include <vector>

#include "core/BaseTypes.h"
#include "core/GlTypes.h"
#include "core/LinkedList.h"
#include "core/Reflection.h"
#include "engine/ecs/IComponent.h"
#include "engine/ui/UIBase.h"

namespace tetrad
{

class MaterialComponent;
class MovableComponent;
class Screen;
class TextComponent;
class TransformComponent;

/**
 * @brief Base class for all UI element classes
 *
 * Provides the interface through which UI elements can be informed of
 * various user actions
 */
COMPONENT()
class UIComponent : public IComponent
{
 public:
  UIComponent(Entity entity);

  void Refresh() override;

  // TODO - should this save the Screen* in order to inform deletion
  // upon destruction?
  virtual void Init(Screen &);
  virtual void Init(Screen &screen, uint8_t priority);

  void SetCurrentTexture(const std::string &tex, TextureType);

  virtual void OnHoverEnter();
  virtual void OnHoverLeave();

  virtual void OnTouchEnter();
  virtual void OnTouchLeave();

  bool IsFollowingCursor() const { return m_bFollowCursor; }
  MovableComponent *GetMover() const { return m_pMover; }

  uint8_t GetPriority() const { return m_Priority; }

  screenBound_t GetScreenBounds() const;

 protected:
  std::vector<UIComponent *> &GetChildren();

 protected:
  friend class Screen;
  friend class ScreenPartition;
  UIRectangleBound_t m_PartitionRectangle;

  friend class DrawSystem;
  GLuint m_CurrTex;

  bool m_IsMovable;
  bool m_bFollowCursor;

  uint8_t m_Priority;

  TransformComponent *m_pTransformComp;
  MovableComponent *m_pMover;
  MaterialComponent *m_pMaterialComp;
  TextComponent *m_pTextComp;

  /** @brief Node of linked list used to render UI elements */
  LinkedNode<UIComponent> m_RenderNode;
};

}  // namespace tetrad
