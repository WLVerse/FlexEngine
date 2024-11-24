/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      base.cpp
\author    [100%] Rocky Sutarius
\par       rocky.sutarius\@digipen.edu
\date      03 October 2024
\brief     This file 

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "base.h"

#include "States.h"
#include "Layers.h"

#include "Components/rendering.h"
#include "windowsizes.h"

namespace ChronoDrift
{

  void BaseLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();

    // load assets only after the window has been created
    AssetManager::Load();
    FreeQueue::Push(std::bind(&AssetManager::Unload), "Chrono Drift AssetManager");

    FlexEngine::Window* window = Application::GetCurrentWindow();
    window->SetTargetFPS(60);
    window->SetVSync(false);
    window->SetIcon(FLX_ASSET_GET(Asset::Texture, R"(\images\flexengine\flexengine-256.png)"));

    window->PushLayer(std::make_shared<ChronoDrift::BattleLayer>());
    window->PushLayer(std::make_shared<ChronoDrift::OverworldLayer>());
    //window->PushLayer(std::make_shared<ChronoDrift::EditorLayer>());
   
    // Renderer Setup

    OpenGLRenderer::EnableBlending();
    Vector2 windowsize{ static_cast<float>(window->GetWidth()), static_cast<float>(window->GetHeight()) };
    OpenGLSpriteRenderer::Init(windowsize);
    OpenGLTextRenderer::Init();
  }

  void BaseLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();

    OpenGLRenderer::DisableBlending();

    FreeQueue::RemoveAndExecute("Chrono Drift AssetManager");
  }

  void BaseLayer::Update()
  {
    OpenGLRenderer::ClearFrameBuffer();
  }

}
