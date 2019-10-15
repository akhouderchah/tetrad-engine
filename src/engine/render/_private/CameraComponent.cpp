// @TODO Orthographic projection not working!
#include "engine/render/CameraComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "engine/ecs/EntityManager.h"
#include "engine/transform/MovableComponent.h"
#include "engine/transform/TransformComponent.h"

namespace tetrad {

using namespace glm;

#define DEFAULT_FOV 45.f
#define DEFAULT_NEAR 0.1f
#define DEFAULT_FAR 1000.f

CameraComponent::CameraComponent(Entity entity)
    : IComponent(entity),
      m_pTransformComp(nullptr),
      m_pMover(nullptr),
      m_ProjectionType(EPT_PERSPECTIVE),
      m_FOV(DEFAULT_FOV),
      m_Near(DEFAULT_NEAR),
      m_Far(DEFAULT_FAR)
{}

void CameraComponent::SetProjectionType(EProjectionType projectionType)
{
  m_ProjectionType = projectionType;
}

void CameraComponent::Refresh()
{
  m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
  m_pMover = EntityManager::GetComponent<MovableComponent>(m_Entity);
}

const glm::mat4& CameraComponent::GetCameraMatrix(float width, float height) const
{
  // NOTE: This is contingent on the DrawSystem getting the camera
  // matrix before getting the entity's world matrix. This shouldn't
  // be an issue, because it makes sense for the DrawSystem to get the
  // matrices in that order. But it is something to keep in mind when
  // an entity has both DrawComponent and CameraComponent.
  if (m_pTransformComp->IsDirty())
  {
    // Generate view matrix
    const TransformDirs& localDirs = m_pTransformComp->GetLocalDirs();

    vec3 cameraPos = m_pTransformComp->GetPosition();

    // logic for when camera is attached to another entity
    if (m_pTransformComp->m_pParentTransform)
    {
      cameraPos += m_pTransformComp->m_pParentTransform->GetPosition();
    }

    m_CameraMatrix = lookAt(cameraPos, cameraPos + localDirs.facingDir, localDirs.upDir);

    // Add projection matrix on the left side
    switch (m_ProjectionType)
    {
      case EPT_PERSPECTIVE:
        m_CameraMatrix =
            perspectiveFov(m_FOV, width, height, m_Near, m_Far) * m_CameraMatrix;
        break;
      case EPT_ORTHOGRAPHIC:
        m_CameraMatrix = ortho(0.f, width, 0.f, height, m_Near, m_Far) * m_CameraMatrix;
        break;
      default:
        DEBUG_LOG("Camera: Invalid perspective type - " << m_ProjectionType << "\n");
    }
  }

  return m_CameraMatrix;
}

}  // namespace tetrad
