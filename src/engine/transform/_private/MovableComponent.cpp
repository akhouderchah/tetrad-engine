#include "engine/transform/MovableComponent.h"

#include <glm/gtx/euler_angles.hpp>

#include "engine/ecs/EntityManager.h"
#include "engine/transform/TransformComponent.h"

namespace tetrad {

using namespace glm;

MovableComponent::MovableComponent(Entity entity)
    : IComponent(entity), m_pTransformComp(nullptr)
{}

void MovableComponent::Refresh()
{
  m_pTransformComp = EntityManager::GetComponent<TransformComponent>(m_Entity);
}

void MovableComponent::SetPosition(const vec3& position)
{
  DEBUG_ASSERT(m_pTransformComp);
  m_pTransformComp->MarkDirty();
  m_pTransformComp->m_Position = position;
}

void MovableComponent::Move(const vec3& shift, EMoveType moveType)
{
  DEBUG_ASSERT(m_pTransformComp);
  m_pTransformComp->MarkDirty();

  vec3 moveVec;
  if (moveType == EMoveType::LOCAL)
  {
    const TransformDirs& dirs = m_pTransformComp->GetLocalDirs();
    moveVec =
        shift[0] * dirs.rightDir + shift[1] * dirs.upDir + shift[2] * dirs.facingDir;
  }
  else
  {
    moveVec = shift;
  }
  m_pTransformComp->m_Position += moveVec;
}

void MovableComponent::AbsoluteMove(const vec3& shift, EMoveType moveType)
{
  vec3&& scale = m_pTransformComp->GetParentScale();
  Move(shift / scale, moveType);
}

void MovableComponent::SetOrientation(const vec3& radAngles)
{
  DEBUG_ASSERT(m_pTransformComp);
  m_pTransformComp->MarkDirty();
  m_pTransformComp->m_Orientation = quat(radAngles);
  m_pTransformComp->UpdateDirs();
}

void MovableComponent::Rotate(float rotationRads, const vec3& rotationAxis)
{
  DEBUG_ASSERT(m_pTransformComp);
  m_pTransformComp->MarkDirty();
  m_pTransformComp->m_Orientation =
      glm::angleAxis((rotationRads), rotationAxis) * m_pTransformComp->m_Orientation;
  m_pTransformComp->UpdateDirs();
}

void MovableComponent::Rotate(const vec3& eulerAngles)
{
  DEBUG_ASSERT(m_pTransformComp);
  m_pTransformComp->MarkDirty();
  m_pTransformComp->m_Orientation = quat(eulerAngles) * m_pTransformComp->m_Orientation;
  m_pTransformComp->UpdateDirs();
}

void MovableComponent::Rotate(const mat3& rotationMatrix)
{
  DEBUG_ASSERT(m_pTransformComp);
  m_pTransformComp->MarkDirty();
  m_pTransformComp->m_Orientation =
      toQuat(rotationMatrix) * m_pTransformComp->m_Orientation;
  m_pTransformComp->UpdateDirs();
}

void MovableComponent::SetScale(const vec3& scale)
{
  DEBUG_ASSERT(m_pTransformComp);
  m_pTransformComp->MarkDirty();
  m_pTransformComp->m_Scale = scale;
}

void MovableComponent::Scale(const vec3& amount)
{
  DEBUG_ASSERT(m_pTransformComp);
  m_pTransformComp->MarkDirty();
  m_pTransformComp->m_Scale += amount;
}

}  // namespace tetrad
