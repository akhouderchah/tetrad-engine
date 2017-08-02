#include "editor/Editor.h"

#include "EntityManager.h"
#include "EventSystem.h"
#include "ObserverComponent.h"
#include "CameraComponent.h"
#include "AttachComponent.h"
#include "PhysicsSystem.h"

#include "UI/UIButton.h"

void ViewportButtonCallback(GLFWwindow*, int, int, int);
void GUICursorCallback(GLFWwindow*, double, double);
void ResizeCallback(GLFWwindow*, int, int);
double xpos, ypos;
Screen *pCurrentScreen = nullptr;

UIComponent *pPrevUI = nullptr;
UIComponent *pPrevValidUI = nullptr;

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

	pCurrentScreen = m_pScreen;

	// Setup mouse callbacks
	glfwGetCursorPos(m_pWindow, &xpos, &ypos);
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

	for(int i = 0; i < 10; ++i){
	// Create test window
	entity = EntityManager::CreateEntity();
	entity.Add<TransformComponent>()->Init(glm::vec3(.5, .5, 0), glm::vec3(1.f/20,
																			 1.f/21,
																			 1));
	entity.Add<MovableComponent>();
	auto pUI = entity.Add<UIComponent>();
	pUI->SetCurrentTexture(PAUSE_BACKGROUND_PATH, TextureType::RGBA);
	m_pScreen->Inform(pUI, Screen::EIT_CREATED);

	// Create test button
	entity = EntityManager::CreateEntity();
	entity.Add<TransformComponent>()->Init(glm::vec3(1, 1, 1),
										   glm::vec3(.5, .5, 1));
	entity.Add<MovableComponent>();
	entity.Add<AttachComponent>()->Attach(pUI->GetEntity());
	auto pButton = entity.Add<UIButton>();
	pButton->SetTextures(TEXTURE_PATH + "UI/BTN_Exit.tga", PAUSE_BACKGROUND_PATH, PAUSE_BACKGROUND_PATH);
	m_pScreen->Inform(pButton, Screen::EIT_CREATED);
	}

	m_Timer.Start();
	return true;
}

void Editor::AddSystems()
{
	m_pInputSystem = new EventSystem;
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
		if(pPrevUI)
		{
			if(action == GLFW_PRESS)
			{
				pPrevUI->OnTouchEnter();
			}
			else //action == GLFW_RELEASE
			{
				pPrevUI->OnTouchLeave();
				pPrevValidUI = nullptr;
			}
		}
		else if(action == GLFW_RELEASE)
		{
			if(pPrevValidUI)
			{
				pPrevValidUI->OnTouchLeave();
				pPrevValidUI = nullptr;
			}
			// TODO Raytrace & select object
		}
		break;
	default:
		break;
	}
}

void GUICursorCallback(GLFWwindow*, double currX, double currY)
{
	static double prevX = xpos;
	static double prevY = ypos;

	// Inform elements of hover enter & exits
	currY = pCurrentScreen->GetHeight() - currY - 1;
	UIComponent *pUI = pCurrentScreen->FindElementAt(currX, currY);
	if(pUI)
	{
		pPrevValidUI = pUI;
	}

	if(pUI != pPrevUI)
	{
		if(pPrevUI)
		{
			pPrevUI->OnHoverLeave();
		}
		if(pUI)
		{
			pUI->OnHoverEnter();
		}
	}

	if(pPrevValidUI && pPrevValidUI->IsFollowingCursor())
	{
		double xDiff = (currX - prevX) / pCurrentScreen->GetWidth();
		double yDiff = (currY - prevY) / pCurrentScreen->GetHeight();

		// TODO - we may need to modify the z also!
		pPrevValidUI->GetMover()->Move(glm::vec3(xDiff, yDiff, 0));
		pCurrentScreen->Inform(pPrevValidUI, Screen::EIT_UPDATED);
	}

	pPrevUI = pUI;

	prevX = currX;
	prevY = currY;
}

void ResizeCallback(GLFWwindow*, int width, int height)
{
	glViewport(0, 0, width, height);
	pCurrentScreen->SetSize(width, height);
}
