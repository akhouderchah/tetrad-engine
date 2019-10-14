#include "engine/render/MaterialComponent.h"

#include "core/ErrorSystem.h"

namespace tetrad
{

MaterialComponent::MaterialComponent(Entity entity, glm::vec4 multColor)
    : IComponent(entity),
      m_T(0.f),
      m_Time(0.f),
      m_ScrollRate(0.f),
      m_AddColor(0, 0, 0, 0),
      m_MultColor(multColor),
      m_TopMultiplier(.5, .5, .5, 1)
{
}

const MaterialComponent &MaterialComponent::GetDefaultTransparent()
{
  static MaterialComponent materialComp(kNullEntity, glm::vec4(0, 0, 0, 0));

  return materialComp;
}

void MaterialComponent::SetAddColor(glm::vec4 &&addColor)
{
  ASSERT_CHECK(m_Entity != kNullEntity, );
  m_AddColor = addColor;
}

void MaterialComponent::SetMultColor(glm::vec4 &&multColor)
{
  ASSERT_CHECK(m_Entity != kNullEntity, );
  m_MultColor = multColor;
}

void MaterialComponent::SetMultColor(glm::vec3 &&multColor)
{
  ASSERT_CHECK(m_Entity != kNullEntity, );
  m_MultColor = glm::vec4(multColor, m_MultColor[3]);
}

void MaterialComponent::SetTopMultiplier(glm::vec4 &&topMultiplier)
{
  ASSERT_CHECK(m_Entity != kNullEntity, );
  m_TopMultiplier = topMultiplier;
}

bool MaterialComponent::SetOpacity(float opacity)
{
  ASSERT_CHECK(m_Entity != kNullEntity, false);
  m_MultColor[3] = opacity;
  return IncrementOpacity(0.f);
}

bool MaterialComponent::IncrementOpacity(float delta)
{
  ASSERT_CHECK(m_Entity != kNullEntity, false);
  float &opacity = m_MultColor[3];
  opacity += delta;

  if (opacity < 0.f)
  {
    opacity = 0.f;
    return true;
  }
  else if (opacity > 1.f)
  {
    opacity = 1.f;
    return true;
  }

  return false;
}

void MaterialComponent::FadeIn(deltaTime_t fadeTime)
{
  ASSERT_CHECK(m_Entity != kNullEntity, );
  DEBUG_ASSERT(fadeTime > 0.f);
  m_T = fadeTime;
}

void MaterialComponent::FadeOut(deltaTime_t fadeTime)
{
  ASSERT_CHECK(m_Entity != kNullEntity, );
  DEBUG_ASSERT(fadeTime > 0.f);
  m_T = -1.f * fadeTime;
}

void MaterialComponent::Refresh() {}

void MaterialComponent::Tick(deltaTime_t dt)
{
  if (m_T != 0.f)
  {
    m_T *= !(IncrementOpacity(dt / m_T));
  }

  if (m_ScrollRate != 0.f)
  {
    m_Time += m_ScrollRate * dt;
  }
}

}  // namespace tetrad
