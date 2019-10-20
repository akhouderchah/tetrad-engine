#include "editor/Editor.h"

#include "core/Log.h"
#include "core/Paths.h"
#include "engine/ecs/EntityManager.h"
#include "engine/event/EventSystem.h"
#include "engine/event/ObserverComponent.h"
#include "engine/game/Action_ExitGame.h"
#include "engine/game/CallbackContext.h"
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

namespace tetrad {

Editor::Editor()
    : m_pInputSystem(nullptr), m_pDrawSystem(nullptr), m_pSystemObserver(nullptr)
{}

void Editor::OnInitialized()
{
  // Setup mouse callbacks
  GLFWwindow *pWindow = GetCurrentScreen().GetWindow();
  glfwSetCursorPosCallback(pWindow, CallbackContext::Cursor_GUI);
  glfwSetMouseButtonCallback(pWindow, CallbackContext::MouseButton_Viewport);

  // Setup window resize callback
  glfwSetWindowSizeCallback(pWindow, CallbackContext::Resize_Default);

  Entity entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 0), glm::vec3(3.f, 2.3f, 1));
  entity.Add<MovableComponent>();
  entity.Add<AttachComponent>();
  DrawComponent *pDraw = entity.Add<DrawComponent>();
  pDraw->SetGeometry(ShapeType::PLANE);
  pDraw->SetTexture(BACKGROUND_PATH, TextureType::RGB);
  entity.Add<MaterialComponent>();

  // Create side-panel 1
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, .25), glm::vec3(.25, .95, 1));
  auto pUI = entity.Add<UIComponent>();
  pUI->SetCurrentTexture(TEXTURE_PATH + "Black.tga", TextureType::RGBA);
  pUI->Init(GetCurrentScreen(), 0);
  entity.Add<MaterialComponent>()->SetAddColor(glm::vec4(0.2, 0.2, 0.2, 0));

  // Create side-panel 2
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(.75, 0, .25), glm::vec3(.25, .95, 1));
  pUI = entity.Add<UIComponent>();
  pUI->SetCurrentTexture(TEXTURE_PATH + "Black.tga", TextureType::RGBA);
  pUI->Init(GetCurrentScreen(), 0);
  entity.Add<MaterialComponent>()->SetAddColor(glm::vec4(0.1, 0.2, 0.2, 0));

  // Create bottom-panel
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(.25, 0, .25), glm::vec3(.5, .25, 1));
  pUI = entity.Add<UIComponent>();
  pUI->SetCurrentTexture(TEXTURE_PATH + "Black.tga", TextureType::RGBA);
  pUI->Init(GetCurrentScreen(), 0);
  MaterialComponent *pMat = entity.Add<MaterialComponent>();
  pMat->SetAddColor(glm::vec4(0.2, 0.2, 0.2, 0));
  pMat->SetTopMultiplier(glm::vec4(0.9, 0.9, 0.9, 1));

  // Create top-panel
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, .95, 0), glm::vec3(1, .05, .5));
  pUI = entity.Add<UIComponent>();
  pUI->SetCurrentTexture(TEXTURE_PATH + "Black.tga", TextureType::RGBA);
  pUI->Init(GetCurrentScreen(), UI_PRIORITY_TOP);
  entity.Add<MaterialComponent>()->SetAddColor(glm::vec4(0.5, 0.5, 0.5, 0));

  // Create Camera 1
  m_CameraEntity = EntityManager::CreateEntity();
  m_CameraEntity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 5));
  m_CameraEntity.Add<MovableComponent>();
  m_CameraEntity.Add<PhysicsComponent>()->SetGravity(false);
  CameraComponent *pCamera = m_CameraEntity.Add<CameraComponent>();
  ObserverComponent *pObserver = m_CameraEntity.Add<ObserverComponent>();
  pObserver->Subscribe(*m_pInputSystem);
  pObserver->AddEvent(EGameEvent(EGE_PLAYER1_LEFT),
                      new Action_Move(m_CameraEntity, Action_Move::EMD_LEFT));
  pObserver->AddEvent(EGameEvent(EGE_PLAYER1_RIGHT),
                      new Action_Move(m_CameraEntity, Action_Move::EMD_RIGHT));
  pObserver->AddEvent(EGameEvent(EGE_PLAYER1_FORWARDS),
                      new Action_Move(m_CameraEntity, Action_Move::EMD_FORWARDS));
  pObserver->AddEvent(EGameEvent(EGE_PLAYER1_BACKWARDS),
                      new Action_Move(m_CameraEntity, Action_Move::EMD_BACKWARDS));

  // Create viewport 1
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(.25, .25, .25),
                                         glm::vec3(.249, .249, 1));
  UIViewport *pViewport = entity.Add<UIViewport>();
  pViewport->SetCamera(pCamera);
  pViewport->Init(GetCurrentScreen());

  // Create camera 2
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 5));
  pCamera = entity.Add<CameraComponent>();

  // Create viewport 2
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(.5, .25, 1), glm::vec3(.249, .249, 1));
  pViewport = entity.Add<UIViewport>();
  pViewport->SetCamera(pCamera);
  pViewport->Init(GetCurrentScreen());

  // Create camera 3
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 4));
  pCamera = entity.Add<CameraComponent>();

  // Create viewport 3
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(.25, .5, 1), glm::vec3(.249, .249, 1));
  pViewport = entity.Add<UIViewport>();
  pViewport->SetCamera(pCamera);
  pViewport->Init(GetCurrentScreen());

  // Create camera 4
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(0, 0, 3));
  pCamera = entity.Add<CameraComponent>();

  // Create viewport 4
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(.5, .5, 1), glm::vec3(.249, .249, 1));
  pViewport = entity.Add<UIViewport>();
  pViewport->SetCamera(pCamera);
  pViewport->Init(GetCurrentScreen());

  for (int i = 0; i < 10; ++i)
  {
    // Create test window
    entity = EntityManager::CreateEntity();
    entity.Add<TransformComponent>()->Init(glm::vec3(.5, .5, 0),
                                           glm::vec3(1.f / 10, 1.f / 11, 1));
    entity.Add<MovableComponent>();
    auto pUI = entity.Add<UIComponent>();
    pUI->SetCurrentTexture(TEXTURE_PATH + "Black.tga", TextureType::RGBA);
    pUI->Init(GetCurrentScreen());
    entity.Add<MaterialComponent>()->SetAddColor(glm::vec4(0.4, 0.4, 0.4, 0));

    // Create test button
    entity = EntityManager::CreateEntity();
    entity.Add<TransformComponent>()->Init(glm::vec3(.75, .75, 1), glm::vec3(.5, .5, 1));
    entity.Add<MovableComponent>();
    entity.Add<AttachComponent>()->Attach(pUI->GetEntity());
    auto textComp = entity.Add<TextComponent>();
    textComp->SetFont(ResourceManager::LoadFont(STANDARD_FONT_PATH));
    textComp->SetText("File");
    textComp->SetTextScale(.8f);
    auto pButton = entity.Add<UIButton>();
    pButton->SetTextures(TEXTURE_PATH + "UI/BTN_Exit.tga", PAUSE_BACKGROUND_PATH,
                         PAUSE_BACKGROUND_PATH);
    pButton->Init(GetCurrentScreen());
  }

  // Create test text
  entity = EntityManager::CreateEntity();
  entity.Add<TransformComponent>()->Init(glm::vec3(.1, 1, 1), glm::vec3(.75, .75, 1));
  auto textComp = entity.Add<TextComponent>();
  textComp->SetFont(ResourceManager::LoadFont(STANDARD_FONT_PATH));
  textComp->SetText("\nFile\nCreated by Alex Khouderchah\n(C) 2017");
}

void Editor::AddSystems()
{
  m_pInputSystem = new EventSystem;
  m_pInputSystem->MakeInputSystem();
  AppendSystem(m_pInputSystem);

  AppendSystem(new PhysicsSystem);

  m_pDrawSystem = new DrawSystem;
  AppendSystem(m_pDrawSystem);

  // Create the system observer
  m_pSystemObserver = EntityManager::CreateEntity().Add<ObserverComponent>();
  m_pSystemObserver->Subscribe(*m_pInputSystem);
  m_pSystemObserver->AddEvent(EGE_PAUSE, new Action_ExitGame(&GetCurrentScreen()));
}

}  // namespace tetrad
