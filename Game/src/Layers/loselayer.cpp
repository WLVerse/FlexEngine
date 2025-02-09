#include "Layers.h"
#include "FlexEngine.h"

#include "FMOD/FMODWrapper.h"

namespace Game
{
  void LoseLayer::OnAttach()
  {
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);

    // Camera sizing
    {
      FlexECS::Entity cam = scene->CreateEntity("Test Cam");
      cam.AddComponent<Position>({});
      cam.AddComponent<Rotation>({});
      cam.AddComponent<Scale>({});
      cam.AddComponent<Transform>({});
      // There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write
      // each exact variable cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
      //  Second way is to create a camera outside and then copy constructor it -> Easier
      Camera gameTestCamera({ 850.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
      cam.AddComponent<Camera>(gameTestCamera);
    }
      
    #pragma region Lose UI spawns
    FlexECS::Entity ui = FlexECS::Scene::CreateEntity("UI_Lose_V");
    ui.AddComponent<Position>({ Vector3(0, -350 , 0) });
    ui.AddComponent<Scale>({ Vector3(56, 27, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Lose_V.png") });
    ui.AddComponent<ZIndex>({ 1 });

    ui = FlexECS::Scene::CreateEntity("Lose Base");
    ui.AddComponent<Position>({ Vector3(-0.822, 20, 0) });
    ui.AddComponent<Scale>({ Vector3(1600, 500, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Lose_Base_Screen.png") });
    ui.AddComponent<ZIndex>({ 0 });

    ui = FlexECS::Scene::CreateEntity("Press any button");
    ui.AddComponent<Position>({ Vector3(0, -300, 0) });
    ui.AddComponent<Scale>({ Vector3(455, 44, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Win_Text_Press Any Button To Continue.png") });

    // Box dimension is 1900x300
    ui = FlexECS::Scene::CreateEntity("git gud noob");
    ui.AddComponent<Position>({ Vector3(-280, -130, 0) });
    ui.AddComponent<Scale>({ Vector3(0.3, 0.3, 0.3) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<ZIndex>({ 1 });
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                                 FLX_STRING_NEW(R"(Losing? Try upgrading your gear or trying a different approach to the fight)"),
                                 Vector3::One,
                                 std::make_pair(1, 1),
                                 Vector2(1900, 300)});

    ui = FlexECS::Scene::CreateEntity("lose audio");
    ui.AddComponent<Position>({ Vector3(-669, -24, 0) });
    ui.AddComponent<Scale>({ Vector3(0.45, 0.5, 0.5) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Audio>({ true, false, false, false, FLX_STRING_NEW(R"(\audio\lose.mp3)")});

    #pragma endregion

    // find camera
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
    {
      CameraManager::SetCamera(entity, entity.GetComponent<Camera>());
      break;
    }
  }

  void LoseLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceStop();
  }

  void LoseLayer::Update()
  {

  }
}
