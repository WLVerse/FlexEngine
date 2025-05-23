/** WLVerse
 * \file FMODWrapper.cpp
 *
 * \brief
 *   Audio wrapper to be used to play all audio in the game.
 *   This system uses FMOD studio as a basis for playing music
 *
 * \authors
 *   Yew Chong (yewchong.k\@digipen.edu)
 *
 * \link <https://portylstudios.notion.site/WLVerse-GAM150-Plan-d3460d6a24db417ca86f9ed54deaf365?pvs=4>
 *   WLVerse GAM150 Plan
 * \endlink
 *
 * \par All content (c) 2024 DigiPen Institute of Technology Singapore. All rights reserved.
 */

#include "FMODWrapper.h"
#include <string>

#include "flexprefs.h" // For saving volume settings

namespace FlexEngine
{
// Static initialization for wrapper
FMOD::System* FMODWrapper::fmod_system = NULL;
FMOD::Studio::System* FMODWrapper::fmod_studio_system = NULL;
FMOD_RESULT FMODWrapper::result;
FMOD::ChannelGroup* FMODWrapper::bgm_group = nullptr;
FMOD::ChannelGroup* FMODWrapper::sfx_group = nullptr;
bool FMODWrapper::is_paused = false;

// Static initialization for core
std::map<std::string, FMOD::Channel*> FMODWrapper::Core::channels;

// Callback function which calls stop sound when the sound is done playing
FMOD_RESULT F_CALLBACK channelCallback(FMOD_CHANNELCONTROL* channelControl, FMOD_CHANNELCONTROL_TYPE controlType [[maybe_unused]], 
                                       FMOD_CHANNELCONTROL_CALLBACK_TYPE callbackType, 
                                       void* commandData1 [[maybe_unused]], void* commandData2 [[maybe_unused]] )
{
  if (callbackType == FMOD_CHANNELCONTROL_CALLBACK_END)
  {
    // Get ptr to the channel and read data
    FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(channelControl);
    std::string* id = nullptr;
    channel->getUserData(reinterpret_cast<void**>(&id));

    if (id)
    {
      // Remove the channel from the map
      FMODWrapper::Core::StopSound(*id);

      // Free userdata memory alloc
      delete id;
      channel->setUserData(nullptr);
    }
  }
  return FMOD_OK;
}

/*!
  \brief Constructor for the FMODWrapper. Handles the creation of the FMOD system.
*/
void FMODWrapper::Load()
{
  FMOD_ASSERT(FMOD::Studio::System::create(&fmod_studio_system));
  FMOD_ASSERT(fmod_studio_system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));
  FMOD_ASSERT(fmod_studio_system->getCoreSystem(&fmod_system));

  fmod_system->createChannelGroup("BGM", &FMODWrapper::bgm_group);
  fmod_system->createChannelGroup("SFX", &FMODWrapper::sfx_group);

  // Set the volume of the channel groups
  FlexPrefs::Load(); // Load the settings in case they aren't open
  FMODWrapper::bgm_group->setVolume(FlexPrefs::GetFloat("game.volume", 0.0f));
  FMODWrapper::sfx_group->setVolume(FlexPrefs::GetFloat("game.sfx.volume", 0.0f));
}

/*!
  \brief Destructor for the FMODWrapper. Handles close and release of the FMOD system.
*/
void FMODWrapper::Unload()
{
  // Save any settings in case
  FlexPrefs::Save();

  Core::ForceStop();
  FMOD_ASSERT(fmod_studio_system->release()); // Unloads core as well...
}

/*!
  \brief Updates the FMOD instance
*/
void FMODWrapper::Update()
{
  FMOD_ASSERT(fmod_studio_system->update()); // Invokes fmod core's update as well...
}

void FMODWrapper::Core::PlaySound(std::string const& identifier, Asset::Sound const& asset, CHANNELGROUP cg)
{
  if (FMODWrapper::is_paused) return; // Don't play if paused

  // Play the sound given a sound handle and a channel...
  if (channels.find(identifier) == channels.end()) // not already used identifier
  {
    FMOD::Channel* channel;
    FMOD_ASSERT(fmod_system->playSound(asset.sound, nullptr, false, &channel));

    if (channel)
    {
      if (cg == CHANNELGROUP::BGM)
      {
        channel->setChannelGroup(FMODWrapper::bgm_group);
      }
      else if (cg == CHANNELGROUP::SFX)
      {
        channel->setChannelGroup(FMODWrapper::sfx_group);
      }
    }

    // Set it to automatically remove from list when done
    std::string* id = new std::string(identifier);
    channel->setCallback(channelCallback); 
    channel->setUserData(reinterpret_cast<void*>(id));

    channels[identifier] = channel;
  }
  else
  {
    Log::Warning("Channel already exists for identifier: " + identifier + ", playing newly requested sound...");
    StopSound(identifier);
    PlaySound(identifier, asset);
  }
}

