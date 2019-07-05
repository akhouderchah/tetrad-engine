#pragma once

#include <iostream>
#include <typeinfo>

#include "core/ConstVector.h"
#include "core/ErrorSystem.h"
#include "core/Guid.h"
#include "core/ObjectHandle.h"
#include "engine/ecs/IComponentManager.h"
#include "engine/ecs/ObjList.h"

namespace tetrad {

class Entity;

/**
 * @brief Creates, deletes, and retrieves components of type T
 *
 * Used mainly by the EntityManager. The user will rarely have a need to
 * use any ComponentManager directly.
 */
template <class T>
class ComponentManager : public IComponentManager
{
public:
	ObjectHandle::ID_t Add(IComponent *pComponent) override
		{ return m_CompList.Add(pComponent); }
	ObjectHandle::ID_t Delete(ObjectHandle::ID_t index) override
		{ return m_CompList.Delete(index); }

	void DeleteAll(){ m_CompList.DeleteAll(); }

	IComponent *Get(ObjectHandle::ID_t index) const override{ return m_CompList[index]; }
	ConstVector<T*> GetAll(){ return m_CompList.GetAll(); }
	IComponent *operator[](ObjectHandle::ID_t index) const{ return m_CompList[index]; }
	static ObjectHandle::type_t GetType(){ return s_ID; }
private:
	ComponentManager(){ std::cout << "Creating manager of type: " << typeid(T).name() << std::endl; }
	friend class GUID<IComponentManager, ObjectHandle::type_t>;

	ObjList<T> m_CompList;
	static ObjectHandle::type_t s_ID;
};

/**
 * @brief Full specialization of the GUID class so as to allow for the compile-time adding of
 * ComponentManagers to EntityManager::s_pComponentManagers;
 */
template <>
class GUID<IComponentManager, ObjectHandle::type_t>
{
public:
	GUID() = delete;
	~GUID() = delete;

	template <class T>
	static ObjectHandle::type_t GenerateID();

private:
	static void AddManager(IComponentManager* pManager);

	static ObjectHandle::type_t s_CurrentID;
};

template <typename T>
ObjectHandle::type_t GUID<IComponentManager, ObjectHandle::type_t>::GenerateID()
{
	RELEASE_ASSERT(s_CurrentID+1 > s_CurrentID);

	// Add ComponentManager<T> to EntityManager::s_pComponentManagers
	AddManager(new ComponentManager<T>());

	return s_CurrentID++;
}

template <class T>
ObjectHandle::type_t ComponentManager<T>::s_ID = GUID<IComponentManager,ObjectHandle::type_t>::GenerateID<T>();

}  // namespace tetrad
