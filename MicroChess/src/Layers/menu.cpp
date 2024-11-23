#if 0
#include "menu.h"

#include "Components/rendering.h"

#include "Renderer/sprite2d.h"

namespace MicroChess
{

  void MenuLayer::SetupMenu()
  {
    //auto scene = FlexECS::Scene::CreateScene();
    //FlexECS::Scene::SetActiveScene(scene);
    //
    //// cleanup
    //
    //buttons.clear();
    //buttons.reserve(4);
    //
    //// create entities
    //
    //buttons.push_back(FlexECS::Scene::CreateEntity("Play"));
    //buttons.push_back(FlexECS::Scene::CreateEntity("Settings"));
    //buttons.push_back(FlexECS::Scene::CreateEntity("Quit"));
    //
    //// add components
    //
    //auto window = Application::GetCurrentWindow();
    //Vector2 screen_center = { window->GetWidth() * 0.5f, window->GetHeight() * 0.5f };
    //for (std::size_t i = 0; i < buttons.size(); i++)
    //{
    //  auto& entity = buttons[i];
    //  entity.AddComponent<IsActive>({ true });
    //  entity.AddComponent<Sprite>({
    //    scene->Internal_StringStorage_New(R"(\images\flexengine\flexengine-256.png)"),
    //    { 1.0f, 1.0f, 1.0f },
    //    { 0.0f, 0.0f, 0.0f },
    //    { 1.0f, 1.0f, 1.0f },
    //    Renderer2DProps::Alignment_Center
    //  });
    //  entity.AddComponent<Position>({ screen_center, { 0.0f, i * (50.0f + 25.0f) } });
    //  entity.AddComponent<Scale>({ { 100, 50 }, { 1, 1 } });
    //  entity.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
    //}

  }

  void MenuLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();

    // ECS Setup
    SetupMenu();
  }

  void MenuLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
  }

  void MenuLayer::Update()
  {
    // show cursor position
    ImGui::Begin("Cursor Position");
    ImGui::Text("Mouse Position: (%.1f, %.1f)", Input::GetCursorPosition().x, Input::GetCursorPosition().y);
    ImGui::End();

    RenderSprite2D();
  }

}
#endif