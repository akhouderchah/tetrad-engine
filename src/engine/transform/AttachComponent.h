#pragma once

#include "core/Reflection.h"
#include "engine/ecs/IComponent.h"

namespace tetrad {

class TransformComponent;
class UIComponent;

/** @brief Component to attach this entity to another.
 *
 * @note Requires the parent entity to have a TransformComponent
 */
COMPONENT()
class AttachComponent : public IComponent
{
 public:
  AttachComponent(Entity entity);
  ~AttachComponent();

  void Refresh() override;

  bool Attach(Entity entity);
  void Unattach();

 private:
  void RefreshParent(TransformComponent *pParent);
  void RemoveUI();

 private:
  Entity m_AttachEntity;

  TransformComponent *m_pOwnedTransform;
  UIComponent *m_pUIComp;

  friend class TransformComponent;
};

}  // namespace tetrad
