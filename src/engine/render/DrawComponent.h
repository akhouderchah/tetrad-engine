#pragma once

#include "core/BaseTypes.h"
#include "core/GlTypes.h"
#include "core/Reflection.h"
#include "engine/ecs/IComponent.h"

namespace tetrad {

class DrawSystem;
class TransformComponent;
class MaterialComponent;

/** @brief Component to make an entity visible in the game world.
 *
 * Provides the draw system with the information required to render
 * an entity. Note that some of this information may come from other
 * components (like the TransformComponent), or from resources external
 * to this component, but nonetheless compiled together in this component.
 *
 * References the resources (meshes, textures, etc) needed to draw the object.
 */
COMPONENT()
class DrawComponent : public IComponent
{
 public:
  DrawComponent(Entity entity);

  void SetGeometry(ShapeType shape);
  void SetGeometry(std::string model);

  void SetTexture(std::string texture, TextureType type);

  const glm::vec4 &GetAddColor() const;
  const glm::vec4 &GetMultColor() const;
  float GetTime() const;

  void Refresh() override;

  struct Vertex
  {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 uv;
  };

 private:
  // DrawComponent(const DrawComponent& that);

  /// Things that a draw system should know about go here
  friend DrawSystem;
  TransformComponent *m_pTransformComp;
  MaterialComponent *m_pMaterialComp;
  GLuint m_VBO;
  GLuint m_IBO;
  GLuint m_Tex;
  GLsizei m_IndexCount;
};

}  // namespace tetrad
