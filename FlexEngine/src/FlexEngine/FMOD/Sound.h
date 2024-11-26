/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// sound.h
//
// Asset declaration for sound
//
// AUTHORS
// [100%] Yew Chong (yewchong.k@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#pragma once

// Asset template for assetmanager

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
      void Unload();
    };
  }
}