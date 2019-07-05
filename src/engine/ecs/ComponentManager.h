#pragma once

#include <iostream>
#include <typeinfo>

#include "core/ConstVector.h"
#include "core/ErrorSystem.h"
#include "core/Guid.h"
#include "core/ObjectHandle.h"
#include "engine/ecs/Entity.h"
#include "engine/ecs/IComponentManager.h"

namespace tetrad {

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
	~ComponentManager();

	ObjectHandle::ID_t Add(IComponent *pComponent) override;
	ObjectHandle::ID_t Delete(ObjectHandle::ID_t index) override;

	void DeleteAll() override;

	IComponent *Get(ObjectHandle::ID_t index) const override;
	ConstVector<T*> GetAll();
	IComponent *operator[](ObjectHandle::ID_t index) const;

	static ObjectHandle::type_t GetType(){ return s_ID; }
private:
	friend class GUID<IComponentManager, ObjectHandle::type_t>;

	ComponentManager();

	std::vector<T*> m_pComponents;
	static ObjectHandle::type_t s_ID;
};

template <typename T>
ComponentManager<T>::ComponentManager()
{
#ifdef _DEBUG
	std::cout << "Creating manager of type: " << typeid(T).name() << std::endl;
#endif  // _DEBUG

	// Add the null element
	m_pComponents.push_back(new T(kNullEntity));

	// @TODO Don't construct null element?
	//T *pComponent = (T*)::operator new (sizeof(T));
	//m_pComponents.push_back(pComponent);
}

template <typename T>
ComponentManager<T>::~ComponentManager()
{
	//::operator delete(&m_pComponents[0]);

	for(size_t i = 0; i < m_pComponents.size(); ++i)
	{
		delete m_pComponents[i];
	}
	m_pComponents.clear();
}

template <typename T>
ObjectHandle::ID_t ComponentManager<T>::Add(IComponent *pComponent)
{
	size_t i = m_pComponents.size();
	if(ObjectHandle::MAX_ID <= i)
	{
		LOG("Failed to allocate space for a new element! Perhaps remove some existing entities/components or expand the ObjectHandle sizes.\n");
		return 0;
	}

	m_pComponents.push_back((T*)pComponent);
	return (ObjectHandle::ID_t)i;
}

template <typename T>
ObjectHandle::ID_t ComponentManager<T>::Delete(ObjectHandle::ID_t index)
{
	size_t size = m_pComponents.size();
	if(index >= size || index == 0){ return 0; }

	delete m_pComponents[index];
	m_pComponents[index] = m_pComponents.back();
	m_pComponents.pop_back();

	if(index == size-1){ return 0; }
	return index;
}

template <typename T>
void ComponentManager<T>::DeleteAll()
{
	// Delete and pop back everything but the 0 element
	for(size_t i = m_pComponents.size(); i-->1;)
	{
		delete m_pComponents[i];
		m_pComponents.pop_back();
	}
}

template <typename T>
IComponent *ComponentManager<T>::Get(ObjectHandle::ID_t index) const
{
	if(index >= m_pComponents.size() || index == 0)
	{
		index = 0;
	}

	return m_pComponents[index];
}

template <typename T>
ConstVector<T*> ComponentManager<T>::GetAll()
{
	return ConstVector<T*>(m_pComponents);
}

template <typename T>
IComponent *ComponentManager<T>::operator[](ObjectHandle::ID_t index) const
{
	return this->Get(index);
}

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
