#pragma once

#include "Base.h"
#include "IComponent.h"

class TransformComponent;
class Screen;

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

	bool IsAt(double x, double y);

protected:
	friend class Screen;
	UIRectangleBound_t m_PartitionRectangle;

	TransformComponent *m_pTransformComp;
};
