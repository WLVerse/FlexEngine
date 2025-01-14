// WLVERSE [https://wlverse.web.app]
// statisticspanellayer.cpp
// 
// Statistics panel for debugging.
// 
// This panel is designed to display every single thing that is happening in the engine
// - List of layers in application layerstack
// - Current state for application state manager
// - All windows and their properties
//   - List of layers in window layerstack
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

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

    // This panel is designed to display every single thing that is happening in the engine
    // - List of layers in application layerstack
    // - Current state for application state manager
    // - All windows and their properties
    //   - List of layers in window layerstack
    #pragma region Statistics Panel

    // Always remember to set the context before using ImGui
    //FLX_GLFW_ALIGNCONTEXT();
    FLX_IMGUI_ALIGNCONTEXT();

    ImGui::Begin("Statistics");
    ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnDoubleClick; // open by default

    #pragma region Application

    if (ImGui::CollapsingHeader("Application", tree_node_flags))
    {
      auto& layerstack = Application::GetLayerStack();
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

    #pragma endregion

    #pragma region Windows

    if (ImGui::CollapsingHeader("Windows", tree_node_flags))
    {
      for (auto& [window_name, window] : Application::GetWindowRegistry())
      {

        std::string window_name_str = window->GetTitle() + " (" + window_name + ")";
        if (ImGui::TreeNode(window_name_str.c_str()))
        {
          if (ImGui::TreeNode("Window Properties"))
          {
            int win_pos_x, win_pos_y;
            glfwGetWindowPos(window->GetGLFWWindow(), &win_pos_x, &win_pos_y);
            ImGui::Text("Window Position: %d, %d", win_pos_x, win_pos_y);
            ImGui::Text("Window Size: %d x %d", window->GetWidth(), window->GetHeight());
            ImGui::Text("FPS: %d", window->GetFramerateController().GetFPS());
            ImGui::Text("Delta Time: %.3f ms", window->GetFramerateController().GetDeltaTime());

            ImGui::TreePop();
          }

          if (ImGui::TreeNode("Layers"))
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

            ImGui::TreePop();
          }
            
          ImGui::TreePop();
        } // TreeNode Window

      }
    } // CollapsingHeader Windows

    #pragma endregion

    #pragma region Renderer

    if (ImGui::CollapsingHeader("Renderer", tree_node_flags))
    {
      ImGui::Text("Draw Calls: %d", OpenGLRenderer::GetDrawCallsLastFrame());
    }

    #pragma endregion

    ImGui::End();

    #pragma endregion

  }

}
