#include "DrawSystem.h"
#include "EntityManager.h"
#include "Paths.h"
#include "PhysicsComponent.h"
#include "LogSystem.h"

GLuint vertexArrayID;

DrawSystem::DrawSystem() :
	m_pDrawComponents(EntityManager::GetAll<DrawComponent>()),
	m_pMaterialComponents(EntityManager::GetAll<MaterialComponent>())
{
}

DrawSystem::~DrawSystem()
{
}

bool DrawSystem::Initialize()
{
	ShaderProgram program(2);
	program.AddShader(GL_VERTEX_SHADER, TEST_VERTEX_PATH);
	program.AddShader(GL_FRAGMENT_SHADER, TEST_FRAG_PATH);
	m_Program = program.Compile();
	if(m_Program == GL_NONE)
		return false;

	m_WorldLoc = glGetUniformLocation(m_Program, "gWorld");
	if(m_WorldLoc == 0xFFFFFFFF){ return false; }

	m_TextureLoc = glGetUniformLocation(m_Program, "gSampler");
	if(m_TextureLoc == 0xFFFFFFFF){ return false; }

	m_AlphaLoc = glGetUniformLocation(m_Program, "gAlpha");
	if(m_AlphaLoc == 0xFFFFFFFF){ return false; }

	m_TimeLoc = glGetUniformLocation(m_Program, "gTime");
	if(m_TimeLoc == 0xFFFFFFFF){ return false; }

	glClearColor(0.f, 0.f, 0.4f, 1.f);

	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	/*
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	*/

	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void DrawSystem::Shutdown()
{
	glDeleteProgram(m_Program);
}

void DrawSystem::Tick(deltaTime_t dt)
{
	DEBUG_ASSERT(m_pCurrentCamera);

	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_Program);

	// Set scale
	static float scale = 0.f;
	scale -= dt * 1.f;

	//static MovableComponent* pMove = EntityManager::GetComponent<MovableComponent>(m_pDrawComponents[1].second);
	//pMove->SetOrientation(glm::vec3(4*sin(scale), 0, 0));

	// Update all materials
	for(size_t i = 1; i < m_pMaterialComponents.size(); ++i)
	{
		m_pMaterialComponents[i]->Tick(dt);
	}

	static glm::mat4 MVP;
	const glm::mat4& cameraMat = m_pCurrentCamera->GetCameraMatrix();

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Iterate over all drawables and draw them
	for(size_t i = 1; i < m_pDrawComponents.size(); ++i)
	{
		DEBUG_ASSERT(m_pDrawComponents[i]->m_pTransformComp);

		// Update material globals in shaders
		glUniform1f(m_AlphaLoc, m_pDrawComponents[i]->GetOpacity());
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
	glUseProgram(0);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	// Display screen
	glfwSwapBuffers(s_pWindow);
}

