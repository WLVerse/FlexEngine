#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void MenuLayer::OnAttach()
  {
    #pragma region Menu Spawns
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);
    {
      FlexECS::Entity cam = scene->CreateEntity("Test Cam");
      cam.AddComponent<Position>({});
      cam.AddComponent<Rotation>({});
      cam.AddComponent<Scale>({});
      cam.AddComponent<Transform>({});
      //There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write each exact variable
      Camera gameTestCamera({ 1920.0f/2,1080.f/2,0 }, 1920, 1080.f, -2.0f, 2.0f);
      cam.AddComponent<Camera>(gameTestCamera);
    }
    {
      FlexECS::Entity background = scene->CreateEntity("Background");
      background.AddComponent<Position>({ Vector3(0.f, 0.f, 0.0f) });
      background.AddComponent<Rotation>({ Vector3(180.f, 0.f, 0.f) }); // Rotation is not needed for 2D
      background.AddComponent<Scale>({ Vector3(1920.f, 1080.f, 0.0f) });
      background.AddComponent<Transform>({}); // Transform is not needed for 2D
      background.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/MainMenu/Env_Main_Menu_BG.png)"), -1 });
      background.GetComponent<Sprite>()->center_aligned = true;

      FlexECS::Entity button = scene->CreateEntity("Start Button");
      button.AddComponent<Position>({ Vector3(126.f, 450.f, 0.f) });
      button.AddComponent<Rotation>({});
      button.AddComponent<Scale>({ Vector3(800.f, 50.f, 0.0f) });
      button.AddComponent<Transform>({});
      button.AddComponent<Button>({});
      button.AddComponent<BoundingBox2D>({});
      button.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)"), -1 });
      button.GetComponent<Sprite>()->center_aligned = true;
      button.AddComponent<Script>({ FLX_STRING_NEW("MenuButton") });

      button = scene->CreateEntity("Drifters");
      button.AddComponent<Position>({ Vector3(126.f, 523.f, 0.f) });
      button.AddComponent<Rotation>({});
      button.AddComponent<Scale>({ Vector3(800.f, 50.f, 0.0f) });
      button.AddComponent<Transform>({});
      button.AddComponent<Button>({});
      button.AddComponent<BoundingBox2D>({});
      button.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)"), -1 });
      button.GetComponent<Sprite>()->center_aligned = true;

      button = scene->CreateEntity("Codex");
      button.AddComponent<Position>({ Vector3(126.f, 597.f, 0.f) });
      button.AddComponent<Rotation>({});
      button.AddComponent<Scale>({ Vector3(800.f, 50.f, 0.0f) });
      button.AddComponent<Transform>({});
      button.AddComponent<Button>({});
      button.AddComponent<BoundingBox2D>({});
      button.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)"), -1 });
      button.GetComponent<Sprite>()->center_aligned = true;

      button = scene->CreateEntity("Settings");
      button.AddComponent<Position>({ Vector3(126.f, 672.f, 0.f) });
      button.AddComponent<Rotation>({});
      button.AddComponent<Scale>({ Vector3(800.f, 50.f, 0.0f) });
      button.AddComponent<Transform>({});
      button.AddComponent<Button>({});
      button.AddComponent<BoundingBox2D>({});
      button.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)"), -1 });
      button.GetComponent<Sprite>()->center_aligned = true;

      button = scene->CreateEntity("Exit Button");
      button.AddComponent<Position>({ Vector3(126.f, 747.f, 0.f) });
      button.AddComponent<Rotation>({});
      button.AddComponent<Scale>({ Vector3(800.f, 50.f, 0.0f) });
      button.AddComponent<Transform>({});
      button.AddComponent<Button>({});
      button.AddComponent<BoundingBox2D>({});
      button.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)"), -1 });
      button.GetComponent<Sprite>()->center_aligned = true;
      button.AddComponent<Script>({ FLX_STRING_NEW("QuitButton") });
    }
    #pragma endregion

    Log::Info("Menu Layer Attach");
  }

  void MenuLayer::OnDetach()
  {
    Log::Info("Menu Layer Detach");
  }

  void MenuLayer::Update()
  {
  }
}
