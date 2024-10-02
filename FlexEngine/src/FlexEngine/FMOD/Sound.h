/* WLVERSE[https://wlverse.web.app]
// sound.h
//
// Interface for Sound asset to talk to the AssetManager and provides a cleaner handle for higher level code to interact with FMOD.
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
*/
#pragma once

#include "flx_api.h"
#include "FMOD/studio/fmod_studio.hpp" // FMOD studio

namespace FlexEngine
{
  namespace Asset
  {
    // Interface for engine level to pass into wrapped FMOD
    class __FLX_API Sound
    {
    public:
      FMOD::Sound* sound;

      // Only allow construction of sound if we have a key.
      Sound(std::string const& key);
      ~Sound() noexcept;
    };
  }
}