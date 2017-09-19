#include "Game.h"
#include "EntityManager.h"
#include "ErrorSystem.h"
#include "EventSystem.h"
#include "CameraComponent.h"
#include "CallbackContext.h"

#include "TextComponent.h"

#include <chrono>

using namespace glm;

GameAttributes::GameAttributes(
    ScreenAttributes mainWindowAttr,
	MouseMode mouseMode
	):

	m_MainWindowAttr(mainWindowAttr),
	m_MouseMode(mouseMode)
{}

Game::Game() :
	m_CurrentState(EGameState::DISABLED),
	m_PrevState(EGameState::DISABLED),
	m_AvgDelta(.01666667), m_AvgDeltaAlpha(.125)
{
}

Game::~Game()
{
}

bool Game::Initialize(const GameAttributes& attributes)
{
	m_Random.SetSeed(static_cast<unsigned long>(std::chrono::system_clock::now().time_since_epoch().count()));

	if(!glfwInit())
	{
		LOG_ERROR("Failed to initialize glfw!\n");
		return false;
	}

	// Create window
	if(!m_MainScreen.Initialize(attributes.m_MainWindowAttr))
	{
		LOG_ERROR("Failed to create the primary window!\n");
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
	if(glewInit() != GLEW_OK)
	{
		LOG_ERROR("Failed to initialize glew!\n");
		glfwTerminate();
		return false;
	}

	// TODO - Should we just duplicate this pointer for each
	// system in order to allow for multiple windows?
	ISystem::SetScreen(&m_MainScreen);

	// Initialize systems
	EntityManager::Initialize();
	AddSystems();
	DEBUG_LOG("Finished adding game systems\n");

	for(size_t i = 0; i < m_pSystems.size(); ++i)
	{
		if(!m_pSystems[i]->Initialize(this))
		{
			LOG_ERROR("Failed to initialize system: " << i << "\n");
			glfwTerminate();
			return false;
		}
	}
	DEBUG_LOG("Finished initializing game systems\n");

	m_CurrentState = EGameState::STARTED;
	return true;
}

void Game::Shutdown()
{
	for(size_t i = m_pSystems.size(); i > 0;)
	{
		m_pSystems[--i]->Shutdown();
		delete m_pSystems[i];
	}

	EntityManager::Shutdown();

	m_MainScreen.Shutdown();
	glfwTerminate();
}

void Game::Run()
{
	deltaTime_t deltaTime;
	deltaTime_t invAlpha = 1 - m_AvgDeltaAlpha;

#ifdef _DEBUG
	Entity fpsText = EntityManager::CreateEntity();
	fpsText.Add<TransformComponent>()->Init(glm::vec3(0.1, 0.9, 1));
	TextComponent *pText = fpsText.Add<TextComponent>();
	pText->SetFont(ResourceManager::LoadFont(STANDARD_FONT_PATH));
	pText->SetColor(glm::vec4(.9f, .9f, .9f, .75f));
	char fps[8];
#endif

	while(!glfwWindowShouldClose(m_MainScreen.GetWindow()))
	{
		deltaTime = m_Timer.Tick();

		// delta EMWA calculation
		m_AvgDelta = (m_AvgDeltaAlpha * deltaTime) + (invAlpha * m_AvgDelta);

		#ifdef _DEBUG
		sprintf(fps, "%7.2f", 1.f/m_AvgDelta);
		pText->SetText(std::string("FPS: ") + fps);
		#endif

		// Tick systems
		for(size_t i = 0; i < m_pSystems.size(); ++i)
		{
			m_pSystems[i]->Tick(deltaTime);
		}
	}
}

bool Game::Pause()
{
	if(m_CurrentState != EGameState::PAUSED)
	{
		m_PrevState = m_CurrentState;
		m_CurrentState = EGameState::PAUSED;

		return true;
	}
	return false;
}

bool Game::Unpause()
{
	if(m_CurrentState == EGameState::PAUSED)
	{
		m_CurrentState = m_PrevState;

		return true;
	}
	return false;
}

void Game::Reset()
{
}
