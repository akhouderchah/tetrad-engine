#include "engine/game/Game.h"

#include <chrono>

#include "core/ErrorSystem.h"
#include "core/Paths.h"
#include "core/Rand.h"
#include "engine/ecs/EntityManager.h"
#include "engine/game/CallbackContext.h"
#include "engine/render/CameraComponent.h"
#include "engine/resource/ResourceManager.h"
#include "engine/transform/TransformComponent.h"
#include "engine/ui/TextComponent.h"

using namespace glm;

namespace tetrad
{

GameAttributes::GameAttributes(ScreenAttributes mainWindowAttr, MouseMode mouseMode)
    : m_MainWindowAttr(mainWindowAttr), m_MouseMode(mouseMode)
{
}

Game::Game()
    : m_CurrentState(EGameState::DISABLED),
      m_PrevState(EGameState::DISABLED),
      m_DeltaAvg(.01666667),
      m_DeltaAlpha(.125),
      m_JitterAvg(0),
      m_JitterAlpha(.25)
{
}

bool Game::Initialize(const GameAttributes &attributes)
{
  // Ensures that the static Random instance gets constructed
  Random::GetGlobalInstance();

  if (!glfwInit())
  {
    LOG_ERROR("Failed to initialize glfw\n");
    return false;
  }

  EntityManager::Initialize();

  // Create window
  if (!m_MainScreen.Initialize(attributes.m_MainWindowAttr))
  {
    LOG_ERROR("Failed to create the primary window\n");
    glfwTerminate();
    return false;
  }

  // Setup keyboard & mouse input
  GLFWwindow *pWindow = m_MainScreen.GetWindow();

  glfwSetInputMode(pWindow, GLFW_CURSOR, (uint32_t)attributes.m_MouseMode);
  CallbackContext::SetGame(this);
  CallbackContext::SetScreen(&m_MainScreen);

  glfwSetKeyCallback(pWindow, CallbackContext::Keyboard_3DCamera);
  glfwSetCursorPosCallback(pWindow, CallbackContext::Cursor_3DCamera);
  glfwSetWindowSizeCallback(pWindow, CallbackContext::Resize_Default);

  glewExperimental = true;
  if (glewInit() != GLEW_OK)
  {
    LOG_ERROR("Failed to initialize glew\n");
    glfwTerminate();
    return false;
  }

  // Initialize systems
  AddSystems();
  DEBUG_LOG("Finished adding game systems\n");

  for (size_t i = 0; i < m_pSystems.size(); ++i)
  {
    if (!m_pSystems[i]->Initialize(this))
    {
      LOG_ERROR("Failed to initialize system: " << i << "\n");
      glfwTerminate();
      return false;
    }
  }
  DEBUG_LOG("Finished initializing game systems\n");

  OnInitialized();
  m_CurrentState = EGameState::STARTED;
  m_Timer.Start();
  return true;
}

void Game::Shutdown()
{
  for (size_t i = m_pSystems.size(); i > 0;)
  {
    m_pSystems[--i]->Shutdown();
    delete m_pSystems[i];
  }

  m_MainScreen.Shutdown();
  glfwTerminate();

  EntityManager::Shutdown();
}

void Game::Run()
{
  deltaTime_t deltaTime;
  deltaTime_t deltaInvAlpha = 1 - m_DeltaAlpha;
  deltaTime_t jitterInvAlpha = 1 - m_JitterAlpha;

#ifdef _DEBUG
  Entity fpsText = EntityManager::CreateEntity();
  fpsText.Add<TransformComponent>()->Init(glm::vec3(0.1, 0.9, 1));
  TextComponent *pText = fpsText.Add<TextComponent>();
  pText->SetFont(ResourceManager::LoadFont(STANDARD_FONT_PATH));
  pText->SetColor(glm::vec4(.9f, .9f, .9f, .75f));

  char fpsStr[8];
  char jitterStr[8];
#endif

  while (!glfwWindowShouldClose(m_MainScreen.GetWindow()))
  {
    deltaTime = m_Timer.Tick();

    // delta EMWA calculation
    m_DeltaAvg = (m_DeltaAlpha * deltaTime) + (deltaInvAlpha * m_DeltaAvg);
    m_JitterAvg =
        (m_JitterAlpha * abs(deltaTime - m_DeltaAvg)) + (jitterInvAlpha * m_JitterAvg);

#ifdef _DEBUG
    snprintf(fpsStr, sizeof(fpsStr), "%7.2f", 1.f / m_DeltaAvg);
    snprintf(jitterStr, sizeof(jitterStr), "%7.2f", m_JitterAvg * 1000);
    pText->SetText(std::string("FPS: ") + fpsStr + "\nJitter (ms):" + jitterStr);
#endif

    // Tick systems
    for (size_t i = 0; i < m_pSystems.size(); ++i)
    {
      m_pSystems[i]->Tick(deltaTime);
    }
  }
}

bool Game::Pause()
{
  if (m_CurrentState != EGameState::PAUSED)
  {
    m_PrevState = m_CurrentState;
    m_CurrentState = EGameState::PAUSED;

    OnPause();
    return true;
  }
  return false;
}

bool Game::Resume()
{
  if (m_CurrentState == EGameState::PAUSED)
  {
    m_CurrentState = m_PrevState;

    OnResume();
    return true;
  }
  return false;
}

void Game::Reset() {}

}  // namespace tetrad
