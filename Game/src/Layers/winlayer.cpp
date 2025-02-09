#include "Layers.h"
#include "FlexEngine.h"

#include "FMOD/FMODWrapper.h"

namespace Game
{
  void WinLayer::OnAttach()
  {
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);

    FlexECS::Entity ui = FlexECS::Scene::CreateEntity("UI_Lose_V");
    ui.AddComponent<Position>({Vector3(0, -425 , 0)});
    ui.AddComponent<Scale>({ Vector3(56, 27, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("images/battle ui/UI_Lose_V.png")});

    ui = FlexECS::Scene::CreateEntity("Player Stats");
    ui.AddComponent<Position>({ Vector3(0, 28, 0) });
    ui.AddComponent<Scale>({ Vector3(1900, 700, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("images/battle ui/UI_Win_Player Stats.png") });

    ui = FlexECS::Scene::CreateEntity("Press any button");
    ui.AddComponent<Position>({ Vector3(0, -370, 0) });
    ui.AddComponent<Scale>({ Vector3(455, 44, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("images/battle ui/UI_Win_Text_Press Any Button To Continue.png") });
    
    // find camera
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
    {
      CameraManager::SetCamera(entity, entity.GetComponent<Camera>());
      break;
    }
  }

  void MenuLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceStop();
  }

  void MenuLayer::Update()
  {

  }
}
