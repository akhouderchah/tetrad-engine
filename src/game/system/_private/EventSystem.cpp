#include "EventSystem.h"
#include "InputConstants.h"
#include "ObserverComponent.h"
#include "CameraComponent.h"
#include "MovableComponent.h"
#include "Screen.h"

#include "UI/UIViewport.h"

EventSystem* EventSystem::s_pInputSystem = nullptr;

double prevX;
double prevY;
double mouseSensitivity = 1.f;

EventSystem::EventSystem()
{
}

EventSystem::~EventSystem()
{
}

bool EventSystem::Initialize()
{
	return true;
}

void EventSystem::Shutdown()
{
	UnmakeInputSystem();
}

void EventSystem::Tick(deltaTime_t dt)
{
	(void)dt;

	// Get events and such
	glfwPollEvents();

	Event event = m_EventQueue.Consume();
	while(event.event != EGE_END)
	{
		for(size_t i = 0; i < m_pObservers.size(); ++i)
		{
			m_pObservers[i]->Notify(event);
		}
		event = m_EventQueue.Consume();
	}
}

bool EventSystem::MakeInputSystem(Screen *pScreen)
{
	if(s_pInputSystem){ return false; }

	s_pInputSystem = this;

	glfwGetCursorPos(pScreen->GetWindow(), &prevX, &prevY);
	return true;
}

void EventSystem::UnmakeInputSystem()
{
	if(s_pInputSystem == this)
	{
		s_pInputSystem = nullptr;
	}
}

bool EventSystem::RegisterObserver(ObserverComponent& observer)
{
	// In debug mode, check to see if we're adding an observer multiple times
	#ifdef _DEBUG
	for(size_t i = 0; i < m_pObservers.size(); ++i)
	{
		if(m_pObservers[i] == &observer)
		{
			LOG_SPECIAL("WARNING", "The observer at memory location: " << &observer << " is being registered multiple times to the same EventSystem!\n");
		}
	}
	#endif

	m_pObservers.push_back(&observer);

	return true;
}

void EventSystem::UnregisterObserver(ObserverComponent& observer)
{
	for(size_t i = 0; i < m_pObservers.size(); ++i)
	{
		if(m_pObservers[i] == &observer)
		{
			std::swap(m_pObservers[i], m_pObservers.back());
			m_pObservers.pop_back();
			return;
		}
	}
}

void EventSystem::Inform(const Event& event)
{
	// @TODO - log error if it occurs
	m_EventQueue.PushEvent(event);
}

void EventSystem::SetMouseSensitivity(double sensitivity)
{
	mouseSensitivity = sensitivity;
}

void KeyCallback(GLFWwindow*, int key, int scancode, int action, int mods)
{
	(void)scancode; (void)mods;
	Event event;

	DEBUG_ASSERT(EventSystem::s_pInputSystem);

	// @TODO - GO THROUGH AN INPUT MAPPER!!!!
	if(action != GLFW_REPEAT)
	{
		switch(key)
		{
		case GLFW_KEY_ESCAPE:
			event.event = EGE_PAUSE;
			break;
		case GLFW_KEY_Z:
			event.event = EGE_PLAYER1_JUMP;
			break;
		case GLFW_KEY_X:
			event.event = EGE_PLAYER2_JUMP;
			break;
		case GLFW_KEY_W:
			event.event = EGE_PLAYER1_FORWARDS;
			break;
		case GLFW_KEY_A:
			event.event = EGE_PLAYER1_LEFT;
			break;
		case GLFW_KEY_S:
			event.event = EGE_PLAYER1_BACKWARDS;
			break;
		case GLFW_KEY_D:
			event.event = EGE_PLAYER1_RIGHT;
			break;
		default:
			event.event = EGE_NONE;
		}

		if(action == GLFW_PRESS){ event.action = EEventAction::ON; }
		else if(action == GLFW_RELEASE){ event.action = EEventAction::OFF; }

		// @TODO we need to store the game mode and use it to set this
		event.state = EGameState::STARTED;
		EventSystem::s_pInputSystem->Inform(event);
	}
}

void CursorCallback(GLFWwindow*, double currX, double currY)
{
	// TODO use current UIViewport & CallbackContext!!!
	// Get current viewport - NOTE, THIS ASSUMES ONE VIEWPORT!
	static ConstVector<UIViewport*> pViewports =
		EntityManager::GetAll<UIViewport>();
	static screenBound_t screenBounds(0,0,0,0);
	screenBounds = pViewports[1]->GetScreenBounds();
	CameraComponent *pCamera = pViewports[1]->GetCamera();
	Screen *pScreen = pViewports[1]->GetScreen();

	DEBUG_ASSERT(pScreen);

	if(!pCamera)
	{
		prevX = currX;
		prevY = currY;
	}

	// Calculate normalized x & y diffs, then scale by sensitivity factor
	double xDiff = mouseSensitivity * (currX - prevX) /
		(pScreen->GetWidth()*(screenBounds.points[1].X - screenBounds.points[0].X));
	double yDiff = mouseSensitivity * (currY - prevY) /
		(pScreen->GetHeight()*(screenBounds.points[1].Y - screenBounds.points[0].Y));

	// Apply appropriate rotations to camera
	TransformDirs localDirs = pCamera->m_pTransformComp->GetLocalDirs();
	pCamera->m_pMover->Rotate(-yDiff, localDirs.rightDir);
	pCamera->m_pMover->Rotate(-xDiff, glm::vec3(0, 1, 0));

	// Store current cursor position
	prevX = currX;
	prevY = currY;
}
