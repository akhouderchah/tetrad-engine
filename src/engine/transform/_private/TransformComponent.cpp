#include "engine/transform/TransformComponent.h"

#include <glm/gtx/transform.hpp>

#include "engine/ecs/EntityManager.h"
#include "engine/transform/AttachComponent.h"

namespace tetrad {

using glm::mat3;
using glm::mat4;
using glm::quat;
using glm::vec3;

TransformComponent::TransformComponent(Entity entity)
    : IComponent(entity), m_pParentTransform(nullptr)
{
  MarkDirty();
}

TransformComponent::~TransformComponent()
{
  if (!EntityManager::InShutdown())
  {
    for (Entity childEntity : m_ChildEntities)
    {
      auto pAttach = EntityManager::GetComponent<AttachComponent>(childEntity);
      if (pAttach->GetID() == 0)
      {
        auto pTrans = EntityManager::GetComponent<TransformComponent>(childEntity);
        if (pTrans->GetID() != 0)
        {
          pTrans->m_pParentTransform = nullptr;
        }
      }
      else
      {
        pAttach->RefreshParent(nullptr);
      }
    }
  }
}

bool TransformComponent::Init(const vec3& position, const vec3& scale)
{
  m_Position = position;
  m_Scale = scale;
  UpdateDirs();
  return true;
}

void TransformComponent::Refresh()
{
  for (Entity childEntity : m_ChildEntities)
  {
    EntityManager::GetComponent<AttachComponent>(childEntity)->RefreshParent(this);
  }
}

const mat4& TransformComponent::GetWorldMatrix() const
{
  if (IsDirty())
  {
    m_PosMatrix =
        glm::translate(m_Position) * glm::mat4_cast(m_Orientation) * glm::scale(m_Scale);

    // Note that there is an edge case where the parent-child graph of
    // TransformComponents contains a cycle. In this case, an infinite
    // loop is avoided by virtue of m_PosMatrix having been modified by
    // this point.
    //
    // This is only true for the current implementation of MarkDirty, and
    // more importantly, it does not make sense for there to be dependency
    // cycles to begin with.
    //
    // TODO detect dependency cycles?
    if (m_pParentTransform)
    {
      m_PosMatrix = m_pParentTransform->GetWorldMatrix() * m_PosMatrix;
    }
  }

  return m_PosMatrix;
}

void TransformComponent::MarkDirty()
{
  m_PosMatrix[0][3] = 1.f;

  for (Entity childEntity : m_ChildEntities)
  {
    EntityManager::GetComponent<TransformComponent>(childEntity)->MarkDirty();
  }
}

void TransformComponent::UpdateDirs() const
{
  mat3 orientationMatrix = toMat3(GetOrientation());
  m_LocalDirs.facingDir = normalize(orientationMatrix * vec3(0, 0, -1));
  m_LocalDirs.upDir = normalize(orientationMatrix * vec3(0, 1, 0));
  m_LocalDirs.rightDir = normalize(cross(m_LocalDirs.facingDir, m_LocalDirs.upDir));
}

vec3 TransformComponent::GetAbsolutePosition() const
{
  if (m_pParentTransform)
  {
    return m_pParentTransform->GetAbsolutePosition() +
           m_pParentTransform->GetAbsoluteScale() * m_Position;
  }
  return m_Position;
}

#define DEFINE_ABSOLUTE(retType, val)                                                \
  retType TransformComponent::GetAbsolute##val() const                               \
  {                                                                                  \
    if (m_pParentTransform) return m_##val * m_pParentTransform->GetAbsolute##val(); \
    return m_##val;                                                                  \
  }

DEFINE_ABSOLUTE(quat, Orientation)
DEFINE_ABSOLUTE(vec3, Scale)

vec3 TransformComponent::GetParentScale() const
{
  if (m_pParentTransform)
  {
    return m_pParentTransform->GetAbsoluteScale();
  }
  return vec3(1, 1, 1);
}

}  // namespace tetrad
