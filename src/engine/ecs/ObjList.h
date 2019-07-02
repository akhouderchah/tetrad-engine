#pragma once

#include "core/ConstVector.h"
#include "core/LogSystem.h"
#include "engine/ecs/Entity.h"

class IComponent;

/**
 * @brief Class to deal with the allocation of objects
 */
template <typename T>
class ObjList
{
public:
	ObjList();
	~ObjList();

	// Returns the component index of the newly added object
	ObjectHandle::ID_t Add(IComponent *pComp);

	// Returns the ID of the comp moved to the current index
	ObjectHandle::ID_t Delete(ObjectHandle::ID_t index);
	void DeleteAll();

	ConstVector<T*> GetAll(){ return ConstVector<T*>(m_pObjects); }
	T *operator[](ObjectHandle::ID_t index) const;
private:
	std::vector<T*> m_pObjects;
};

template <typename T>
ObjList<T>::ObjList()
{
	// Add the null element
	m_pObjects.push_back(new T(kNullEntity));

	// @TODO Don't construct null element?
	//T *pObj = (T*)::operator new (sizeof(T));
	//m_pObjects.push_back(pObj);
}

template <typename T>
ObjList<T>::~ObjList()
{
	//::operator delete(&m_pObjects[0]);

	for(size_t i = 0; i < m_pObjects.size(); ++i)
	{
		delete m_pObjects[i];
	}
	m_pObjects.clear();
}

template <typename T>
ObjectHandle::ID_t ObjList<T>::Add(IComponent *pObj)
{
	size_t i = m_pObjects.size();
	if(i >= ObjectHandle::MAX_ID)
	{
		LOG("Failed to allocate space for a new element! Perhaps remove some existing entities/components or expand the ObjectHandle sizes.\n");
		return 0;
	}

	m_pObjects.push_back((T*)pObj);
	return (ObjectHandle::ID_t)i;
}

template <typename T>
ObjectHandle::ID_t ObjList<T>::Delete(ObjectHandle::ID_t index)
{
	size_t size = m_pObjects.size();
	if(index >= size || index == 0){ return 0; }

	delete m_pObjects[index];
	m_pObjects[index] = m_pObjects.back();
	m_pObjects.pop_back();

	if(index == size-1){ return 0; }
	return index;
}

template <typename T>
void ObjList<T>::DeleteAll()
{
	// Delete and pop back everything but the 0 element
	for(size_t i = m_pObjects.size(); i-->1;)
	{
		delete m_pObjects[i];
		m_pObjects.pop_back();
	}
}

template <typename T>
T *ObjList<T>::operator[](ObjectHandle::ID_t index) const
{
	if(index >= m_pObjects.size() || index == 0)
	{
		index = 0;
	}

	return m_pObjects[index];
}

