#include "engine/render/DrawSystem.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "core/Log.h"
#include "core/Paths.h"
#include "core/StlUtils.h"
#include "engine/ecs/EntityManager.h"
#include "engine/game/Game.h"
#include "engine/render/CameraComponent.h"
#include "engine/render/MaterialComponent.h"
#include "engine/render/ShaderProgram.h"
#include "engine/resource/Font.h"
#include "engine/resource/ResourceManager.h"
#include "engine/screen/Screen.h"
#include "engine/transform/MovableComponent.h"
#include "engine/transform/TransformComponent.h"
#include "engine/ui/TextComponent.h"
#include "engine/ui/UI.h"

namespace tetrad {

namespace {
// TODO modify pattern & move dithering to separate file.
// clang-format off
constexpr auto kDitherPattern = make_array<char>(
     0, 32,  8, 40,  2, 34, 10, 42,
    48, 16, 56, 24, 50, 18, 58, 26,
    12, 44,  4, 36, 14, 46,  6, 38,
    60, 28, 52, 20, 62, 30, 54, 22,
     3, 35, 11, 43,  1, 33,  9, 41,
    51, 19, 59, 27, 49, 17, 57, 25,
    15, 47,  7, 39, 13, 45,  5, 37,
    63, 31, 55, 23, 61, 29, 53, 21);
// clang-format on
}  // namespace

GLuint vertexArrayID;

DrawSystem::DrawSystem()
    : m_pDrawComponents(EntityManager::GetAll<DrawComponent>()),
      m_pMaterialComponents(EntityManager::GetAll<MaterialComponent>()),
      m_pTextComponents(EntityManager::GetAll<TextComponent>()),
      m_pViewports(EntityManager::GetAll<UIViewport>()),
      m_UIPlane(ResourceManager::LoadModel(MODEL_PATH + "UIplane.obj"))
{}

void DrawSystem::Tick(deltaTime_t dt)
{
  // Update all materials.
  for (size_t i = 1; i < m_pMaterialComponents.size(); ++i)
  {
    m_pMaterialComponents[i]->Tick(dt);
  }

  // Clear screen.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(m_WorldProgram);

  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);
  glEnableVertexAttribArray(2);

  // TODO - will multiple viewports mess with this?
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  Screen &currentScreen = m_pGame->GetCurrentScreen();
  // Render world for each viewport.
  size_t max = m_pViewports.size();
  for (size_t view = 1; view < max; ++view)
  {
    DEBUG_ASSERT(m_pViewports[view]);
    RenderWorld(currentScreen, *m_pViewports[view]);
  }
  // Now we've finished rendering on a per-viewport basis. Set the glViewport to
  // be the entire screen.
  glViewport(0, 0, currentScreen.GetWidth(), currentScreen.GetHeight());

  RenderUi(currentScreen);
  RenderFreeText(currentScreen);

  glUseProgram(0);
  glDisableVertexAttribArray(2);
  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);

  // Display screen.
  glfwSwapBuffers(currentScreen.GetWindow());
}

void DrawSystem::RenderWorld(const Screen &screen, const UIViewport &viewport)
{
  screenBound_t bounds = viewport.GetScreenBounds();
  uint32_t w = screen.GetWidth();
  uint32_t h = screen.GetHeight();
  float sX = bounds.points[0].X * w;
  float sY = bounds.points[0].Y * h;
  float viewWidth = w * bounds.points[1].X - sX;
  float viewHeight = h * bounds.points[1].Y - sY;

  const glm::mat4 &cameraMat =
      viewport.GetCamera()->GetCameraMatrix(viewWidth, viewHeight);

  glViewport(sX, sY, viewWidth, viewHeight);

  for (size_t i = 1; i < m_pDrawComponents.size(); ++i)
  {
    DEBUG_ASSERT(m_pDrawComponents[i]->m_pTransformComp);

    // Update material globals in shaders.
    glUniform4fv(m_WorldUniforms.m_AddColorLoc, 1,
                 &m_pDrawComponents[i]->GetAddColor()[0]);
    glUniform4fv(m_WorldUniforms.m_MultColorLoc, 1,
                 &m_pDrawComponents[i]->GetMultColor()[0]);
    glUniform1f(m_WorldUniforms.m_TimeLoc, m_pDrawComponents[i]->GetTime());

    // Create final MVP matrix.
    //
    // This could be done in the vertex shader, but would result in duplicating
    // this computation for every vertex in a model.
    static glm::mat4 MVP;
    MVP = cameraMat * m_pDrawComponents[i]->m_pTransformComp->GetWorldMatrix();
    glUniformMatrix4fv(m_WorldUniforms.m_WorldLoc, 1, GL_FALSE, &MVP[0][0]);

    glBindBuffer(GL_ARRAY_BUFFER, m_pDrawComponents[i]->m_VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pDrawComponents[i]->m_IBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex),
                          (const GLvoid *)sizeof(glm::vec3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex),
                          (const GLvoid *)(2 * sizeof(glm::vec3)));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_pDrawComponents[i]->m_Tex);
    glUniform1i(m_WorldUniforms.m_TextureLoc, 0);

    glDrawElements(GL_TRIANGLES, m_pDrawComponents[i]->m_IndexCount, GL_UNSIGNED_INT, 0);
  }
}

