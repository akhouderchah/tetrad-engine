#pragma once

#include "core/ObjectHandle.h"

namespace tetrad
{

class IComponent;

/**
 * @brief Base interface for all component managers.
 *
 * Since there can be an arbitrary number of component managers
 * generated at compile-time, this base class makes it possible
 * to hold a list of what component managers exist.
 */
class IComponentManager
{
 public:
  IComponentManager(){};
  virtual ~IComponentManager(){};

  virtual ObjectHandle::ID_t Add(IComponent *pComponent) = 0;

  virtual ObjectHandle::ID_t Delete(ObjectHandle::ID_t index) = 0;
  virtual void DeleteAll() = 0;

  virtual IComponent *Get(ObjectHandle::ID_t index) const = 0;
};

}  // namespace tetrad
