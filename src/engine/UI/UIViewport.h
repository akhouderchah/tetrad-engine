#pragma once

#include "UIComponent.h"

class Screen;
class CameraComponent;

/**
 * @brief UI element that acts as a viewport
 *
 * @note This class diverges from the typical UIComponent child class
 * in that UIViewports are NOT placed in the same ObjList as the other
 * UIComponent types. For this UI class specifically, one would want to
 * call EntityManager::GetAll<UIViewport>();
 */
class UIViewport : public UIComponent
{
public:
	UIViewport(Entity entity);
	~UIViewport(){}

	void Refresh() final;

	void SetScreen(Screen *pScreen){ m_pScreen = pScreen; }
	inline Screen *GetScreen(){ return m_pScreen; }

	void SetCamera(CameraComponent *pCamera){ m_pCamera = pCamera; }
	inline CameraComponent *GetCamera(){ return m_pCamera; }

private:
	CameraComponent *m_pCamera;
	Screen *m_pScreen;
};