void FMODWrapper::Core::PlayLoopingSound(std::string const& identifier, Asset::Sound const& asset, CHANNELGROUP cg)
{
  // Play the sound given a sound handle and a channel...
  if (channels.find(identifier) == channels.end()) // not already used identifier
  {
    FMOD::Channel* channel;
    FMOD_ASSERT(fmod_system->playSound(asset.sound, nullptr, false, &channel));
    channel->setMode(FMOD_LOOP_NORMAL); 

    if (channel)
    {
      if (cg == CHANNELGROUP::BGM)
      {
        channel->setChannelGroup(FMODWrapper::bgm_group);
      }
      else if (cg == CHANNELGROUP::SFX)
      {
        channel->setChannelGroup(FMODWrapper::sfx_group);
      }
    }

    // Set it to automatically remove from list when done
    std::string* id = new std::string(identifier);
    channel->setCallback(channelCallback);
    channel->setUserData(reinterpret_cast<void*>(id));

    channels[identifier] = channel;
  }
  else
  {
    Log::Warning("Channel already exists for identifier: " + identifier + ", playing newly requested sound...");
    StopSound(identifier);
    PlaySound(identifier, asset);
  }
}

void FMODWrapper::Core::StopSound(std::string const& identifier)
{
  if (channels.find(identifier) != channels.end())
  {
    channels[identifier]->stop();
    channels.erase(identifier);
  }
  else Log::Warning("Tried to stop channel that does not exist for identifier: " + identifier);
}

void FMODWrapper::Core::ForceStop()
{
  // A bizarre choice... but I have no choice because this is not callback safe. I've tried not making a copy but it doesn't work.
  std::vector<FMOD::Channel*> channelsToStop;
  for (auto& channel : channels)
  {
    channelsToStop.push_back(channel.second);
  }

  for (auto* channel : channelsToStop)
  {
    if (channel)
    {
      channel->stop(); 
    }
  }

  channels.clear();
}

void FMODWrapper::Core::ForceFadeOut(float fadeDuration)
{
  for (auto& [key, channel] : channels)
  {
    if (!channel) continue; // Skip invalid 

    // Enable volume ramp to start fade out
    channel->setVolumeRamp(true);

    // Set fade-out duration
    FMOD_ASSERT(channel->setFadePointRamp(static_cast<unsigned long long>(fadeDuration * 1000.f), 0.0f)); // End volume (0%) after fadeDuration

    // Schedule a stop at the end of the fade using DSP clock delay system
    unsigned long long dspClock;
    channel->getSystemObject(&fmod_system);
    channel->getDSPClock(nullptr, &dspClock);

    // Create variables for getting software format, but we just want the sample rate.
    int sampleRate = 0;
    FMOD_SPEAKERMODE speakerMode;
    int numRawSpeakers;
    fmod_system->getSoftwareFormat(&sampleRate, &speakerMode, &numRawSpeakers);

    // Set the delay to stop the sound after the fade duration
    channel->setDelay(0, dspClock + static_cast<unsigned long long>(fadeDuration * sampleRate), false);
  }
}

void FMODWrapper::Core::StopAll()
{
  for (auto& channel : channels)
  {
    channel.second->setPaused(true);
  }
}

void FMODWrapper::Core::ResumeAll()
{
  for (auto& channel : channels)
  {
    channel.second->setPaused(false);
  }
}

void FMODWrapper::Core::WindowFocusCallback([[maybe_unused]] GLFWwindow* window, int focused) // Forced to use this signature, but don't need the window pointer itself.
{
  if (focused == GLFW_FALSE)
  {
    is_paused = true;
    StopAll();
  }
  else
  {
    is_paused = false;
    ResumeAll();
  }
}

void FMODWrapper::Core::ChangeLoopProperty(std::string const& identifier, bool is_looping)
{
  // Proper way to stop but this kills the sound
  channels[identifier]->setMode(is_looping ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
  
  // Slightly hacky but should always work
  //channels[identifier]->setLoopCount(0);
}

void FMODWrapper::Core::AdjustGroupVolume(CHANNELGROUP channelGroup, float volPercent)
{
  if (channelGroup == CHANNELGROUP::BGM)
  {
    FMODWrapper::bgm_group->setVolume(volPercent);
    FlexPrefs::SetFloat("game.volume", volPercent);
  }
  else if (channelGroup == CHANNELGROUP::SFX)
  {
    FMODWrapper::sfx_group->setVolume(volPercent);
    FlexPrefs::SetFloat("game.sfx.volume", volPercent);
  }
}

float FMODWrapper::Core::GetGroupVolume(CHANNELGROUP channelGroup)
{
  float volume = 0.f;
  if (channelGroup == CHANNELGROUP::BGM) FMODWrapper::bgm_group->getVolume(&volume);
  else if (channelGroup == CHANNELGROUP::SFX) FMODWrapper::sfx_group->getVolume(&volume);
  return volume;
}

}