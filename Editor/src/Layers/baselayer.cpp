#include "States.h"
#include "Layers.h"

namespace Editor
{

  void BaseLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();

    // load assets only after the window has been created
    AssetManager::Load();
    FreeQueue::Push(std::bind(&AssetManager::Unload), "Application AssetManager");

    // Renderer Setup

    OpenGLRenderer::EnableBlending();

    // Send message to Splash Layer
    Application::MessagingSystem::Send("Is base layer loaded?", true);
  }

  void BaseLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();

    OpenGLRenderer::DisableBlending();

    FreeQueue::RemoveAndExecute("Application AssetManager");
  }

  void BaseLayer::Update()
  {

  }

}
