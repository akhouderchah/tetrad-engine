#pragma once

#include "Base.h"
#include "IComponent.h"

class TransformComponent;
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

	virtual void OnHoverEnter(){}
	virtual void OnHoverLeave(){}

	virtual void OnTouchEnter(){}
	virtual void OnTouchLeave(){}

protected:
	screenBound_t GetScreenBounds() const;

protected:
	friend class Screen;
	friend class ScreenPartition;
	UIRectangleBound_t m_PartitionRectangle;

	friend class DrawSystem;
	GLuint m_CurrTex;

	TransformComponent *m_pTransformComp;
};
