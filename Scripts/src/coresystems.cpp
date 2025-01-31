#include <FlexEngine.h>
#include "FlexEngine/Physics/physicssystem.h"

using namespace FlexEngine;

class CoreSystemsScript : public IScript
{
public:
  CoreSystemsScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "GameplayLoops"; }

  void Awake() override
  {

  }

  void Start() override
  {
    
  }

  void Update() override
  {
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
          FMODWrapper::Core::PlaySound("test", FLX_ASSET_GET(Asset::Sound, FLX_STRING_GET(element.GetComponent<Audio>()->audio_file)));
        }

        element.GetComponent<Audio>()->should_play = false;
      }
    }
  }

  void Stop() override
  {
    
  }
};

// Static instance to ensure registration
static CoreSystemsScript GameplayLoops;
