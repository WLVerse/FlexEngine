#include "prefabeditor.h"

namespace ChronoDrift {
  void PrefabEditor::ShowPrefabEditorWindow() {
    // Create a window
    ImGui::Begin("Hello, ImGui!");  // Window title
    ImGui::Text("This is a basic ImGui window.");  // Display text
    if (ImGui::Button("Close")) {  // Create a button
      //glfwSetWindowShouldClose(window, true);
    }

    // Save/Load Prefabs
    if (ImGui::Button("Save Prefab")) {
      FlexECS::EntityID temp_entity = FlexECS::ecs_manager.CreateEntity("Temp");
      FlexECS::ecs_manager.SaveEntityAsPrefab(temp_entity, "test");
    }
    if (ImGui::Button("Load Prefab")) {
      // LoadPrefabAsEntity (Do i need to create a function for this?)
    }
    ImGui::End();

    }
}