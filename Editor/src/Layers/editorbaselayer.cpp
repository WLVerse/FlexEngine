// WLVERSE [https://wlverse.web.app]
// editorbaselayer.h
// 
// Base layer for the editor.
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

#define SPRITESHEET_TEST

namespace Editor
{

  #ifdef SPRITESHEET_TEST
  Asset::Texture spritesheet;
  #endif

  void EditorBaseLayer::OnAttach()
  {
    // test scene
    #if 0
    {
      auto scene = FlexECS::Scene::CreateScene();
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 0");
        entity.AddComponent<Vector2>({ 2, 4 });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 1");
        entity.AddComponent<Vector2>({ 35, 42 });
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 2");
        entity.AddComponent<Vector2>({ 1, 2 });
        entity.AddComponent<Vector3>({ 1, 2, 3 });
      }
      //scene->DumpArchetypeIndex();
    }
    #endif

    // preliminary test
    #if 0
    {
      for (auto& [type_list, archetype] : scene->archetype_index)
      {
        // test conversion
        FlexECS::_Archetype _archetype_index = FlexECS::Internal_ConvertToSerializedArchetype(archetype);

        // print the archetype
        std::stringstream ss;
        for (std::size_t i = 0; i < _archetype_index.type.size(); ++i)
        {
          ss << _archetype_index.type[i] << " ";
        }
        Log::Warning("Archetype: " + ss.str());

        // print what was converted
        for (std::size_t i = 0; i < _archetype_index.archetype_table.size(); ++i)
        {
          // print the type and data
          Log::Info("Type: " + _archetype_index.type[i]);

          // print per entity
          for (std::size_t j = 0; j < _archetype_index.archetype_table[i].size(); ++j)
          {
            Log::Debug("Entity: " + std::to_string(j));
            Log::Debug("Data: " + _archetype_index.archetype_table[i][j]);
          }
        }
      }
    }
    #endif

    // test serialization
    #if 0
    {
      Path save_to_path = Path::current("temp/serialization_test_scene.flxscene");
      File& file = File::Open(save_to_path);
      scene->Save(file);
    }
    #endif

    // test deserialization
    #if 0
    {
      Path load_from_path = Path::current("temp/serialization_test_scene.flxscene");
      File& file = File::Open(load_from_path);
      auto loaded_scene = FlexECS::Scene::Load(file);
      loaded_scene->DumpArchetypeIndex();
    }
    #endif

    #ifdef SPRITESHEET_TEST
    spritesheet.Load(Path::current("assets/images/Prop_Flaming_Barrel.png"));
    #endif
  }

  void EditorBaseLayer::OnDetach()
  {

  }

  void EditorBaseLayer::Update()
  {
    // Always remember to set the context before using ImGui
    FLX_IMGUI_ALIGNCONTEXT();

    // setup dockspace
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;
    //#pragma warning(suppress: 4189) // local variable is initialized but not referenced
    dockspace_main_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

    // spritesheet test
    // draw a sprite with render system
    #if 0
    {
      Renderer2DProps props;
      props.asset = R"(/images/Prop_Flaming_Barrel.png)";
      //props.texture_index = 0;
      //props.window_size = Vector2(1600.0f, 900.0f);
      props.alignment = Renderer2DProps::Alignment_TopLeft;

      static Camera camera(0.0f, 0.0f, 1600.0f, 900.0f, -1.0f, 1.0f);

      OpenGLRenderer::DrawTexture2D(camera, props);
    }
    #else
    {
      ImGui::Begin("Sprite Test");
      ImGui::Image(IMGUI_IMAGE(spritesheet));
      ImGui::End();

      OpenGLRenderer::DrawSimpleTexture2D(
        spritesheet,
        Vector2(0.0f, 0.0f),
        Vector2(384.0f, 96.0f),
        Vector2(1600.0f, 900.0f),
        Renderer2DProps::Alignment_TopLeft
      );
    }
    #endif
  }

}
