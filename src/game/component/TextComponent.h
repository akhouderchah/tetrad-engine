#pragma once

#include "IComponent.h"

class Font;
class TransformComponent;

/**
 * @brief Component to allow rendering of text in-game
 */
class TextComponent : public IComponent
{
public:
	TextComponent(Entity entity);
	virtual ~TextComponent();

	void Refresh();

	inline void SetFont(Font &font){ m_pFont = &font; }
	inline const Font &GetFont(){ DEBUG_ASSERT(m_pFont); return *m_pFont; }

	inline void SetText(const std::string &text){ m_Text = text; }
	inline void AppendText(const std::string &text){ m_Text += text; }
	inline const std::string &GetText() const{ return m_Text; }

	inline const glm::vec4 GetTextColor(){ return glm::vec4(1, 1, 1, 1); }

	inline const TransformComponent *GetTransformComp() const{ return m_pTransformComp; }

private:
	std::string m_Text;
	Font *m_pFont;

	TransformComponent *m_pTransformComp;
};
