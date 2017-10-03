#pragma once

#include "Types.h"

class EntityManager;

/**
 * @brief Entity class to be used for all game objects
 *
 * Just a thin wrapper for an ObjHandle (specifically one of type entity).
 * Should not be inherited from. Instead, functionality is added
 * through the creation and modification of components and systems.
 *
 * @note An entity with m_UID.ID == 0 is a null entity. That is, it doesn't exist
 * in the game, and cannot contain any components. EntityManager::CreateEntity
 * will only create a null entity if no more entities can be created.
 */
class Entity
{
public:
	Entity(ObjHandle ID=ObjHandle::null) : m_ID(ID){}
	operator ObjHandle() const{ return m_ID; }
	bool operator ==(Entity other) const{ return (other.m_ID.GetID() == m_ID.GetID()) && (other.m_ID.GetVersion() == m_ID.GetVersion()); }
	bool operator !=(Entity other) const{ return !(other == *this); }
	bool IsNull() const{ return m_ID.GetID() == ObjHandle::null.GetID(); }

	template <typename T> inline T *GetAs();
	template <typename T> inline T *Add();

private:
	friend class EntityManager;

	ObjHandle m_ID;
};

namespace std
{
	/**
	 * @brief Full specialization of hash class
	 *
	 * This class is necessary to allow unordered_sets and unordered_maps
	 * with Entity instances as the key.
	 *
	 * @note This class only uses the ID part of the Entity's ObjHandle in
	 * calculating the hash value.
	 */
	template <>
	struct hash<Entity>
	{
		std::size_t operator()(const Entity& e) const
		{
			return hash<ObjHandle::ID_t>()(static_cast<ObjHandle>(e).GetID());
		}
	};
}
