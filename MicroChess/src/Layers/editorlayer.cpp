#include "editorlayer.h"
#include "Components/physics.h"
#include "Editor/editor.h"

#include "FlexEngine/Core/imguiwrapper.h"
#include "FlexEngine/Renderer/OpenGL/opengldebugrenderer.h"


namespace ChronoShift 
{
  void EditorLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();
    Editor::GetInstance()->Init();
  }

  void EditorLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();

  }

  void EditorLayer::Update()
  {
    Editor::GetInstance()->Update();

    ImGui::Begin("Tools");
    ImGui::Checkbox("Show Colliders", &show_colliders);
    ImGui::End();

    if (show_colliders)
    {
      for (auto& entity : FlexECS::Scene::GetActiveScene()->View<BoundingBox2D>())
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