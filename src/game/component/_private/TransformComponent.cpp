#include "TransformComponent.h"

#include <glm/gtx/transform.hpp>

using glm::vec3; using glm::quat;
using glm::mat4; using glm::mat3;

TransformComponent::TransformComponent(Entity entity) :
	IComponent(entity)
{
	MarkDirty();
}

TransformComponent::~TransformComponent()
{
}

bool TransformComponent::Init(const vec3& position, const vec3& scale)
{
	m_Position = position;
	m_Scale = scale;
	UpdateDirs();
	return true;
}

const mat4& TransformComponent::GetWorldMatrix() const
{
	if(IsDirty())
	{
		m_PosMatrix = glm::translate(m_Position) *
			glm::mat4_cast(m_Orientation) *
			glm::scale(m_Scale);
	}

	return m_PosMatrix;
}

void TransformComponent::UpdateDirs() const
{
	mat3 orientationMatrix = toMat3(GetOrientation());
	m_LocalDirs.facingDir = normalize(orientationMatrix * vec3(0, 0, -1));
	m_LocalDirs.upDir = normalize(orientationMatrix * vec3(0, 1, 0));
	m_LocalDirs.rightDir = normalize(cross(m_LocalDirs.facingDir, m_LocalDirs.upDir));
}
