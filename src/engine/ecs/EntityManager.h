#pragma once

#include <queue>
#include <unordered_map>
#include <vector>

#include "core/BaseTypes.h"
#include "engine/ecs/ComponentManager.h"
#include "engine/ecs/Entity.h"

namespace tetrad
{

class UIComponent;
class UIViewport;

template <typename T>
ObjectHandle::type_t GetComponentType(UIViewport *)
{
  return ComponentManager<UIViewport>::GetType();
}

template <typename T>
ObjectHandle::type_t GetComponentType(UIComponent *)
{
  return ComponentManager<UIComponent>::GetType();
}

template <typename T>
ObjectHandle::type_t GetComponentType(void *)
{
  return ComponentManager<T>::GetType();
}

/**
 * @brief Handles all the Entity-related functionality (creation,
 * deletion, adding components, etc)
 *
 * Entity 0 is created at initialization, and serves as the null entity.
 * Entity 0 will always fail to add or get components (returning nullptr).
 * Neither DestroyEntity nor DestroyAll will remove the null entity element.
 */
class EntityManager
{
 public:
  /**
   * @brief Initialization function for the entity manager.
   *
   * MUST be called before any other EntityManager calls are made.
   */
  static void Initialize();

  /**
   * @brief Shutdown function for the entity manager.
   *
   * @note The EntityManager cannot be restarted or used after calling
   * this function. Therefore this should only occur on engine destruction
   * (or on an otherwise permanent engine shutdown). If one simply wants to
   * "reset" the EntityManager, call DestroyAll().
   */
  static void Shutdown();
  static bool InShutdown() { return s_InShutdown; }

  static Entity CreateEntity();
  static void DestroyEntity(Entity entity);
  static void DestroyAll();

  template <class T>
  static T *AddComponent(Entity entity, bool skipRefresh = false);
  template <class T>
  static T *GetComponent(Entity entity);

  /**
   * @brief Returns a ConstVector from the corresponding ComponentManager.
   *
   * @note Since all children of UIComponent are stored in the ComponentManager
   * for UIComponent, this method will NOT work for UIComponent children
   * types (use UIComponent instead).
   */
  template <class T>
  static ConstVector<T *> GetAll();
  template <class T>
  static bool HasComponent(Entity entity);
  template <class T>
  static void RemoveComponent(Entity entity, bool skipRefresh = false);

  static IComponent *GetComponent(Entity entity, ObjectHandle::type_t type);
  static bool HasComponent(Entity entity, ObjectHandle::type_t type);
  static void RemoveComponent(Entity entity, ObjectHandle::type_t type,
                              bool skipRefresh = false);

 private:
  // No need to have EntityManager instances (at least for this project)
  //	EntityManager();

  static IComponent *AddComponent(Entity entity, ObjectHandle::type_t, IComponent *pComp,
                                  bool skipRefresh = false);
  static void AddEntities(size_t chunkSize = EntityManager::CHUNK_SIZE);

 private:
  friend class GUID<IComponentManager, ObjectHandle::type_t>;
  static std::vector<IComponentManager *> s_pComponentManagers;

  typedef std::vector<std::pair<ObjectHandle::type_t, ObjectHandle::ID_t>> compList_t;
  static std::vector<std::pair<ObjectHandle::version_t, compList_t>> s_EntityList;
  static std::queue<ObjectHandle::ID_t> s_FreeList;

  /** @brief Maps ID:type -> component index */
  static std::unordered_map<ObjectHandle::handle_t, ObjectHandle::ID_t> s_HandletoIndex;

  /** @brief Default amount added to s_EntityList when more space is needed */
  static const size_t CHUNK_SIZE;
  static bool s_InShutdown;
};

template <typename T>
T *EntityManager::AddComponent(Entity entity, bool skipRefresh)
{
  T *pComp = new T(entity);
  ObjectHandle::type_t type = GetComponentType<T>((T *)0);

  return (T *)AddComponent(entity, type, pComp, skipRefresh);
}

template <typename T>
T *EntityManager::GetComponent(Entity entity)
{
  ObjectHandle::type_t type = GetComponentType<T>((T *)0);
  return (T *)GetComponent(entity, type);
}

template <typename T>
ConstVector<T *> EntityManager::GetAll()
{
  ObjectHandle::type_t type = GetComponentType<T>((T *)0);
  return ((ComponentManager<T> *)s_pComponentManagers[type])->GetAll();
}

template <typename T>
bool EntityManager::HasComponent(Entity entity)
{
  return HasComponent(entity, GetComponentType<T>((T *)0));
}

template <typename T>
void EntityManager::RemoveComponent(Entity entity, bool skipRefresh)
{
  return RemoveComponent(entity, GetComponentType<T>((T *)0), skipRefresh);
}

template <typename T>
T *Entity::GetAs()
{
  return EntityManager::GetComponent<T>(*this);
}

template <typename T>
T *Entity::Add()
{
  return EntityManager::AddComponent<T>(*this);
}

}  // namespace tetrad
