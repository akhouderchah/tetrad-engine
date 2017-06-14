#include "TetradGame.h"

#include "EntityManager.h"
#include "ErrorSystem.h"
#include "EventSystem.h"
#include "ObserverComponent.h"
#include "PhysicsSystem.h"

TetradGame::TetradGame() :
	m_pDrawSystem(nullptr), m_pSystemObserver(nullptr),
	m_pInputSystem(nullptr)
{
}

bool TetradGame::Initialize(const GameAttributes& attributes)
{
	// Game class contains important initializing functionality
	if(!Game::Initialize(attributes))
	{
		ERROR("Failed to initialize engine systems!\n", EEB_CONTINUE);
		return false;
	}

	// Create background
	// TODO - transform values only work for this particular aspect ratio
	Entity entity = EntityManager::CreateEntity();
	entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 1), glm::vec3(3.f, 2.3f, 1));
	entity.Add<MovableComponent>();
	DrawComponent *pDraw = entity.Add<DrawComponent>();
	pDraw->SetGeometry(ShapeType::PLANE);
	pDraw->SetTexture(BACKGROUND_PATH, TextureType::RGB);
	entity.Add<MaterialComponent>()->SetScrollRate(-0.2f);

	// Create floor
	entity = EntityManager::CreateEntity();
	entity.Add<TransformComponent>()->Init(glm::vec3(0,-2.6f,0.f),
										   glm::vec3(3.75f,0.2f,1));
	entity.Add<MovableComponent>();
	pDraw = entity.Add<DrawComponent>();
	pDraw->SetGeometry(ShapeType::PLANE);
	pDraw->SetTexture(FLOOR_PATH, TextureType::RGBA);
	entity.Add<MaterialComponent>()->SetScrollRate(-0.75f);

	// Create camera
	entity = EntityManager::CreateEntity();
	entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 5));
	entity.Add<MovableComponent>();
	entity.Add<PhysicsComponent>()->SetGravity(false);
	CameraComponent *pCamera = entity.Add<CameraComponent>();
	m_pDrawSystem->SetCurrentCamera(pCamera);
	ObserverComponent* pObserver = entity.Add<ObserverComponent>();
	pObserver->Subscribe(*m_pInputSystem);
	pObserver->AddEvent(EGameEvent(EGE_PLAYER1_LEFT),
						new Action_Move(entity, Action_Move::EMD_LEFT));
	pObserver->AddEvent(EGameEvent(EGE_PLAYER1_RIGHT),
						new Action_Move(entity, Action_Move::EMD_RIGHT));
	pObserver->AddEvent(EGameEvent(EGE_PLAYER1_FORWARDS),
						new Action_Move(entity, Action_Move::EMD_FORWARDS));
	pObserver->AddEvent(EGameEvent(EGE_PLAYER1_BACKWARDS),
						new Action_Move(entity, Action_Move::EMD_BACKWARDS));

	// Create jumping boxes
	for(int i = 0; i < 2; ++i)
	{
		entity = EntityManager::CreateEntity();
		entity.Add<TransformComponent>()->Init(glm::vec3(i*.5, 0.f, 1.f),
											 glm::vec3(.2f, .2f, .2f));
		entity.Add<MovableComponent>();
		pDraw = entity.Add<DrawComponent>();
		pDraw->SetGeometry(MODEL_PATH + "suzanne.obj");
		pDraw->SetTexture(TEXTURE_PATH + "Black.tga", TextureType::RGB);
		entity.Add<PhysicsComponent>();
		ObserverComponent* pObserver = entity.Add<ObserverComponent>();
		pObserver->Subscribe(*m_pInputSystem);
		pObserver->AddEvent(EGameEvent(EGE_PLAYER1_JUMP + i), new Action_Jump(entity));
	}

	// Create fade screen entity
	entity = EntityManager::CreateEntity();
	entity.Add<TransformComponent>()->Init(glm::vec3(0,0,1),
										   glm::vec3(3.1, 2.5f, 1.f));
	pDraw = entity.Add<DrawComponent>();
	pDraw->SetGeometry(ShapeType::PLANE);
	pDraw->SetTexture(PAUSE_BACKGROUND_PATH, TextureType::RGBA);
	entity.Add<MaterialComponent>()->SetOpacity(0.f);
	Action_PauseGame::SetFadeScreen(entity);

	m_Timer.Start();

	return true;
}

void TetradGame::AddSystems()
{
	m_pInputSystem = new EventSystem;
	m_pInputSystem->MakeInputSystem();
	m_pSystems.push_back(m_pInputSystem);

	m_pSystems.push_back(new PhysicsSystem);

	m_pDrawSystem = new DrawSystem;
	m_pSystems.push_back(m_pDrawSystem);

	// Create the system observer
	m_pSystemObserver = EntityManager::CreateEntity().Add<ObserverComponent>();
	m_pSystemObserver->Subscribe(*m_pInputSystem);
	m_pSystemObserver->AddEvent(EGE_PAUSE, new Action_PauseGame(m_pWindow));
}
