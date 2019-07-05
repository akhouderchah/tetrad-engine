#include "engine/ecs/ComponentManager.h"

#include "engine/ecs/EntityManager.h"

namespace tetrad {

ObjectHandle::type_t GUID<IComponentManager, ObjectHandle::type_t>::s_CurrentID = 0;

void GUID<IComponentManager, ObjectHandle::type_t>::AddManager(IComponentManager* pManager)
{
	EntityManager::s_pComponentManagers.push_back(pManager);
	std::cout << EntityManager::s_pComponentManagers.size() << std::endl;
}

}  // namespace tetrad
