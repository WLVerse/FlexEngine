#include "editorlayer.h"
#include "Components/physics.h"
#include "Components/battlecomponents.h"
#include "Editor/editor.h"

#include "FlexEngine/Core/imguiwrapper.h"
#include "FlexEngine/Renderer/OpenGL/opengldebugrenderer.h"


namespace ChronoDrift 
{
  void EditorLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();
    Editor::GetInstance().Init();

    RegisterRenderingComponents();
    RegisterBattleComponents();

    FreeQueue::Push([]() {
      Editor::GetInstance().Shutdown();
    }, "Editor Shutdown");
  }

  void EditorLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
  }

  void EditorLayer::Update()
  {
    Editor::GetInstance().Update();

    ImGui::Begin("Tools");

    ImGui::Checkbox("Show Colliders", &show_colliders);

    if (ImGui::Button("Spawn 2500 Objects"))
    {
      auto scene = FlexECS::Manager::GetInstance().GetActiveScene();
      FlexECS::Entity thing = FlexECS::Manager::GetInstance().CreateEntity("White Queen");
      thing.AddComponent<IsActive>({ true });
      thing.AddComponent<Position>({ {0,0} });
      thing.AddComponent<Rotation>({ });
      thing.AddComponent<Scale>({ { 15,15 } });
      thing.AddComponent<ZIndex>({ 10 });
      thing.AddComponent<Transform>({ });
      thing.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(R"(\images\chess_queen.png)"),
        Vector3::One,
        Vector3::Zero,
        Vector3::One,
        Renderer2DProps::Alignment_Center
       });
      thing.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

      for (size_t x = 0; x < 50; x++)
      {
        for (size_t y = 0; y < 50; y++)
        {
          FlexECS::Entity cloned_thing = FlexECS::Manager::GetInstance().CloneEntity(thing);
          auto& position = cloned_thing.GetComponent<Position>()->position;
          position.x = static_cast<float>(15 * (x + 1));
          position.y = static_cast<float>(15 * (y + 1));
        }
      }
    }

    ImGui::End();

    if (show_colliders)
    {
      for (auto& entity : FlexECS::Manager::GetInstance().GetActiveScene()->CachedQuery<BoundingBox2D>())
      {
        const Vector3& max = entity.GetComponent<BoundingBox2D>()->max;
        const Vector3& min = entity.GetComponent<BoundingBox2D>()->min;
        //construct lines for AABB
        Vector3 topleft = min;
        Vector3 topright = { max.x, min.y };
        Vector3 botleft = { min.x, max.y };
        Vector3 botright = max;
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(topleft, topright);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(topright, botright);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(botright, botleft);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(botleft, topleft);
      }
    }
  }

}