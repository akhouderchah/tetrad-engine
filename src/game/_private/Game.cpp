#include "Game.h"
#include "EntityManager.h"
#include "ErrorSystem.h"
#include "EventSystem.h"
#include "CameraComponent.h"

#include <chrono>

using namespace glm;

GameAttributes::GameAttributes(
	int32_t width,
	int32_t height,
	std::string title,
	bool fullscreen,
	bool isResizable,
	MouseMode mouseMode,
	uint8_t samples,
	uint8_t screenPartitionRows,
	uint8_t screenPartitionCols) :

	m_Width(width),
	m_Height(height),
	m_WindowTitle(title),
	m_Fullscreen(fullscreen),
	m_IsResizable(isResizable),
	m_MouseMode(mouseMode),
	m_SampleCount(samples),
	m_ScreenPartitionRows(screenPartitionRows),
	m_ScreenPartitionCols(screenPartitionCols)
{}

Game::Game() :
	m_pScreen(nullptr), m_pWindow(nullptr),
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
	if(!CreatePrimaryWindow(attributes))
	{
		LOG_ERROR("Failed to create a primary window!\n");
		return false;
	}

	m_pScreen = new Screen(attributes.m_Width, attributes.m_Height,
						   attributes.m_ScreenPartitionRows,
						   attributes.m_ScreenPartitionCols);
	glViewport(0, 0, attributes.m_Width, attributes.m_Height);

	glewExperimental = true;
	if(glewInit() != GLEW_OK)
	{
		LOG_ERROR("Failed to initialize glew!\n");
		return false;
	}

	// Setup keyboard input
	glfwSetInputMode(m_pWindow, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(m_pWindow, KeyCallback);

	// Setup mouse input
	glfwSetInputMode(m_pWindow, GLFW_CURSOR, (uint32_t)attributes.m_MouseMode);
	glfwSetCursorPosCallback(m_pWindow, CursorCallback);

	// TODO - Should we just duplicate this pointer for each
	// system in order to allow for multiple windows?
	ISystem::SetWindow(m_pWindow);

	// Initialize systems
	EntityManager::Initialize();
	AddSystems();
	DEBUG_LOG("Finished adding game systems\n");

	for(size_t i = 0; i < m_pSystems.size(); ++i)
	{
		if(!m_pSystems[i]->Initialize())
		{
			LOG_ERROR("Failed to initialize system: " << i << "\n");
			return false;
		}
	}
	DEBUG_LOG("Finished initializing game systems\n");

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

	if(m_pScreen)
	{
		delete m_pScreen;
		m_pScreen = nullptr;
	}

	glfwTerminate();
}

void Game::Run()
{
	deltaTime_t deltaTime;
	deltaTime_t invAlpha = 1 - m_AvgDeltaAlpha;

	while(!glfwWindowShouldClose(m_pWindow))
	{
		deltaTime = m_Timer.Tick();

		// delta EMWA calculation
		m_AvgDelta = (m_AvgDeltaAlpha * deltaTime) + (invAlpha * m_AvgDelta);

		// Tick systems
		for(size_t i = 0; i < m_pSystems.size(); ++i)
		{
			m_pSystems[i]->Tick(deltaTime);
		}
	}
}

void Game::Reset()
{
}

bool Game::CreatePrimaryWindow(const GameAttributes& attributes)
{
	if(m_pWindow != nullptr)
	{
		glfwDestroyWindow(m_pWindow);
	}

	GLFWmonitor* pMonitor = nullptr;
	if(attributes.m_Fullscreen)
	{
		pMonitor = glfwGetPrimaryMonitor();
	}

	glfwWindowHint(GLFW_SAMPLES, attributes.m_SampleCount);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, attributes.m_IsResizable);

	// Create window and inform cameras of window size
	m_pWindow = glfwCreateWindow(attributes.m_Width, attributes.m_Height, attributes.m_WindowTitle.c_str(), pMonitor, NULL);

	CameraComponent::SetWindowSize(attributes.m_Width, attributes.m_Height);

	if(!m_pWindow)
	{
		LOG_ERROR("Failed to create the GLFW window!");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(m_pWindow);

	return true;
}

