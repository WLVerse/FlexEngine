#include "States.h"
#include "Layers.h"

namespace Editor
{

  void StatisticsPanelLayer::OnAttach()
  {

  }

  void StatisticsPanelLayer::OnDetach()
  {

  }

  void StatisticsPanelLayer::Update()
  {

    #pragma region Statistics Panel

    #if 1
    {
      ImGui::Begin("Statistics");
      ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick; // open by default

      auto window = Application::GetCurrentWindow();

      if (ImGui::CollapsingHeader("Window", tree_node_flags))
      {
        int win_pos_x, win_pos_y;
        window->GetWindowPosition(&win_pos_x, &win_pos_y);
        ImGui::Text("Window Position: %d, %d", win_pos_x, win_pos_y);
        ImGui::Text("Window Size: %d x %d", window->GetWidth(), window->GetHeight());
        ImGui::Text("FPS: %d", window->GetFPS());
        ImGui::Text("Delta Time: %.3f ms", window->GetDeltaTime());
      }

      if (ImGui::CollapsingHeader("Layers", tree_node_flags))
      {
        auto& layerstack = window->GetLayerStack();

        ImGui::Text("Layer Count: %d", layerstack.GetLayerCount());
        ImGui::Text("Overlay Count: %d", layerstack.GetOverlayCount());

        ImGui::Separator();

        ImGui::Text("Layer Stack");
        size_t index = 0;
        for (size_t i = 0; i < layerstack.GetLayerCount(); ++i, ++index)
        {
          ImGui::Text("  [%d] %s", index, layerstack.GetLayerName(i).c_str());
        }
        ImGui::Separator();
        for (size_t i = 0; i < layerstack.GetOverlayCount(); ++i, ++index)
        {
          ImGui::Text("  [%d] %s", index, layerstack.GetOverlayName(i).c_str());
        }
      }

      if (ImGui::CollapsingHeader("Renderer", tree_node_flags))
      {
        ImGui::Text("Draw Calls: %d", OpenGLRenderer::GetDrawCallsLastFrame());
      }

      ImGui::End();
    }
    #endif

    #pragma endregion

  }

}
