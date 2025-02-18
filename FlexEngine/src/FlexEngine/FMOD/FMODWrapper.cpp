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

namespace FlexEngine
{
// Static initialization
FMOD::System* FMODWrapper::fmod_system = NULL;
FMOD::Studio::System* FMODWrapper::fmod_studio_system = NULL;
FMOD_RESULT FMODWrapper::result;
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
}

/*!
  \brief Destructor for the FMODWrapper. Handles close and release of the FMOD system.
*/
void FMODWrapper::Unload()
{
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

void FMODWrapper::Core::PlaySound(std::string const& identifier, Asset::Sound const& asset)
{
  // Play the sound given a sound handle and a channel...
  if (channels.find(identifier) == channels.end()) // not already used identifier
  {
    FMOD::Channel* channel;
    FMOD_ASSERT(fmod_system->playSound(asset.sound, nullptr, false, &channel));

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

void FMODWrapper::Core::PlayLoopingSound(std::string const& identifier, Asset::Sound const& asset)
{
  // Play the sound given a sound handle and a channel...
  if (channels.find(identifier) == channels.end()) // not already used identifier
  {
    FMOD::Channel* channel;
    FMOD_ASSERT(fmod_system->playSound(asset.sound, nullptr, false, &channel));
    channel->setMode(FMOD_LOOP_NORMAL); 

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
    StopAll();
  }
  else
  {
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

}// namespace FlexEngine

  //// Sample code to get if the channel is playing...
  //bool b;
  //channel->isPlaying(&b);

  //// Stop the channel
  //channel->stop();

  //// Channel represents an instance of sound being played...
  //channel->setVolume(0.5f);

  //// Channel groups are like a mixer, you can control the volume of a group of sounds
  //FMOD::ChannelGroup* musicGroup = nullptr;
  //fmod_system->createChannelGroup("Music", &musicGroup);

  //// Volume controls group wise
  //musicGroup->setVolume(0.7f);  // Set volume for the entire group
  //musicGroup->stop();