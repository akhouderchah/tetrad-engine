#pragma once

#include "Base.h"
#include "IComponent.h"

class TransformComponent;
class MovableComponent;
class Screen;
class ScreenPartition;
class DrawSystem;

/**
 * @brief Base case for all UI element classes
 *
 * Provides the interface through which UI elements can be informed of
 * various user actions
 */
class UIComponent : public IComponent
{
public:
	UIComponent(Entity entity);
	virtual ~UIComponent();

	void Refresh();

	void SetCurrentTexture(const std::string &tex, TextureType);

	virtual void OnHoverEnter();
	virtual void OnHoverLeave();

	virtual void OnTouchEnter();
	virtual void OnTouchLeave();

	bool IsFollowingCursor() const{ return m_bFollowCursor; }
	MovableComponent *GetMover() const{ return m_pMover; }

protected:
	screenBound_t GetScreenBounds() const;
	std::vector<UIComponent*> &GetChildren();

protected:
	friend class Screen;
	friend class ScreenPartition;
	UIRectangleBound_t m_PartitionRectangle;

	friend class DrawSystem;
	GLuint m_CurrTex;

	bool m_IsMovable;
	bool m_bFollowCursor;

	TransformComponent *m_pTransformComp;
	MovableComponent *m_pMover;
};
