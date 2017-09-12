#include "DrawSystem.h"
#include "EntityManager.h"
#include "Paths.h"
#include "LogSystem.h"
#include "Screen.h"
#include "UI/UI.h"
#include "Font.h"

#include "PhysicsComponent.h"
#include "CameraComponent.h"
#include "TextComponent.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

GLuint vertexArrayID;

DrawSystem::DrawSystem() :
	m_pDrawComponents(EntityManager::GetAll<DrawComponent>()),
	m_pMaterialComponents(EntityManager::GetAll<MaterialComponent>()),
	m_pTextComponents(EntityManager::GetAll<TextComponent>()),
	m_pViewports(EntityManager::GetAll<UIViewport>()),
	m_UIPlane(ResourceManager::LoadModel(MODEL_PATH + "UIplane.obj"))
{
}

DrawSystem::~DrawSystem()
{
}

bool DrawSystem::Initialize(Game *pGame)
{
	if(!ISystem::Initialize(pGame))
	{
		return false;
	}

	// Setup default shader
	ShaderProgram program(2);
	program.PushShader(GL_VERTEX_SHADER, SHADER_PATH + "world-vert.glsl");
	program.PushShader(GL_FRAGMENT_SHADER, SHADER_PATH + "world-frag.glsl");
	m_WorldProgram = program.Compile();
	if(m_WorldProgram == GL_NONE){ return false; }

	// Get default shader uniforms
	if(!m_WorldUniforms.GetLocations(m_WorldProgram)){ return false; }

	// Setup UI shader
	program.PopShader();
	program.PushShader(GL_FRAGMENT_SHADER, SHADER_PATH + "ui-frag.glsl");
	m_UIProgram = program.Compile();
	if(m_UIProgram == GL_NONE){ return false; }

	// Get UI shader uniforms
	if(!m_UIUniforms.GetLocations(m_UIProgram)){ return false; }

	// Setup text shader
	program.PopShader();
	program.PushShader(GL_FRAGMENT_SHADER, SHADER_PATH + "text-frag.glsl");
	m_TextProgram = program.Compile();
	if(m_TextProgram == GL_NONE){ return false; }

	// Get text shader uniforms
	if(!m_TextUniforms.GetLocations(m_TextProgram)){ return false; }

	// Setup initial OpenGL state
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

	// TODO Enable for wireframe drawing
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	return true;
}

void DrawSystem::Shutdown()
{
	glDeleteProgram(m_WorldProgram);
	glDeleteProgram(m_UIProgram);
}

