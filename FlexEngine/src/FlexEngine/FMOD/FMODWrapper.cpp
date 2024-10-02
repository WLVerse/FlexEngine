/** WLVerse
 * \file FMODWrapper.cpp
 *
 * \brief
 *   Audio wrapper to be used to play all audio in the game.
 *   This system uses FMOD studio as a basis for playing music
 *
 * \authors
 *   [100%] Yew Chong (yewchong.k\@digipen.edu)
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
  FMOD_ASSERT(fmod_studio_system->release()); // Unloads core as well...
}

/*!
  \brief Updates the FMOD instance
*/
void FMODWrapper::Update()
{
  FMOD_ASSERT(fmod_studio_system->update()); // Invokes fmod core's update as well...
}

/*!
  \brief Plays the sound.
  \param identifier The identifier of the sound for controlling
  \param sound The sound to play
*/
void FMODWrapper::Core::PlaySound(std::string const& identifier, Asset::Sound const& asset)
{
  // Play the sound given a sound handle and a channel...
  if (channels.find(identifier) == channels.end()) // not already used identifier
  {
    FMOD::Channel* channel;
    FMOD_ASSERT(fmod_system->playSound(asset.sound, nullptr, false, &channel));
    channels[identifier] = channel;
  }
  else Log::Warning("Channel already exists for identifier: " + identifier);
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