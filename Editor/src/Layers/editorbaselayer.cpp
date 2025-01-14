#include "Layers.h"
#include "editor.h"

#define TESTCOMPONENTVIEWER 1
#if TESTCOMPONENTVIEWER
#include "componentviewer.h"
#include "testcomponentviewer.h"
#endif

namespace Editor
{

  #if TESTCOMPONENTVIEWER

  FLX_REFL_REGISTER_START(Position)
    FLX_REFL_REGISTER_PROPERTY(position)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Scale)
    FLX_REFL_REGISTER_PROPERTY(scale)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Rotation)
    FLX_REFL_REGISTER_PROPERTY(rotation)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Transform)
    FLX_REFL_REGISTER_PROPERTY(is_dirty)
    FLX_REFL_REGISTER_PROPERTY(transform)
  FLX_REFL_REGISTER_END;

  COMPONENT_VIEWER_START(Position)
    COMPONENT_VIEWER_DRAG_VECTOR2(position)
  COMPONENT_VIEWER_END(Position)

  COMPONENT_VIEWER_START(Rotation)
    COMPONENT_VIEWER_DRAG_VECTOR3(rotation)
  COMPONENT_VIEWER_END(Rotation)

  COMPONENT_VIEWER_START(Scale)
    COMPONENT_VIEWER_DRAG_VECTOR2(scale)
  COMPONENT_VIEWER_END(Scale)

  COMPONENT_VIEWER_START(Transform)
    COMPONENT_VIEWER_BOOL(is_dirty)
    COMPONENT_VIEWER_MAT44(transform)
  COMPONENT_VIEWER_END(Transform)
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

    Editor::GetInstance().Init();

    REGISTER_COMPONENT_VIEWER(Position);
    REGISTER_COMPONENT_VIEWER(Rotation);
    REGISTER_COMPONENT_VIEWER(Scale);
    REGISTER_COMPONENT_VIEWER_FUNCTIONS(Transform, COMPONENT_ENABLE_ADD, COMPONENT_DISABLE_REMOVE);

    #if TESTCOMPONENTVIEWER
    auto scene = FlexECS::Scene::CreateScene();
    {
      FlexECS::Entity entity = scene->CreateEntity("Save Test 0");
      entity.AddComponent<Position>({});
    }
    {
      FlexECS::Entity entity = scene->CreateEntity("Save Test 1");
      entity.AddComponent<Scale>({});
    }
    {
      FlexECS::Entity entity = scene->CreateEntity("Save Test 2");
      entity.AddComponent<Transform>({});
      entity.AddComponent<Rotation>({});
    }
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
    //ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
    //#pragma warning(suppress: 4189) // local variable is initialized but not referenced
    dockspace_main_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);

    Editor::GetInstance().Update();
  }

}
