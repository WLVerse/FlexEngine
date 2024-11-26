 /*!************************************************************************
 // WLVERSE [https://wlverse.web.app]
 // FMODWrapper.h
 //
 // Audio Manager extends a function to setup FMOD at the start and end of the game. This is meant
 // to be used at the core level that manages its own construction and destruction.
 // 
 // It doesn't handle any of the audio playing, and meant to be run in a system in the game layer.
 // 
 // Note that you can't run two sounds under the same name unless the previous sound is a one shot and finished playing or was stopped.
 // 
 // Now here's the sauce...
 // For a quick start: https://www.fmod.com/docs/2.02/api/white-papers-getting-started.html#fmod-core-api-initialization-do-not-use-this-if-using-fmod-studio-api-initialization
 // Core API reference: https://www.fmod.com/docs/2.02/api/core-api-system.html#system_playsound
 //
 // AUTHORS
 // [100%] Yew Chong (yewchong.k@digipen.edu)
 //   - Main Author
 //
 // Copyright (c) 2024 DigiPen, All rights reserved.
 **************************************************************************/

#pragma once

#include "flx_api.h"
#include "pch.h"
#include "FMOD/studio/fmod_studio.hpp" // FMOD studio
#include "FMOD/core/fmod_errors.h"     // FMOD error handling for macro
#include <cassert>
#include "FMOD/Sound.h"                // Definition of the asset
#include "GLFW/glfw3.h"                // For focus/unfocus callback

// Most of the FMOD functions return an FMOD_RESULT, we want to watch for this with FMOD assert macro to crash fast for debugging
// This function forces the compiler to not optimize out our error string...
inline const char* GetFMODErrorString(FMOD_RESULT result) {
  return FMOD_ErrorString(result);
}

// Macro to assert any FMOD function that returns a FMOD::Result
#define FMOD_ASSERT(FUNCTION) \
    FMODWrapper::result = FUNCTION;  \
    if (FMODWrapper::result != FMOD_OK) { \
        const char* errorString = GetFMODErrorString(FMODWrapper::result); \
        Log::Error("FMOD Error: " + std::string(errorString)); \
        assert(false); \
    }

namespace FlexEngine
{
class __FLX_API FMODWrapper
{
public:

  // This class extends usage for middle level to call from.
  class Core
  {
    static std::map<std::string, FMOD::Channel*> channels; // Stores all instances of channels in existence

  public:
    /*!
      \brief Plays the sound. Usage: FMODWrapper::Core::PlaySound("mario", FLX_ASSET_GET(Asset::Sound, AssetKey("/audio/test.mp3")));
      \param identifier The identifier of the sound for controlling
      \param sound The sound to play
    */
    static void PlaySound(std::string const&, Asset::Sound const&);

    /*!
    \brief Plays a sound which loops automatically. Usage: FMODWrapper::Core::PlayLoopingSound("mario", FLX_ASSET_GET(Asset::Sound, AssetKey("/audio/test.mp3")));
    \param identifier The identifier of the sound for controlling
    \param sound The sound to play
    */
    static void PlayLoopingSound(std::string const& identifier, Asset::Sound const& asset);

    /*
      \brief Stops playing a sound. Usage: FMODWrapper::Core::StopSound("mario2");
      \param identifier The identifier of the sound to stop
    */
    static void StopSound(std::string const&);

    // Function to support GLFW callback
    static void WindowFocusCallback(GLFWwindow* window, int focused);

    static void ForceStop();

  private:
    // The below 2 functions are used for manually silencing FMOD. This is meant for things like alt tabbing.
    static void StopAll();
    static void ResumeAll();
  };

  // Static fmod variables
  static FMOD::Studio::System* fmod_studio_system;
  static FMOD::System* fmod_system;
  static FMOD_RESULT result;

  inline static bool is_studio_init() { return fmod_studio_system != NULL; }
  inline static bool is_core_init() { return fmod_system != NULL; }

  // Functions for setup of core FMOD systems
  static void Load();
  static void Unload();
  static void Update();
};
}// namespace FlexEngine