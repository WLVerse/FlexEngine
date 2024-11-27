/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      edtiorlayer.cpp
\author    [90%] Rocky Sutarius
           - Main author, basically everything here.
\co-author [10%] Yew Chong
           - Migrated some code from overworld.cpp like Spawn 2500 Objects
\par       rocky.sutarius\@digipen.edu
\par       yewchong.k\@digipen.edu
\date      24 November 2024
\brief     Editor layer behavior, editor is initialized in this layer.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/


#include "editorlayer.h"
#include "Components/physics.h"
#include "Components/battlecomponents.h"
#include "Editor/editor.h"

#include "FlexEngine/Core/imguiwrapper.h"
#include "FlexEngine/Renderer/OpenGL/opengldebugrenderer.h"
#include "windowsizes.h"

namespace ChronoDrift 
{
  void EditorLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();
    Editor::GetInstance().Init();
    Editor::GetInstance().SetCamManager(*m_CamM_Instance);
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

    SETTOOLSWINDOW
    ImGui::Begin("Tools");

    ImGui::Checkbox("Show Colliders", &show_colliders);

    if (ImGui::Button("Spawn 2500 Objects"))
    {
      auto scene = FlexECS::Scene::GetActiveScene();
      FlexECS::Entity thing = FlexECS::Scene::CreateEntity("White Queen");
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
          FlexECS::Entity cloned_thing = scene->CloneEntity(thing);
          auto& position = cloned_thing.GetComponent<Position>()->position;
          position.x = static_cast<float>(15 * (x + 1));
          position.y = static_cast<float>(15 * (y + 1));
        }
      }
    }

    ImGui::End();

    if (show_colliders)
    {
      for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<BoundingBox2D>())
      {
        const Vector3& max = entity.GetComponent<BoundingBox2D>()->max;
        const Vector3& min = entity.GetComponent<BoundingBox2D>()->min;
        //construct lines for AABB
        Vector3 topleft = min;
        Vector3 topright = { max.x, min.y };
        Vector3 botleft = { min.x, max.y };
        Vector3 botright = max;
        
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(Editor::GetInstance().GetCamManager(), topleft, topright);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(Editor::GetInstance().GetCamManager(), topright, botright);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(Editor::GetInstance().GetCamManager(), botright, botleft);
        FlexEngine::OpenGLDebugRenderer::DrawLine2D(Editor::GetInstance().GetCamManager(), botleft, topleft);
      }
    }
  }


}