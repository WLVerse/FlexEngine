#include "Layers.h"
#include "FlexEngine.h"

#include "FMOD/FMODWrapper.h"

namespace Game
{
  std::vector<FlexECS::Entity> win_layer_entities;

  void WinLayer::OnAttach()
  {
    //auto scene = FlexECS::Scene::CreateScene();
    //FlexECS::Scene::SetActiveScene(scene);
    auto scene = FlexECS::Scene::GetActiveScene();

    // Camera sizing
    FlexECS::Entity cam = scene->CreateEntity("Win Cam");
    cam.AddComponent<Position>({});
    cam.AddComponent<Rotation>({});
    cam.AddComponent<Scale>({});
    cam.AddComponent<Transform>({});
    Camera gameTestCamera({ 850.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
    cam.AddComponent<Camera>(gameTestCamera);
    win_layer_entities.push_back(cam);
      
    #pragma region Win UI spawns
    FlexECS::Entity ui = FlexECS::Scene::CreateEntity("UI_Lose_V");
    ui.AddComponent<Position>({ Vector3(0, -425 , 0) });
    ui.AddComponent<Scale>({ Vector3(56, 27, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Lose_V.png") });
    ui.AddComponent<ZIndex>({ 101 });
    win_layer_entities.push_back(ui);

    ui = FlexECS::Scene::CreateEntity("Player Stats");
    ui.AddComponent<Position>({ Vector3(-533, -49, 0) });
    ui.AddComponent<Scale>({ Vector3(334, 100, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Win_Player Stats.png") });
    ui.AddComponent<ZIndex>({ 100 });
    win_layer_entities.push_back(ui);

    ui = FlexECS::Scene::CreateEntity("Win base");
    ui.AddComponent<Position>({ Vector3(0, 0, 0) });
    ui.AddComponent<Scale>({ Vector3(1600, 675, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Win_Screen_Base.png") });
    ui.AddComponent<ZIndex>({ 100 });
    win_layer_entities.push_back(ui);

    ui = FlexECS::Scene::CreateEntity("Press any button");
    ui.AddComponent<Position>({ Vector3(0, -370, 0) });
    ui.AddComponent<Scale>({ Vector3(455, 44, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Sprite>({ FLX_STRING_NEW("/images/battle ui/UI_Win_Text_Press Any Button To Continue.png") });
    ui.AddComponent<ZIndex>({ 101 });
    win_layer_entities.push_back(ui);


    ui = FlexECS::Scene::CreateEntity("dmg dealt");
    ui.AddComponent<Position>({ Vector3(-674, -58, 0) });
    ui.AddComponent<Scale>({ Vector3(0.28, 0.3, 0.3) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<ZIndex>({ 101 });
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                            FLX_STRING_NEW(R"(DMG DEALT)") });
    win_layer_entities.push_back(ui);

    ui = FlexECS::Scene::CreateEntity("dmg value");
    ui.AddComponent<Position>({ Vector3(-423, -57, 0) });
    ui.AddComponent<Scale>({ Vector3(0.28, 0.3, 0.3) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<ZIndex>({ 101 });
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"), FLX_STRING_NEW(R"(0000)") }
    );
    win_layer_entities.push_back(ui);

    ui = FlexECS::Scene::CreateEntity("completion time value");
    ui.AddComponent<Position>({ Vector3(-545, 48, 0) });
    ui.AddComponent<Scale>({ Vector3(2.2, 3, 1) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<ZIndex>({ 101 });
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                            FLX_STRING_NEW(R"(1m52s)") });
    win_layer_entities.push_back(ui);

    ui = FlexECS::Scene::CreateEntity("renko text");
    ui.AddComponent<Position>({ Vector3(-669, -24, 0) });
    ui.AddComponent<Scale>({ Vector3(0.45, 0.5, 0.5) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<ZIndex>({ 101 });
    ui.AddComponent<Transform>({});
    ui.AddComponent<Text>({ FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
                            FLX_STRING_NEW(R"(Renko)") });
    win_layer_entities.push_back(ui);

    ui = FlexECS::Scene::CreateEntity("win audio");
    ui.AddComponent<Position>({ Vector3(-669, -24, 0) });
    ui.AddComponent<Scale>({ Vector3(0.45, 0.5, 0.5) });
    ui.AddComponent<Rotation>({});
    ui.AddComponent<Transform>({});
    ui.AddComponent<Audio>({ true, false, false, false, FLX_STRING_NEW(R"(\audio\win.mp3)") });
    win_layer_entities.push_back(ui);

    #pragma endregion

    // set camera
    CameraManager::SetMainGameCameraID(cam);

    // shoddy shift
    for (auto& entity : win_layer_entities)
    {
      entity.GetComponent<Position>()->position += Vector3(950, 550, 0);
      entity.GetComponent<Scale>()->scale *= 1.4f;
    }
  }

  void WinLayer::OnDetach()
  {
    auto scene = FlexECS::Scene::GetActiveScene();
    for (auto& entity : win_layer_entities) scene->DestroyEntity(entity);
  }

  void WinLayer::Update()
  {

  }
}
