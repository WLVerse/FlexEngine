#include "Layers.h"
#include "FlexEngine.h"

#include "FMOD/FMODWrapper.h"

namespace Game
{
  void WinLayer::OnAttach()
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
      
    #pragma region Win UI spawns
    FlexECS::Entity ui = FlexECS::Scene::CreateEntity("UI_Lose_V");
    ui.AddComponent<Position>({ Vector3(0, -425 , 0) });
    ui.AddComponent<Scale>({ Vector3(56, 27, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Lose_V.png") });

    ui = FlexECS::Scene::CreateEntity("Player Stats");
    ui.AddComponent<Position>({ Vector3(-533, -49, 0) });
    ui.AddComponent<Scale>({ Vector3(334, 100, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Win_Player Stats.png") });

    ui = FlexECS::Scene::CreateEntity("Press any button");
    ui.AddComponent<Position>({ Vector3(0, -370, 0) });
    ui.AddComponent<Scale>({ Vector3(455, 44, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Win_Text_Press Any Button To Continue.png") });

    ui = FlexECS::Scene::CreateEntity("dmg dealt");
    ui.AddComponent<Position>({ Vector3(-674, -58, 0) });
    ui.AddComponent<Scale>({ Vector3(0.28, 0.3, 0.3) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                            FLX_STRING_NEW(R"(DMG DEALT)")});

    ui = FlexECS::Scene::CreateEntity("dmg value");
    ui.AddComponent<Position>({ Vector3(-423, -57, 0) });
    ui.AddComponent<Scale>({ Vector3(0.28, 0.3, 0.3) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                            FLX_STRING_NEW(R"(0000)")});

    ui = FlexECS::Scene::CreateEntity("completion time text");
    ui.AddComponent<Position>({ Vector3(-627, 172, 0) });
    ui.AddComponent<Scale>({ Vector3(0.45, 0.5, 0.5) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                            FLX_STRING_NEW(R"(Completion Time)")});

    ui = FlexECS::Scene::CreateEntity("completion time value");
    ui.AddComponent<Position>({ Vector3(-545, 48, 0) });
    ui.AddComponent<Scale>({ Vector3(2.2, 3, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                            FLX_STRING_NEW(R"(1m52s)")});

    ui = FlexECS::Scene::CreateEntity("renko text");
    ui.AddComponent<Position>({ Vector3(-669, -24, 0) });
    ui.AddComponent<Scale>({ Vector3(0.45, 0.5, 0.5) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                            FLX_STRING_NEW(R"(Renko)")});

    ui = FlexECS::Scene::CreateEntity("win audio");
    ui.AddComponent<Position>({ Vector3(-669, -24, 0) });
    ui.AddComponent<Scale>({ Vector3(0.45, 0.5, 0.5) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Audio>({ true, false, false, false, FLX_STRING_NEW(R"(\audio\win.mp3)")});

    #pragma endregion
    // find camera
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
    {
      CameraManager::SetCamera(entity, entity.GetComponent<Camera>());
      break;
    }
  }

  void WinLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceStop();
  }

  void WinLayer::Update()
  {

  }
}
