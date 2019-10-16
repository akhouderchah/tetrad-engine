#pragma once

#include "core/BaseTypes.h"
#include "core/Reflection.h"
#include "engine/ecs/IComponent.h"

namespace tetrad {

class DrawComponent;

/** @brief Component to allow changes in material effects at runtime.
 *
 * Current effects supported are:
 *    + Colors to add & multiply to texture
 *    + Fade in/out
 *    + Texture scrolling
 */
COMPONENT()
class MaterialComponent : public IComponent
{
 public:
  MaterialComponent(Entity entity, glm::vec4 multColor = glm::vec4(1, 1, 1, 1));

  static const MaterialComponent &GetDefaultTransparent();

  /** @brief Set color to add to texture.
   *
   * @note Having a non-zero w component may result in strange visual
   * effects. Use with caution.
   *
   * @note Colors are NOT clamped before being set.
   */
  void SetAddColor(glm::vec4 &&addColor);

  /** @brief Set color to multiply to texture.
   *
   * @note Use the vec3 version for entities that will use fading.
   *
   * @note Colors are NOT clamped before being set.
   */
  void SetMultColor(glm::vec3 &&multColor);
  void SetMultColor(glm::vec4 &&multColor);

  /** @brief Set color multiplier for top of UI element.
   *
   * @note Having a non-zero w component may result in strange visual
   * effects. Use with caution.
   *
   * @note Colors are NOT clamped before being set.
   * @note NOT currently available for non-UI elements
   */
  void SetTopMultiplier(glm::vec4 &&topMultiplier);

  /** @brief Set the transparency of the entity to a value between 0 and 1.
   *
   * @return true if the resulting opacity is 0 or 1
   */
  bool SetOpacity(float opacity);

  /** @brief Add modify the transparency of the entity by some amount.
   *
   * @note Opacity will be clamped between 0 and 1
   * @return true if the resulting opacity is 0 or 1
   */
  bool IncrementOpacity(float delta);

  /** @brief Change opacity from current value to 1. */
  void FadeIn(deltaTime_t fadeTime = 1.f);

  /** @brief Change opacity from current value to 0. */
  void FadeOut(deltaTime_t fadeTime = 1.f);

  /** @brief Set the texture scroll rate. */
  void SetScrollRate(float scrollRate) { m_ScrollRate = scrollRate; }

  void Refresh() override;

 private:
  friend class DrawSystem;
  friend class DrawComponent;
  void Tick(deltaTime_t dt);

 private:
  deltaTime_t m_T;
  float m_Time;
  float m_ScrollRate;

  glm::vec4 m_AddColor;
  glm::vec4 m_MultColor;
  glm::vec4 m_TopMultiplier;
};

}  // namespace tetrad