void DrawSystem::RenderUi(const Screen &screen)
{
  glUseProgram(m_UIProgram);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_DitherTexture);
  glUniform1i(m_UIUniforms.m_DitherTextureLoc, 1);

  glBindBuffer(GL_ARRAY_BUFFER, m_UIPlane.m_VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_UIPlane.m_IBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex),
                        (const GLvoid *)sizeof(glm::vec3));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex),
                        (const GLvoid *)(2 * sizeof(glm::vec3)));

  static glm::mat4 MVP;
  static glm::mat4 UICameraMat = glm::ortho(0.f, 1.f, 0.f, 1.f, 1.f, 100.f);

  const LinkedList<UIComponent> &uiList = screen.GetRenderList();
  LinkedNode<UIComponent> *pUINode = uiList.First();
  while (pUINode)
  {
    UIComponent *pUI = linked_node_owner(pUINode, UIComponent, m_RenderNode);
    DEBUG_ASSERT(pUI->m_pTransformComp);

    glUniform4fv(m_UIUniforms.m_AddColorLoc, 1, &pUI->m_pMaterialComp->m_AddColor[0]);
    glUniform4fv(m_UIUniforms.m_MultColorLoc, 1, &pUI->m_pMaterialComp->m_MultColor[0]);
    glUniform4fv(m_UIUniforms.m_TopMultLoc, 1, &pUI->m_pMaterialComp->m_TopMultiplier[0]);

    MVP = UICameraMat * pUI->m_pTransformComp->GetWorldMatrix();
    glUniformMatrix4fv(m_UIUniforms.m_WorldLoc, 1, GL_FALSE, &MVP[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pUI->m_CurrTex);
    glUniform1i(m_UIUniforms.m_TextureLoc, 0);

    glDrawElements(GL_TRIANGLES, m_UIPlane.m_IndexCount, GL_UNSIGNED_INT, 0);

    TextComponent *pText = pUI->m_pTextComp;
    DEBUG_ASSERT(pText);
    if (pText->GetID() != 0)
    {
      RenderTextComponent(screen, *pText);
      glUseProgram(m_UIProgram);
    }

    pUINode = uiList.Next(*pUINode);
  }
}

void DrawSystem::RenderFreeText(const Screen &screen)
{
  const LinkedList<TextComponent> &textList = TextComponent::s_FreeTextComps;
  LinkedNode<TextComponent> *pTextNode = textList.First();
  while (pTextNode)
  {
    TextComponent *pText = linked_node_owner(pTextNode, TextComponent, m_FreeTextNode);
    DEBUG_ASSERT(pText);
    RenderTextComponent(screen, *pText);

    pTextNode = textList.Next(*pTextNode);
  }
}

