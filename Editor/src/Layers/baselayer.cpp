#include "States.h"
#include "Layers.h"

namespace Editor
{

  void BaseLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();

    // load assets only after the window has been created
    AssetManager::Load();
    FreeQueue::Push(std::bind(&AssetManager::Unload), "Editor AssetManager");

    FlexEngine::Window* window = Application::GetCurrentWindow();
    window->SetIcon(FLX_ASSET_GET(Asset::Texture, R"(\images\flexengine\flexengine-256.png)"));

    // Renderer Setup

    OpenGLRenderer::EnableBlending();
  }

  void BaseLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();

    OpenGLRenderer::DisableBlending();

    FreeQueue::RemoveAndExecute("Editor AssetManager");
  }

  void BaseLayer::Update()
  {
    // clear frame buffer and reset draw call count
    OpenGLRenderer::ClearFrameBuffer();

    // setup dockspace
    ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoDockingInCentralNode;
    #pragma warning(suppress: 4189) // local variable is initialized but not referenced
    ImGuiID dockspace_main_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), dockspace_flags);
    
  }

}
