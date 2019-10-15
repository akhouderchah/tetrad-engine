#include "engine/ecs/EntityManager.h"

#include "engine/ecs/IComponent.h"

using namespace std;

namespace tetrad {

/**
 * Static Definitions
 */
vector<IComponentManager *> EntityManager::s_pComponentManagers;
vector<pair<ObjectHandle::version_t, EntityManager::compList_t>>
    EntityManager::s_EntityList;
queue<ObjectHandle::ID_t> EntityManager::s_FreeList;
unordered_map<ObjectHandle::handle_t, ObjectHandle::ID_t> EntityManager::s_HandletoIndex;
bool EntityManager::s_InShutdown = false;

const size_t EntityManager::CHUNK_SIZE = 64;

/**
 * Function Definitions
 */
void EntityManager::Initialize()
{
  s_EntityList.push_back(make_pair(0, compList_t()));  // Create the null entity
  AddEntities();  // @TODO - maybe add more than CHUNK_SIZE entities for init?

  //
  // Note we don't fill s_pComponentManagers here.
  // The ComponentManagers place themselves into the vector
  // during static initialization (ordering is irrelevant).
  //
}

void EntityManager::Shutdown()
{
  s_InShutdown = true;
  DestroyAll();
  s_EntityList.clear();
  s_HandletoIndex.clear();
  queue<ObjectHandle::ID_t>().swap(s_FreeList);

  size_t i = s_pComponentManagers.size();
  while (i != 0)
  {
    // This step prevents us from restarting an EntityManager
    delete s_pComponentManagers[--i];
    s_pComponentManagers.pop_back();
  }
  s_pComponentManagers.clear();
  s_InShutdown = false;
}

Entity EntityManager::CreateEntity()
{
  // Fill freeList as needed
  if (s_FreeList.size() <= 1)
  {
    AddEntities();

    if (s_FreeList.size() == 0)
    {
      LOG("Failed to allocate space for a new entity! Perhaps remove some existing "
          "entities or expand the ObjectHandle sizes.\n");
      return Entity(ObjectHandle::null);
    }
  }

  // Take from freeList
  ObjectHandle::ID_t ID = s_FreeList.front();
  s_FreeList.pop();
  DEBUG_ASSERT(ID);

  ObjectHandle::version_t version = s_EntityList[ID].first;

  return Entity(ObjectHandle::constructHandle(ID, 0, version));
}

void EntityManager::DestroyEntity(Entity entity)
{
  ObjectHandle::ID_t ID = entity.m_ID.GetID();

  // Validity tests
  if (ID == 0 || ID >= s_EntityList.size() ||
      entity.m_ID.GetVersion() != s_EntityList[ID].first)
  {
    return;
  }

  // Go through the compList and remove all elements
  for (size_t i = s_EntityList[ID].second.size(); i-- > 0;)
  {
    RemoveComponent(entity, s_EntityList[ID].second[i].first, true);
  }
  s_EntityList[ID].second.clear();

  // Increase entity version number
  ++s_EntityList[ID].first;

  // Add to free list
  s_FreeList.push(ID);
}

void EntityManager::DestroyAll()
{
  //
  // Clear the freeList, then destroy all entities other than the null entity.
  // Note that deleting a freed entity in this scenario is not harmful.
  // A double free is only dangerous because there would be multiple freeList
  // entries for a single entity.
  //
  queue<ObjectHandle::ID_t>().swap(s_FreeList);

  for (size_t i = 0; i < s_pComponentManagers.size(); ++i)
  {
    s_pComponentManagers[i]->DeleteAll();
  }

  auto first = s_EntityList[0];
  s_EntityList.clear();
  s_EntityList.push_back(first);

  /*
  for(size_t i = 1; i < s_EntityList.size(); ++i)
  {
          ObjectHandle::version_t version = s_EntityList[i].first;
          DestroyEntity(Entity(ObjectHandle::constructHandle(i, 0, version)));
  }
  */
}

IComponent *EntityManager::AddComponent(Entity entity, ObjectHandle::type_t type,
                                        IComponent *pComp, bool skipRefresh)
{
  ObjectHandle::ID_t index = entity.m_ID.GetID();

  // Ensure the index and version are valid
  if (index == 0 || index >= s_EntityList.size() ||
      entity.m_ID.GetVersion() != s_EntityList[index].first)
  {
    return s_pComponentManagers[type]->Get(0);
  }

  // Make sure we don't already have a component of this type
  ObjectHandle::handle_t handle = ObjectHandle::constructRawHandle(index, type, 0u);
  if (s_HandletoIndex.count(handle))
  {
    return s_pComponentManagers[type]->Get(0);
  }

  // Get space in actual component manager
  ObjectHandle::ID_t compIndex = s_pComponentManagers[type]->Add(pComp);
  IComponent *pActual = s_pComponentManagers[type]->Get(compIndex);
  if (pActual != pComp)
  {
    delete pComp;
    return pActual;  // Don't set data structures if the add failed
  }

  // Add to s_EntityList
  s_EntityList[index].second.push_back(std::make_pair(type, compIndex));

  // Add to s_HandletoIndex
  s_HandletoIndex[handle] = compIndex;

  if (skipRefresh)
  {
    return pActual;
  }

  // Refresh all of the entity's components
  for (size_t i = 0; i < s_EntityList[index].second.size(); ++i)
  {
    GetComponent(entity, s_EntityList[index].second[i].first)->Refresh();
  }

  return pActual;
}

IComponent *EntityManager::GetComponent(Entity entity, ObjectHandle::type_t type)
{
  ObjectHandle::ID_t ID = entity.m_ID.GetID();

  // Validity tests
  if (ID == 0 || ID >= s_EntityList.size() ||
      entity.m_ID.GetVersion() != s_EntityList[ID].first)
  {
    return s_pComponentManagers[type]->Get(0);
  }

  // Get component
  ObjectHandle::handle_t handle = ObjectHandle::constructRawHandle(ID, type, 0u);
  auto iter = s_HandletoIndex.find(handle);
  if (iter == s_HandletoIndex.end())
  {
    // Return null if entity doesn't have that component type
    return s_pComponentManagers[type]->Get(0);
  }
  ObjectHandle::ID_t compIndex = iter->second;

  return s_pComponentManagers[type]->Get(compIndex);
}

bool EntityManager::HasComponent(Entity entity, ObjectHandle::type_t type)
{
  ObjectHandle::ID_t ID = entity.m_ID.GetID();

  // Validity tests
  if (ID == 0 || ID >= s_EntityList.size() ||
      entity.m_ID.GetVersion() != s_EntityList[ID].first)
  {
    return false;
  }

  // Check hash for component
  ObjectHandle::handle_t handle = ObjectHandle::constructRawHandle(ID, type, 0u);
  return (s_HandletoIndex.count(handle));
}

void EntityManager::RemoveComponent(Entity entity, ObjectHandle::type_t type,
                                    bool skipRefresh)
{
  ObjectHandle::ID_t ID = entity.m_ID.GetID();

  // See if removal is necessary
  if (ID == 0 || ID >= s_EntityList.size() ||
      entity.m_ID.GetVersion() != s_EntityList[ID].first)
  {
    return;
  }

  ObjectHandle::handle_t handle = ObjectHandle::constructRawHandle(ID, type, 0u);
  auto iter = s_HandletoIndex.find(handle);
  if (iter == s_HandletoIndex.end())
  {
    return;
  }

  // Remove from ComponentManager
  s_pComponentManagers[type]->Get(iter->second)->OnDestroy();
  ObjectHandle::ID_t displaced = s_pComponentManagers[type]->Delete(iter->second);

  // Update displaced component entry, if any
  if (displaced != 0)
  {
    displaced = s_pComponentManagers[type]->Get(displaced)->GetEntity().m_ID.GetID();
    ObjectHandle::handle_t dispHandle =
        ObjectHandle::constructRawHandle(displaced, type, 0u);

    DEBUG_ASSERT(s_HandletoIndex.count(dispHandle));
    s_HandletoIndex[dispHandle] = iter->second;
  }

  // Remove entry from hash
  s_HandletoIndex.erase(handle);

  if (skipRefresh)
  {
    return;
  }

  // Refresh all of the entity's components
  compList_t &compList = s_EntityList[ID].second;
  for (size_t i = compList.size(); i-- > 0;)
  {
    if (compList[i].first == type)
    {
      compList.erase(compList.begin() + i);
    }
    else
    {
      GetComponent(entity, compList[i].first)->Refresh();
    }
  }
}

void EntityManager::AddEntities(size_t chunkSize)
{
  // Add chunkSize to freeList (extra logic for edge-case of nearing the max)
  size_t i = s_EntityList.size();
  size_t max = min(i + chunkSize, (size_t)ObjectHandle::MAX_ID);
  max = (max < i) ? ObjectHandle::MAX_ID : max;
  for (--i; i++ < max;)
  {
    s_EntityList.push_back(make_pair(0, compList_t()));
    s_FreeList.push((ObjectHandle::ID_t)i);
  }
}

}  // namespace tetrad