void DrawSystem::Tick(deltaTime_t dt)
{
	// Clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(m_WorldProgram);

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

	// TODO - will multiple viewports mess with this?
	glEnable(GL_DEPTH_TEST);

	//
	// Render World
	//
	size_t max = m_pViewports.size();
	for(size_t view = 1; view < max; ++view)
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
			glUniform4fv(m_WorldUniforms.m_AddColorLoc, 1,
						 &m_pDrawComponents[i]->GetAddColor()[0]);
			glUniform4fv(m_WorldUniforms.m_MultColorLoc, 1,
						 &m_pDrawComponents[i]->GetMultColor()[0]);
			glUniform1f(m_WorldUniforms.m_TimeLoc, m_pDrawComponents[i]->GetTime());

			// Create final MVP matrix
			//
			// This could be done in the vertex shader, but would result in
			// duplicating this computation for every vertex in a model
			MVP = cameraMat * m_pDrawComponents[i]->m_pTransformComp->GetWorldMatrix();
			glUniformMatrix4fv(m_WorldUniforms.m_WorldLoc, 1, GL_FALSE, &MVP[0][0]);

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
			glUniform1i(m_WorldUniforms.m_TextureLoc, 0);

			glDrawElements(GL_TRIANGLES, m_pDrawComponents[i]->m_IndexCount, GL_UNSIGNED_INT, 0);
		}
	}

	glViewport(0, 0, w, h);

	//
	// Render UIComponents
	//
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

	const LinkedList<UIComponent> &uiList = s_pScreen->GetRenderList();
	LinkedNode<UIComponent> *pUINode = uiList.Begin();
	while(pUINode)
	{
		UIComponent *pUI = linked_node_owner(pUINode, UIComponent, m_RenderNode);
		DEBUG_ASSERT(pUI->m_pTransformComp);

		glUniform4fv(m_UIUniforms.m_AddColorLoc, 1,
					 &pUI->m_pMaterialComp->m_AddColor[0]);
		glUniform4fv(m_UIUniforms.m_MultColorLoc, 1,
					 &pUI->m_pMaterialComp->m_MultColor[0]);
		glUniform4fv(m_UIUniforms.m_TopMultLoc, 1,
					 &pUI->m_pMaterialComp->m_TopMultiplier[0]);

		MVP = UICameraMat * pUI->m_pTransformComp->GetWorldMatrix();
		glUniformMatrix4fv(m_UIUniforms.m_WorldLoc, 1, GL_FALSE, &MVP[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, pUI->m_CurrTex);
		glUniform1i(m_UIUniforms.m_TextureLoc, 0);

		glDrawElements(GL_TRIANGLES, m_UIPlane.m_IndexCount, GL_UNSIGNED_INT, 0);

		TextComponent *pText = pUI->m_pTextComp;
		if(pText->GetID() != 0)
		{
			RenderText(pText);
			glUseProgram(m_UIProgram);
		}

		pUINode = uiList.Next(*pUINode);
	}

	//
	// Render remaining text
	//
	const LinkedList<TextComponent> &textList = TextComponent::s_FreeTextComps;
	LinkedNode<TextComponent> *pTextNode = textList.Begin();
	while(pTextNode)
	{
		TextComponent *pText = linked_node_owner(pTextNode,
												 TextComponent,
												 m_FreeTextNode);
		RenderText(pText);

		pTextNode = textList.Next(*pTextNode);
	}

	glUseProgram(0);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	// Display screen
	glfwSwapBuffers(s_pScreen->GetWindow());
}

void DrawSystem::RenderText(TextComponent *pTextComp)
{
	DEBUG_ASSERT(pTextComp);

	glUseProgram(m_TextProgram);

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
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(m_TextUniforms.m_TextureLoc, 0);

	static glm::mat4 MVP;

	const char *str = pTextComp->GetText().c_str();

	glUniform4fv(m_TextUniforms.m_TextColorLoc, 1,
				 &pTextComp->GetColor()[0]);

	const Font &font = pTextComp->GetFont();

	glm::vec3 pos = pTextComp->GetTransformComp()->GetAbsolutePosition();
	pos.x = 2*pos.x - 1; pos.y = 2*pos.y - 1;
	float xStartPos = pos.x;

	float scaling = pTextComp->GetTextScale() * 2.f;
	glm::vec3 scale(scaling/(s_pScreen->GetWidth()),
					scaling/(s_pScreen->GetHeight()),
					1);

	char c;
	while((c = *str))
	{
		const Font::CharInfo &charInfo = font[c];
		if(c == '\n')
		{
			pos.x = xStartPos;
			pos.y -= charInfo.Size.y * 2 * scale.y;
			++str;
			continue;
		}

		// Render current character
		MVP[0][0] = charInfo.Size.x * scale.x;                       // width
		MVP[1][1] = charInfo.Size.y * scale.y;                       // height
		MVP[3][0] = pos.x + charInfo.Bearing.x * scale.x;            // xpos
		MVP[3][1] = pos.y - (charInfo.Size.y - charInfo.Bearing.y) * // ypos
			scale.y;

		glUniformMatrix4fv(m_WorldUniforms.m_WorldLoc, 1, GL_FALSE, &MVP[0][0]);

		glBindTexture(GL_TEXTURE_2D, charInfo.TextureID);
		glDrawElements(GL_TRIANGLES, m_UIPlane.m_IndexCount, GL_UNSIGNED_INT, 0);

		// Move forward by however much we need to
		pos.x += (charInfo.Advance/64.f) * scale.x;

		// Render next character
		++str;
	}
}
