#pragma once

#include "ConstVector.h"
#include "DrawComponent.h"
#include "ISystem.h"
#include "MovableComponent.h"
#include "ShaderProgram.h"
#include "TransformComponent.h"
#include "MaterialComponent.h"
#include "ResourceManager.h"

class UIComponent;
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

	virtual bool Initialize();
	virtual void Shutdown();

	virtual void Tick(deltaTime_t dt);

private:
	ConstVector<DrawComponent*> m_pDrawComponents;
	ConstVector<MaterialComponent*> m_pMaterialComponents;
	ConstVector<UIComponent*> m_pUIComponents;
	ConstVector<UIViewport*> m_pViewports;

	ModelResource m_UIPlane;

	GLuint m_Program;
	GLuint m_UIProgram;

	// World shader variables
	GLuint m_WorldLoc;
	GLuint m_TextureLoc;
	GLuint m_AddColorLoc;
	GLuint m_MultColorLoc;
	GLuint m_TimeLoc;

	// UI shader variables
	GLuint m_UIWorldLoc;
	GLuint m_UITextureLoc;
	GLuint m_UITimeLoc;
	GLuint m_UIAddColorLoc;
	GLuint m_UIMultColorLoc;
	GLuint m_UITopMultLoc;
};

