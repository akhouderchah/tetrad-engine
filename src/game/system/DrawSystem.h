#pragma once

#include "ConstVector.h"
#include "DrawComponent.h"
#include "ISystem.h"
#include "MovableComponent.h"
#include "ShaderProgram.h"
#include "ShaderGlobals.h"
#include "TransformComponent.h"
#include "MaterialComponent.h"
#include "ResourceManager.h"

class UIComponent;
class TextComponent;
class UIViewport;

/**
 * @brief System to perform the rendering of objects
 *
 * Uses DrawableComponents and TransformComponents in order to
 * render objects as needed. Current implementation uses OpenGL,
 * but could be modified to use a diferent API if needed. The only
 * real coupling between OpenGL and non-rendering parts of this
 * software is the use of GLFW as a window manager.
 *
 * @TODO Much like with the PhysicsSystem, some sort of space
 * partitioning or sorting could help split up work and/or
 * require less work from the CPU.
 */
class DrawSystem : public ISystem
{
public:
	DrawSystem();
	virtual ~DrawSystem();

	virtual bool Initialize(Game *pGame);
	virtual void Shutdown();

	virtual void Tick(deltaTime_t dt);

private:
	void RenderText(TextComponent *pTextComp);

private:
	ConstVector<DrawComponent*> m_pDrawComponents;
	ConstVector<MaterialComponent*> m_pMaterialComponents;
	ConstVector<TextComponent*> m_pTextComponents;
	ConstVector<UIViewport*> m_pViewports;

	ModelResource m_UIPlane;

	GLuint m_WorldProgram;
	WorldShaderGlobals m_WorldUniforms;

	GLuint m_UIProgram;
	UIShaderGlobals m_UIUniforms;

	GLuint m_TextProgram;
	TextShaderGlobals m_TextUniforms;
};

