// WLVERSE [https://wlverse.web.app]
// baselayer.cpp
//
// Base layer that runs before everything. 
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.

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