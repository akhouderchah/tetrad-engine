#include "DrawSystem.h"
#include "EntityManager.h"
#include "Paths.h"
#include "LogSystem.h"
#include "Screen.h"
#include "PhysicsComponent.h"
#include "CameraComponent.h"

#include "UIComponent.h"
#include "UI/UIViewport.h"

#include <glm/gtc/matrix_transform.hpp>

GLuint vertexArrayID;

DrawSystem::DrawSystem() :
	m_pDrawComponents(EntityManager::GetAll<DrawComponent>()),
	m_pMaterialComponents(EntityManager::GetAll<MaterialComponent>()),
	m_pUIComponents(EntityManager::GetAll<UIComponent>()),
	m_pViewports(EntityManager::GetAll<UIViewport>()),
	m_UIPlane(ResourceManager::LoadModel(MODEL_PATH + "UIplane.obj"))
{
}

DrawSystem::~DrawSystem()
{
}

bool DrawSystem::Initialize()
{
	// Setup default shader
	ShaderProgram program(2);
	program.PushShader(GL_VERTEX_SHADER, TEST_VERTEX_PATH);
	program.PushShader(GL_FRAGMENT_SHADER, TEST_FRAG_PATH);
	m_Program = program.Compile();
	if(m_Program == GL_NONE)
		return false;

	// Setup UI shader
	program.PopShader();
	program.PushShader(GL_FRAGMENT_SHADER, UI_FRAG_PATH);
	m_UIProgram = program.Compile();
	if(m_UIProgram == GL_NONE)
		return false;

	// Get default shader uniforms
	m_WorldLoc = glGetUniformLocation(m_Program, "gWorld");
	if(m_WorldLoc == 0xFFFFFFFF){ return false; }

	m_TextureLoc = glGetUniformLocation(m_Program, "gSampler");
	if(m_TextureLoc == 0xFFFFFFFF){ return false; }

	m_AddColorLoc = glGetUniformLocation(m_Program, "gAddColor");
	if(m_AddColorLoc == 0xFFFFFFFF){ return false; }

	m_MultColorLoc = glGetUniformLocation(m_Program, "gMultColor");
	if(m_MultColorLoc == 0xFFFFFFFF){ return false; }

	m_TimeLoc = glGetUniformLocation(m_Program, "gTime");
	if(m_TimeLoc == 0xFFFFFFFF){ return false; }

	// Get UI shader uniforms
	m_UIWorldLoc = glGetUniformLocation(m_UIProgram, "gWorld");
	if(m_UIWorldLoc == 0xFFFFFFFF){ return false; }

	m_UITextureLoc = glGetUniformLocation(m_UIProgram, "gSampler");
	if(m_UITextureLoc == 0xFFFFFFFF){ return false; }

	m_UIAddColorLoc = glGetUniformLocation(m_UIProgram, "gAddColor");
	if(m_UIAddColorLoc == 0xFFFFFFFF){ return false; }

	m_UIMultColorLoc = glGetUniformLocation(m_UIProgram, "gMultColor");
	if(m_UIMultColorLoc == 0xFFFFFFFF){ return false; }

	m_UITimeLoc = glGetUniformLocation(m_UIProgram, "gTime");
	if(m_UITimeLoc == 0xFFFFFFFF){ return false; }

	m_UITopMultLoc = glGetUniformLocation(m_UIProgram, "gTopMultiplier");
	if(m_UITopMultLoc == 0xFFFFFFFF){ return false; }

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClearDepth(1.f);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_DEPTH_CLAMP);

	return true;
}

void DrawSystem::Shutdown()
{
	glDeleteProgram(m_Program);
}