void DrawSystem::RenderTextComponent(const Screen &screen, const TextComponent &textComp)
{
  glUseProgram(m_TextProgram);
  glDisable(GL_DEPTH_TEST);
  // glUseProgram(m_WorldProgram);

  glBindBuffer(GL_ARRAY_BUFFER, m_UIPlane.m_VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_UIPlane.m_IBO);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex), 0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex),
                        (const GLvoid *)sizeof(glm::vec3));
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DrawComponent::Vertex),
                        (const GLvoid *)(2 * sizeof(glm::vec3)));
  glActiveTexture(GL_TEXTURE0);
  glUniform1i(m_TextUniforms.m_TextureLoc, 0);

  const char *str = textComp.GetText().c_str();

  glUniform4fv(m_TextUniforms.m_TextColorLoc, 1, &textComp.GetColor()[0]);

  const Font &font = textComp.GetFont();

  glm::vec3 pos = textComp.GetTransformComp()->GetAbsolutePosition();
  pos.x = 2 * pos.x - 1;
  pos.y = 2 * pos.y - 1;
  float xStartPos = pos.x;

  float scaling = textComp.GetTextScale() * 2.f;
  glm::vec3 scale(scaling / (screen.GetWidth()), scaling / (screen.GetHeight()), 1);

  static glm::mat4 MVP;
  char c;
  while ((c = *str))
  {
    const Font::CharInfo &charInfo = font[c];
    if (c == '\n')
    {
      pos.x = xStartPos;
      pos.y -= charInfo.Size.y * 2 * scale.y;
      ++str;
      continue;
    }

    // Render current character.
    MVP[0][0] = charInfo.Size.x * scale.x;                        // width
    MVP[1][1] = charInfo.Size.y * scale.y;                        // height
    MVP[3][0] = pos.x + charInfo.Bearing.x * scale.x;             // xpos
    MVP[3][1] = pos.y - (charInfo.Size.y - charInfo.Bearing.y) *  // ypos
                            scale.y;

    glUniformMatrix4fv(m_TextUniforms.m_WorldLoc, 1, GL_FALSE, &MVP[0][0]);

    glBindTexture(GL_TEXTURE_2D, charInfo.TextureID);
    glDrawElements(GL_TRIANGLES, m_UIPlane.m_IndexCount, GL_UNSIGNED_INT, 0);

    // Move forward by however much we need to.
    pos.x += (charInfo.Advance / 64.f) * scale.x;

    // Render next character.
    ++str;
  }
}

bool DrawSystem::OnInitialize()
{
  if (!SetupShaders())
  {
    LOG_ERROR("Failed to set up shaders\n");
    return false;
  }

  // Setup initial OpenGL state.
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

  // Create dithering texture.
  glGenTextures(1, &m_DitherTexture);
  glBindTexture(GL_TEXTURE_2D, m_DitherTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 8, 8, 0, GL_RED, GL_UNSIGNED_BYTE,
               &kDitherPattern[0]);

  // TODO Enable for wireframe drawing.
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  return true;
}

void DrawSystem::OnShutdown()
{
  glDeleteTextures(1, &m_DitherTexture);

  glDeleteProgram(m_WorldProgram);
  glDeleteProgram(m_UIProgram);
  glDeleteProgram(m_TextProgram);
}

bool DrawSystem::SetupShaders()
{
  // Setup default shader.
  ShaderProgram program(2);
  program.PushShader(GL_VERTEX_SHADER, SHADER_PATH + "world-vert.glsl");
  program.PushShader(GL_FRAGMENT_SHADER, SHADER_PATH + "world-frag.glsl");
  m_WorldProgram = program.Compile();
  if (m_WorldProgram == GL_NONE)
  {
    return false;
  }
  // Get default shader uniforms.
  if (!m_WorldUniforms.GetLocations(m_WorldProgram))
  {
    return false;
  }

  // Setup UI shader.
  program.PopShader();
  program.PushShader(GL_FRAGMENT_SHADER, SHADER_PATH + "ui-frag.glsl");
  m_UIProgram = program.Compile();
  if (m_UIProgram == GL_NONE)
  {
    return false;
  }
  // Get UI shader uniforms.
  if (!m_UIUniforms.GetLocations(m_UIProgram))
  {
    return false;
  }

  // Setup text shader.
  program.PopShader();
  program.PushShader(GL_FRAGMENT_SHADER, SHADER_PATH + "text-frag.glsl");
  m_TextProgram = program.Compile();
  if (m_TextProgram == GL_NONE)
  {
    return false;
  }
  // Get text shader uniforms.
  if (!m_TextUniforms.GetLocations(m_TextProgram))
  {
    return false;
  }

  return true;
}

}  // namespace tetrad
