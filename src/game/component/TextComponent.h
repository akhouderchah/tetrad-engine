#pragma once

#include "IComponent.h"

class Font;

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

	inline void SetText(const std::string &text){ m_Text = text; }
	inline void AppendText(const std::string &text){ m_Text += text; }
	inline const std::string &GetText() const{ return m_Text; }

private:
	std::string m_Text;
	Font *m_pFont;
};