void DrawSystem::Tick(deltaTime_t dt)
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_Program);

	// Set scale
	static float scale = 0.f;
	scale -= dt * 1.f;

	// Update all materials
	for(size_t i = 1; i < m_pMaterialComponents.size(); ++i)
	{
		m_pMaterialComponents[i]->Tick(dt);
	}

	static glm::mat4 MVP;

	uint32_t w = s_pScreen->GetWidth();
	uint32_t h = s_pScreen->GetHeight();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glEnable(GL_DEPTH_TEST);

	for(size_t view = 1; view < m_pViewports.size(); ++view)
	{
		screenBound_t bounds = m_pViewports[view]->GetScreenBounds();
		float sX = bounds.points[0].X*w;
		float sY = bounds.points[0].Y*h;
		float viewWidth = w*bounds.points[1].X - sX;
		float viewHeight = h*bounds.points[1].Y - sY;

		const glm::mat4& cameraMat = m_pViewports[view]->GetCamera()->
			GetCameraMatrix(viewWidth, viewHeight);

		glViewport(sX, sY, viewWidth, viewHeight);

		// Iterate over all drawables and draw them
		for(size_t i = 1; i < m_pDrawComponents.size(); ++i)
		{
			DEBUG_ASSERT(m_pDrawComponents[i]->m_pTransformComp);

			// Update material globals in shaders
			glUniform4fv(m_AddColorLoc, 1,
						 &m_pDrawComponents[i]->GetAddColor()[0]);
			glUniform4fv(m_MultColorLoc, 1,
						 &m_pDrawComponents[i]->GetMultColor()[0]);
			glUniform1f(m_TimeLoc, m_pDrawComponents[i]->GetTime());

			// Create final MVP matrix
			//
			// This could be done in the vertex shader, but would result in
			// duplicating this computation for every vertex in a model
			MVP = cameraMat * m_pDrawComponents[i]->m_pTransformComp->GetWorldMatrix();
			glUniformMatrix4fv(m_WorldLoc, 1, GL_FALSE, &MVP[0][0]);

			glBindBuffer(GL_ARRAY_BUFFER, m_pDrawComponents[i]->m_VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pDrawComponents[i]->m_IBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
								  sizeof(DrawComponent::Vertex), 0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
								  sizeof(DrawComponent::Vertex),
								  (const GLvoid*)sizeof(glm::vec3));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
								  sizeof(DrawComponent::Vertex),
								  (const GLvoid*)(2*sizeof(glm::vec3)));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_pDrawComponents[i]->m_Tex);
			glUniform1i(m_TextureLoc, 0);

			glDrawElements(GL_TRIANGLES, m_pDrawComponents[i]->m_IndexCount, GL_UNSIGNED_INT, 0);
		}
	}

	glViewport(0, 0, w, h);

	// Draw UIComponents
	glUseProgram(m_UIProgram);
	glDisable(GL_DEPTH_TEST);

	glBindBuffer(GL_ARRAY_BUFFER, m_UIPlane.m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_UIPlane.m_IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
						  sizeof(DrawComponent::Vertex), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
						  sizeof(DrawComponent::Vertex),
						  (const GLvoid*)sizeof(glm::vec3));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
						  sizeof(DrawComponent::Vertex),
						  (const GLvoid*)(2*sizeof(glm::vec3)));

	static glm::mat4 UICameraMat = glm::ortho(0.f, 1.f, 0.f, 1.f, 1.f, 100.f);

	for(size_t i = 1; i < m_pUIComponents.size(); ++i)
	{
		DEBUG_ASSERT(m_pUIComponents[i]->m_pTransformComp);

		glUniform4fv(m_UIAddColorLoc, 1,
					 &m_pUIComponents[i]->m_pMaterialComp->m_AddColor[0]);
		glUniform4fv(m_UIMultColorLoc, 1,
					 &m_pUIComponents[i]->m_pMaterialComp->m_MultColor[0]);
		glUniform4fv(m_UITopMultLoc, 1,
					 &m_pUIComponents[i]->m_pMaterialComp->m_TopMultiplier[0]);
		glUniform1f(m_UITimeLoc, m_pUIComponents[i]->m_pMaterialComp->m_Time);

		MVP = UICameraMat * m_pUIComponents[i]->m_pTransformComp->GetWorldMatrix();
		glUniformMatrix4fv(m_UIWorldLoc, 1, GL_FALSE, &MVP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_pUIComponents[i]->m_CurrTex);
		glUniform1i(m_UITextureLoc, 0);

		glDrawElements(GL_TRIANGLES, m_UIPlane.m_IndexCount, GL_UNSIGNED_INT, 0);
	}

	glUseProgram(0);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	// Display screen
	glfwSwapBuffers(s_pScreen->GetWindow());
}

