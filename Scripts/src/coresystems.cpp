#include <FlexEngine.h>
using namespace FlexEngine;

class CoreSystemsScript : public Script
{
public:
  CoreSystemsScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "GameplayLoops"; }

  void Start() override
  {
   
  }

  void Update() override
  {
    // Transform
    
    // Physics
    
    // Graphics

    // Audio
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Audio>())
    {
      if (!element.GetComponent<Transform>()->is_active) continue; // skip non active entities

      FlexEngine::Audio* audio = element.GetComponent<Audio>();
      if (audio->should_play)
      {
        if (audio->audio_file == "")
        {
          Log::Warning("Audio not attached to entity: " +
            FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()));

          audio->should_play = false;
          continue;
        }

       /* if (audio->is_looping)
        {
          FMODWrapper::Core::PlayLoopingSound(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()),
                                              FLX_ASSET_GET(Asset::Sound, AssetKey{ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(element.GetComponent<Audio>()->audio_file) }));
        }
        else
        {
          FMODWrapper::Core::PlaySound(FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*element.GetComponent<EntityName>()),
                                       FLX_ASSET_GET(Asset::Sound, AssetKey{ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(element.GetComponent<Audio>()->audio_file) }));
        }*/

        element.GetComponent<Audio>()->should_play = false;
      }
    }

    //for (auto e : FlexECS::Scene::GetActiveScene()->Query<Transform>())
    //{
    //  std::cout << "Value of data[0]: " << e.GetComponent<Transform>()->transform.data[0] << std::endl;
    //}
  }

  void Stop() override
  {
    
  }
};

// Static instance to ensure registration
static CoreSystemsScript GameplayLoops;
