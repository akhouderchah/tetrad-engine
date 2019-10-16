#pragma once

#include "engine/ecs/Entity.h"

namespace tetrad {

/** @brief Base class for all components. */
class IComponent
{
 public:
  virtual ~IComponent() {}

  /** @brief Method to refresh ptrs to other components.
   *
   * In order to keep the entity system efficent, components will cache
   * pointers to components they need. This method provides the entity
   * manager with the ability to clear these cached pointers as needed
   * and force components to use the EntityManager to re-acquire other
   * component pointers.
   */
  virtual void Refresh() = 0;

  /** @brief Shutdown method for component.
   *
   * Called by the EntityManager right before the component is destroyed.
   * Can be overridden in order to provide extra behavior before destruction.
   */
  virtual void OnDestroy() {}

  /** @brief Method to get the Entity associated with this component. */
  Entity GetEntity() const { return m_Entity; }

  /** @brief Method to retrieve the ID portion of the associated entity.
   *
   * Only needed by the underlying entity system.
   */
  ObjectHandle::ID_t GetID() const { return ((ObjectHandle)m_Entity).GetID(); }

 protected:
  IComponent(Entity entity) : m_Entity(entity) {}

  Entity m_Entity;
};

}  // namespace tetrad
