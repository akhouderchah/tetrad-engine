// This file should not be included by normal files.
//
// build_tool generates a ComponentManager.cpp file which includes this file and
// contains explicit ComponentManager instantiations for all component types.
#include "engine/ecs/ComponentManager.h"

#include "engine/ecs/Entity.h"
#include "engine/ecs/EntityManager.h"

namespace tetrad {

ObjectHandle::type_t GUID<IComponentManager, ObjectHandle::type_t>::s_CurrentID = 0;

void GUID<IComponentManager, ObjectHandle::type_t>::AddManager(IComponentManager* pManager)
{
	EntityManager::s_pComponentManagers.push_back(pManager);
	std::cout << EntityManager::s_pComponentManagers.size() << std::endl;
}

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

}  // namespace tetrad
