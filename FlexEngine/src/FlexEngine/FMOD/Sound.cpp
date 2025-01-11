#pragma once

#include "FMODWrapper.h"
#include "Sound.h"

namespace FlexEngine
{
  namespace Asset
  {
    Sound::Sound(std::string const& key) : sound(nullptr)
    {
      std::string path = "assets" + key;
      FMOD_ASSERT(FMODWrapper::fmod_system->createSound(path.c_str(), FMOD_DEFAULT, 0, &sound));
    }

    // Cannot free sound as FMOD frees it internally
    Sound::~Sound() noexcept
    {
    }

    void Sound::Unload()
    {
      if (sound)
      {
        FMOD_ASSERT(sound->release());
        sound = nullptr;
      }
    }
  }
}