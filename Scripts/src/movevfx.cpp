#include <FlexEngine.h>
using namespace FlexEngine;



/////////////////////
//Documentation -.-//
/////////////////////

//Call Application::MessagingSystem::Send<bool>("Initialize VFX", true); at the start
//Because both Awake() and Start() doesnt seem to work, lol
//
//How to spawn VFX:
// - Send a "Spawn VFX" message through messaging system.
// Application::MessagingSystem::Send("Spawn VFX", std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>
//
// -Parameters, in order, are:
// Target entity, assetkey of spritesheet, position offset of VFX, VFX scale.
// Go adjust position and scale until it feels right


class MoveVFXSystemScript : public IScript
{
private:
  std::vector<std::pair<FlexEngine::FlexECS::Entity, bool>> m_vfx_pool;


public:
  MoveVFXSystemScript()
  {
    ScriptRegistry::RegisterScript(this);
  }

  std::string GetName() const override
  {
    return "MoveVFXSystem";
  }


  void Update() override
  {
    if (Application::MessagingSystem::Receive<bool>("Initialize VFX"))
    {
      m_vfx_pool.resize(7);
      for (size_t i = 0; i < 7; i++)
      {
        m_vfx_pool[i].first = FlexECS::Scene::CreateEntity("MoveVFX" + std::to_string(i));
        m_vfx_pool[i].first.AddComponent<Transform>({});
        m_vfx_pool[i].first.AddComponent<Position>({ {-50000,-50000,1} });
        m_vfx_pool[i].first.AddComponent<Rotation>({});
        m_vfx_pool[i].first.AddComponent<Scale>({ Vector3(5.0f, 5.0f, 5.0f) });
        m_vfx_pool[i].first.AddComponent<Sprite>({ });
        m_vfx_pool[i].first.AddComponent<Animator>({ });
        m_vfx_pool[i].first.AddComponent<ZIndex>({ 100 });
        
        auto& animator = *m_vfx_pool[i].first.GetComponent<Animator>();
        animator.should_play = false;
        animator.is_looping = false;

        m_vfx_pool[i].second = false;
      }
    }
                                                                        //list of targets          //vfx key  //positionoffset  //scale
    auto vfx_targets = Application::MessagingSystem::Receive<std::tuple<std::vector<FlexECS::Entity>, std::string, Vector3, Vector3>>("Spawn VFX");
    if (std::get<0>(vfx_targets).size() > 0)
    {
      for (size_t i = 0; i < std::get<0>(vfx_targets).size(); i++)
      {
        auto target_entity = std::get<0>(vfx_targets)[i];
        int j = 0;
        for (auto& e : m_vfx_pool)
        {
          ++j;
          if (e.second == true) continue;

          e.first.GetComponent<Position>()->position = target_entity.GetComponent<Position>()->position;
          e.first.GetComponent<Position>()->position += std::get<2>(vfx_targets);
          e.first.GetComponent<Scale>()->scale = std::get<3>(vfx_targets);

          e.first.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(std::get<1>(vfx_targets));
          e.first.GetComponent<Animator>()->should_play = true;
          e.first.GetComponent<Animator>()->is_looping = false;
          e.first.GetComponent<Animator>()->return_to_default = true;
          e.first.GetComponent<Animator>()->frame_time = 0.f;
          e.first.GetComponent<Animator>()->current_frame = 0;

          e.second = true;
          break;
        }
      }
    }
    
    //>>>>TAKE NOTE:<<<<<<
    //>>>>THIS IS A HACK<<<<< BECAUSE I DONT WANNA WAKE PEOPLE UP AT 3:08AM
    // Partially a graphics problem. After animation finishes, if it doesnt have a default animation,
    // A black box is shown.
    // Thus, we just move the black box far away lmao
    for (auto& e : m_vfx_pool)
    {
      if (e.second == false) continue;

      Animator& animator = *e.first.GetComponent<Animator>();
      if ((animator.current_frame >= animator.total_frames - 1) && (animator.total_frames != 0) && (animator.current_frame_time - animator.frame_time < 0.1))
      {
        e.first.GetComponent<Position>()->position = { -50000,-50000,0 };
        e.second = false;
      }
    }
  }

  void OnMouseEnter() override
  {

  }

  void OnMouseStay() override
  {

  }

  void OnMouseExit() override
  {

  }
};

// Static instance to ensure registration
static MoveVFXSystemScript MoveVFXSystem;
