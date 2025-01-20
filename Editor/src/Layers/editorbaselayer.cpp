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
#include "editor.h"
#include "FlexEngine/FlexECS/enginecomponents.h"
#include "editorcomponents.h"
#include "componentviewer.h"

namespace Editor
{

  void EditorBaseLayer::OnAttach()
  {

    // test scene
    #if 1
    {
      auto scene = FlexECS::Scene::CreateScene();
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 0");
        entity.AddComponent<Vector2>({ 2, 4 });
        entity.AddComponent<Transform>({});
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 1");
        entity.AddComponent<Vector2>({ 35, 42 });
        entity.AddComponent<Rotation>({});
      }
      {
        FlexECS::Entity entity = scene->CreateEntity("Save Test 2222222222222222222222222222222222");
        entity.AddComponent<Vector2>({ 1, 2 });
        entity.AddComponent<Vector3>({ 1, 2, 3 });
        entity.AddComponent<Position>({});
        entity.AddComponent<Rotation>({});
        entity.AddComponent<Scale>({});
        entity.AddComponent<Transform>({});
        entity.AddComponent<Audio>({ true, false, false, FLX_STRING_NEW(R"(/audio/attack.mp3)") });
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

    Editor::GetInstance().Init();
    RegisterComponents();
  }

  void EditorBaseLayer::OnDetach()
  {

  }

  void EditorBaseLayer::Update()
  {
    // Always remember to set the context before using ImGui
    FLX_IMGUI_ALIGNCONTEXT();

    // setup dockspace
    //ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    //#pragma warning(suppress: 4189) // local variable is initialized but not referenced
    dockspace_main_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
  
    // spritesheet test
    // draw a sprite with render system
    #if 1
    {
      static auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, R"(/images/Prop_Flaming_Barrel.flxspritesheet)");
      static float time = 0.0f;
      time += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
      int index = (int)(time * asset_spritesheet.columns) % asset_spritesheet.columns;

      Renderer2DProps props;
      props.asset = R"(/images/Prop_Flaming_Barrel.flxspritesheet)";
      props.texture_index = index;
      props.position = Vector2(0.0f, 0.0f);
      props.scale = Vector2(384.0f / asset_spritesheet.columns, 96.0f);
      props.window_size = Vector2(1600.0f, 900.0f);
      props.alignment = Renderer2DProps::Alignment_TopLeft;

      static Camera camera(0.0f, 1600.0f, 900.0f, 0.0f, -2.0f, 2.0f);

      OpenGLRenderer::DrawTexture2D(camera, props);

      OpenGLRenderer::DrawSimpleTexture2D(
        AssetManager::Get<Asset::Texture>(asset_spritesheet.texture),
        Vector2(384.0f, 0.0f),
        Vector2(384.0f, 96.0f),
        Vector2(1600.0f, 900.0f),
        Renderer2DProps::Alignment_TopLeft
      );


      static std::string extra = "";
      if (Input::GetKey(GLFW_KEY_J))
          extra += "j";
      Renderer2DText sample;
      static std::string fullText = "The whole human fraternity is becoming highly dependent on the computer technology; no one can imagine life without computer. As, it has spread its wings so deeply in every area and made people used of it. It is very beneficial for the students of any class. They can use it to prepare their projects, learn poems, read different stories, download notes for exam preparations, collect large information within seconds, learn about painting, drawing, etc. However it enhances the professional skills of the students and helps in getting job easily.";
      static std::string displayedText = ""; // Start with an empty string
      static float elapsedTime = 0.0f;       // To track time
      elapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 100;
      if (elapsedTime >= 1.0f && displayedText.size() < fullText.size()) {
          displayedText += fullText[displayedText.size()]; // Append the next character
          elapsedTime = 0.0f; // Reset the timer
      }
      sample.m_words = displayedText;
      //sample.m_words = "hello there my name is markiplier and welcome back to another game of amnesia the dark descent" + extra;
      sample.m_color = Vector3::Zero;
      sample.m_fonttype = R"(/fonts/Bangers/Bangers-Regular.ttf)";
      sample.m_transform = Matrix4x4(1.00, 0.00, 0.00, 0.00,
          0.00, 1.00, 0.00, 0.00,
          0.00, 0.00, 1.00, 0.00,
           822.00, 248.00, 0.00, 1.00);
      sample.m_window_size = Vector2(static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()), static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight()));
      //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Top };
      //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Middle };
      //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Bottom };
      //sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Top };
      sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle };
      //sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Bottom };
      //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Top };
      //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Middle };
      //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Bottom };
      sample.m_maxwidthtextbox = 850.0f;

      OpenGLRenderer::DrawTexture2D(camera, sample);
    }
    #endif

    Editor::GetInstance().Update();
  }

}
