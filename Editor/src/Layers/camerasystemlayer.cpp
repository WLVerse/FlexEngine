// WLVERSE [https://wlverse.web.app]
// camerasystemlayer.cpp
// 
// This layer literally just tries to get the main camera and updates it
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#include "Layers.h"

namespace Editor 
{
    void CameraSystemLayer::OnAttach()
    {

    }

    void CameraSystemLayer::OnDetach()
    {

    }

    void CameraSystemLayer::Update()
    {
      if (!CameraManager::has_main_camera)
      {
        CameraManager::TryMainCamera();
      }

      for (auto& elem : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
      {
        elem.GetComponent<Camera>()->Update();
      }
    }
}
