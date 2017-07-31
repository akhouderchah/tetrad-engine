#pragma once

#include "UIComponent.h"

class UIButton : public UIComponent
{
public:
	UIButton(Entity entity);
	~UIButton();

	void SetTextures(const std::string &defaultTex,
					 const std::string &hoverTex,
					 const std::string &pressTex);

	void OnHoverEnter();
	void OnHoverLeave();

	void OnTouchEnter();
	void OnTouchLeave();

private:
	GLuint m_DefaultTex;
	GLuint m_HoverTex;
	GLuint m_PressTex;
};
