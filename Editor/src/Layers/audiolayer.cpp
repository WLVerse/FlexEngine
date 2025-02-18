// WLVERSE [https://wlverse.web.app]
// audiolayer.cpp
// 
// Audio implementation in the layer
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "Layers.h"

namespace Editor
{
  void AudioLayer::OnAttach()
  {

  }

  void AudioLayer::OnDetach()
  {

  }

  void AudioLayer::Update()
  {
    Profiler::StartCounter("Audio Loop");

    // Audio
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Audio>())
    {
      if (!element.GetComponent<Transform>()->is_active) continue; // skip non active entities

      FlexEngine::Audio* audio = element.GetComponent<Audio>();
      if (audio->should_play)
      {
        if (audio->audio_file == FLX_STRING_NULL) // or you can use if (!audio->audio_file)
        {
          Log::Warning("Audio not attached to entity: " + FLX_STRING_GET(*element.GetComponent<EntityName>()));
          audio->should_play = false;
          continue;
        }

        if (audio->is_looping)
        {
          FMODWrapper::Core::PlayLoopingSound(FLX_STRING_GET(*element.GetComponent<EntityName>()),
                                              FLX_ASSET_GET(Asset::Sound, AssetKey{ FLX_STRING_GET(element.GetComponent<Audio>()->audio_file) }));
        }
        else
        {
          FMODWrapper::Core::PlaySound(FLX_STRING_GET(*element.GetComponent<EntityName>()),
                                       FLX_ASSET_GET(Asset::Sound, FLX_STRING_GET(element.GetComponent<Audio>()->audio_file)));
        }

        element.GetComponent<Audio>()->should_play = false;
      }

      if (audio->change_mode)
      {
        audio->change_mode = false;
        audio->is_looping = !audio->is_looping;
        FMODWrapper::Core::ChangeLoopProperty(FLX_STRING_GET(*element.GetComponent<EntityName>()), audio->is_looping);
      }
    }
  
    Profiler::EndCounter("Audio Loop");
  }
}
