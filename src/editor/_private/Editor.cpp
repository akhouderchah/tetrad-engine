#include "editor/Editor.h"

#include "EntityManager.h"
#include "EventSystem.h"
#include "ObserverComponent.h"
#include "CameraComponent.h"
#include "AttachComponent.h"
#include "PhysicsSystem.h"

void ViewportButtonCallback(GLFWwindow*, int, int, int);
void GUICursorCallback(GLFWwindow*, double, double);
void ResizeCallback(GLFWwindow*, int, int);
double xpos, ypos;

Editor::Editor() :
	m_pInputSystem(nullptr), m_pDrawSystem(nullptr),
	m_pSystemObserver(nullptr)
{
}

bool Editor::Initialize(const GameAttributes &attributes)
{
	// Initialize base class first
	if(!Game::Initialize(attributes))
	{
		ERROR("Failed to initialize engine systems!\n", EEB_CONTINUE);
		return false;
	}

	// Setup mouse callbacks
	glfwSetCursorPosCallback(m_pWindow, GUICursorCallback);
	glfwSetMouseButtonCallback(m_pWindow, ViewportButtonCallback);

	// Setup window resize callback
	glfwSetWindowSizeCallback(m_pWindow, ResizeCallback);

	Entity entity = EntityManager::CreateEntity();
	entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 1), glm::vec3(3.f, 2.3f, 1));
	entity.Add<MovableComponent>();
	entity.Add<AttachComponent>();
	DrawComponent *pDraw = entity.Add<DrawComponent>();
	pDraw->SetGeometry(ShapeType::PLANE);
	pDraw->SetTexture(BACKGROUND_PATH, TextureType::RGB);
	entity.Add<MaterialComponent>();

	m_CameraEntity = EntityManager::CreateEntity();
	m_CameraEntity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 5));
	m_CameraEntity.Add<MovableComponent>();
	m_CameraEntity.Add<PhysicsComponent>()->SetGravity(false);
	CameraComponent *pCamera = m_CameraEntity.Add<CameraComponent>();
	pCamera->SetCurrentCamera();
	ObserverComponent* pObserver = m_CameraEntity.Add<ObserverComponent>();
	pObserver->Subscribe(*m_pInputSystem);
	pObserver->AddEvent(EGameEvent(EGE_PLAYER1_LEFT),
						new Action_Move(m_CameraEntity, Action_Move::EMD_LEFT));
	pObserver->AddEvent(EGameEvent(EGE_PLAYER1_RIGHT),
						new Action_Move(m_CameraEntity, Action_Move::EMD_RIGHT));
	pObserver->AddEvent(EGameEvent(EGE_PLAYER1_FORWARDS),
						new Action_Move(m_CameraEntity, Action_Move::EMD_FORWARDS));
	pObserver->AddEvent(EGameEvent(EGE_PLAYER1_BACKWARDS),
						new Action_Move(m_CameraEntity, Action_Move::EMD_BACKWARDS));

	m_Timer.Start();
	return true;
}

void Editor::AddSystems()
{
	m_pInputSystem = new EventSystem;
	glfwGetCursorPos(m_pWindow, &xpos, &ypos);
	m_pInputSystem->MakeInputSystem(m_pWindow);
	m_pSystems.push_back(m_pInputSystem);

	m_pSystems.push_back(new PhysicsSystem);

	m_pDrawSystem = new DrawSystem;
	m_pSystems.push_back(m_pDrawSystem);

	// Create the system observer
	m_pSystemObserver = EntityManager::CreateEntity().Add<ObserverComponent>();
	m_pSystemObserver->Subscribe(*m_pInputSystem);
	m_pSystemObserver->AddEvent(EGE_PAUSE, new Action_ExitGame(m_pWindow));
}

void ViewportButtonCallback(GLFWwindow *pWindow, int button, int action, int mods)
{
	(void)mods;
	static uint32_t prevCursorMode = glfwGetInputMode(pWindow, GLFW_CURSOR);

	switch(button)
	{
	case GLFW_MOUSE_BUTTON_RIGHT:
		if(action == GLFW_PRESS)
		{
			glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(pWindow, xpos, ypos);
			glfwSetCursorPosCallback(pWindow, CursorCallback);
		}
		else
		{
			glfwSetCursorPosCallback(pWindow, GUICursorCallback);
			glfwGetCursorPos(pWindow, &xpos, &ypos);
			glfwSetInputMode(pWindow, GLFW_CURSOR, prevCursorMode);
		}
		break;
	case GLFW_MOUSE_BUTTON_LEFT:
		if(action == GLFW_RELEASE)
		{
			// TODO Raytrace & select object
		}
		break;
	default:
		break;
	}
}

void GUICursorCallback(GLFWwindow*, double, double)
{
	// TODO Find currently hovered UI element
}

void ResizeCallback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
}
