#pragma once

#include <string>

#include "core/LinkedList.h"
#include "core/Reflection.h"
#include "engine/ecs/IComponent.h"

namespace tetrad {

class Font;
class TransformComponent;
class UIComponent;

/**
 * @brief Component to allow rendering of text in-game
 */
COMPONENT()
class TextComponent : public IComponent
{
 public:
  TextComponent(Entity entity);
  virtual ~TextComponent();

  void Refresh() override;

  inline void SetFont(Font &font) { m_pFont = &font; }
  inline const Font &GetFont()
  {
    DEBUG_ASSERT(m_pFont);
    return *m_pFont;
  }

  inline void SetText(const std::string &text) { m_Text = text; }
  inline void AppendText(const std::string &text) { m_Text += text; }
  inline const std::string &GetText() const { return m_Text; }

  inline void SetTextScale(float scale) { m_Scale = scale; }
  inline float GetTextScale() const { return m_Scale; }

  inline void SetColor(const glm::vec4 &color) { m_Color = color; }
  inline const glm::vec4 &GetColor() { return m_Color; }

  inline const TransformComponent *GetTransformComp() const { return m_pTransformComp; }

  glm::vec2 CalculateSize() const;

 private:
  std::string m_Text;
  Font *m_pFont;

  TransformComponent *m_pTransformComp;
  UIComponent *m_pUIComp;

  bool m_IsFree;
  float m_Scale;

  glm::vec4 m_Color;

  /** @brief Linked list node of TextComps w/o UIComps */
  LinkedNode<TextComponent> m_FreeTextNode;

  friend class DrawSystem;
  static LinkedList<TextComponent> s_FreeTextComps;
};

}  // namespace tetrad
