#include "tetrad-game/TetradGame.h"

#include "core/ErrorSystem.h"
#include "core/Paths.h"
#include "engine/ecs/EntityManager.h"
#include "engine/event/EventSystem.h"
#include "engine/event/ObserverComponent.h"
#include "engine/game/Action_PauseGame.h"
#include "engine/physics/Action_Jump.h"
#include "engine/physics/Action_Move.h"
#include "engine/physics/PhysicsComponent.h"
#include "engine/physics/PhysicsSystem.h"
#include "engine/render/CameraComponent.h"
#include "engine/render/DrawSystem.h"
#include "engine/render/MaterialComponent.h"
#include "engine/transform/AttachComponent.h"
#include "engine/transform/MovableComponent.h"
#include "engine/transform/TransformComponent.h"
#include "engine/ui/TextComponent.h"
#include "engine/ui/UI.h"
#include "tetrad-game/GameplaySystem.h"
#include "tetrad-game/obstacle/ObstacleFactoryComponent.h"

namespace tetrad
{

TetradGame::TetradGame()
    : m_pDrawSystem(nullptr), m_pSystemObserver(nullptr), m_pInputSystem(nullptr)
{
}

void TetradGame::OnInitialized()
{
  ErrorSystem::SetCurrentGame(this);

  // Create background
  // TODO - transform values only work for this particular aspect ratio
  Entity entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 0), glm::vec3(3.f, 2.3f, 1));
  entity.Add<MovableComponent>();
  DrawComponent *pDraw = entity.Add<DrawComponent>();
  pDraw->SetGeometry(ShapeType::PLANE);
  pDraw->SetTexture(BACKGROUND_PATH, TextureType::RGB);
  entity.Add<MaterialComponent>()->SetScrollRate(-0.2f);

  // Create floor
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, -2.6f, 0.1f),
                                         glm::vec3(3.75f, 0.2f, 1));
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
  ObserverComponent *pObserver = entity.Add<ObserverComponent>();
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
  for (int i = 0; i < 2; ++i)
  {
    entity = EntityManager::CreateEntity();
    entity.Add<TransformComponent>()->Init(glm::vec3(i, 0.f, 1.f),
                                           glm::vec3(.2f, .2f, .2f));
    entity.Add<MovableComponent>();
    pDraw = entity.Add<DrawComponent>();
    pDraw->SetGeometry(MODEL_PATH + "suzanne.obj");
    pDraw->SetTexture(TEXTURE_PATH + "Black.tga", TextureType::RGB);
    entity.Add<PhysicsComponent>();
    ObserverComponent *pObserver = entity.Add<ObserverComponent>();
    pObserver->Subscribe(*m_pInputSystem);
    pObserver->AddEvent(EGameEvent(EGE_PLAYER1_JUMP + i), new Action_Jump(entity));

    if (i == 0)
    {
      pCamera->GetEntity().Add<AttachComponent>()->Attach(entity);
    }

    // Attach box to players
    Entity attachment = EntityManager::CreateEntity();
    attachment.Add<TransformComponent>()->Init(glm::vec3(1, 1, 1));
    attachment.Add<AttachComponent>()->Attach(entity);
    pDraw = attachment.Add<DrawComponent>();
    pDraw->SetGeometry(MODEL_PATH + "cube.obj");
    pDraw->SetTexture(TEXTURE_PATH + "Black.tga", TextureType::RGB);
    if (i == 0)
    {
      attachment.Add<PhysicsComponent>()->SetGravity(false);
      attachment.Add<MovableComponent>();
    }
  }

  // Create fade screen entity
  m_FadeScreen = EntityManager::CreateEntity();
  m_FadeScreen.Add<TransformComponent>()->Init(glm::vec3(0, 0, 1),
                                               glm::vec3(3.1, 2.5f, 1.f));
  pDraw = m_FadeScreen.Add<DrawComponent>();
  pDraw->SetGeometry(ShapeType::PLANE);
  pDraw->SetTexture(PAUSE_BACKGROUND_PATH, TextureType::RGBA);
  m_FadeScreen.Add<MaterialComponent>()->SetOpacity(0.f);
  m_pSystemObserver->AddEvent(EGE_PAUSE, new Action_PauseGame(this));

  // Create text
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(.8, .9, 1), glm::vec3(.8, .8, 1));
  TextComponent *pText = entity.Add<TextComponent>();
  pText->SetFont(ResourceManager::LoadFont(STANDARD_FONT_PATH));
  pText->SetText("Score: 0");

  // Create factory
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 2), glm::vec3(1, 1, 1));
  entity.Add<ObstacleFactoryComponent>()->Enable();

  // Create viewport
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 1), glm::vec3(1.f, 1.f, 1.f));
  UIViewport *pViewport = entity.Add<UIViewport>();
  pViewport->SetCamera(pCamera);
  pViewport->Init(m_MainScreen);
}

void TetradGame::AddSystems()
{
  m_pInputSystem = new EventSystem;
  m_pInputSystem->MakeInputSystem();
  AppendSystem(m_pInputSystem);

  AppendSystem(new PhysicsSystem);

  m_pDrawSystem = new DrawSystem;
  AppendSystem(m_pDrawSystem);

  AppendSystem(new GameplaySystem);

  // Create the system observer
  m_pSystemObserver = EntityManager::CreateEntity().Add<ObserverComponent>();
  m_pSystemObserver->Subscribe(*m_pInputSystem);
}

void TetradGame::OnPause()
{
  EntityManager::GetComponent<MaterialComponent>(m_FadeScreen)->FadeIn(.5f);

  // Create text
  m_PauseText = EntityManager::CreateEntity();
  m_PauseText.Add<TransformComponent>()->Init(glm::vec3(.45, .8, 1), glm::vec3(1, 1, 1));
  TextComponent *pText = m_PauseText.Add<TextComponent>();
  pText->SetFont(ResourceManager::LoadFont(STANDARD_FONT_PATH));
  pText->SetText("PAUSED");

  // Create exit button
  m_ExitText = EntityManager::CreateEntity();
  m_ExitText.Add<TransformComponent>()->Init(glm::vec3(.45, .2, 1), glm::vec3(1, 1, 1));
  auto textComp = m_ExitText.Add<TextComponent>();
  textComp->SetFont(ResourceManager::LoadFont(STANDARD_FONT_PATH));
  textComp->SetText("Exit");
  textComp->SetTextScale(.8f);
  auto pButton = m_ExitText.Add<UIButton>();
  pButton->SetTextures(TEXTURE_PATH + "UI/BTN_Exit.tga", PAUSE_BACKGROUND_PATH,
                       PAUSE_BACKGROUND_PATH);
  pButton->Init(m_MainScreen);

  // Set mouse behavior
  GLFWwindow *pWindow = m_MainScreen.GetWindow();
  glfwGetCursorPos(pWindow, &m_PrevX, &m_PrevY);
  glfwSetInputMode(pWindow, GLFW_CURSOR, (uint32_t)MouseMode::NORMAL);
  glfwSetCursorPosCallback(pWindow, CallbackContext::Cursor_GUI);
}

void TetradGame::OnResume()
{
  EntityManager::GetComponent<MaterialComponent>(m_FadeScreen)->FadeOut(.25f);

  EntityManager::DestroyEntity(m_PauseText);

  m_MainScreen.Inform(m_ExitText.GetAs<UIButton>(), Screen::EIT_DELETED);
  EntityManager::DestroyEntity(m_ExitText);

  // Set mouse behavior TODO - restore saved behavior instead?
  GLFWwindow *pWindow = m_MainScreen.GetWindow();

  glfwSetInputMode(pWindow, GLFW_CURSOR, (uint32_t)MouseMode::DISABLED);
  glfwSetCursorPos(pWindow, m_PrevX, m_PrevY);
  glfwSetCursorPosCallback(pWindow, CallbackContext::Cursor_3DCamera);
}

}  // namespace tetrad
