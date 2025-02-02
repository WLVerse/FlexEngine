#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void MenuLayer::OnAttach()
  {
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);
    {
        FlexECS::Entity cam = scene->CreateEntity("Test Cam");
        cam.AddComponent<Position>({});
        cam.AddComponent<Rotation>({});
        cam.AddComponent<Scale>({});
        cam.AddComponent<Transform>({});
        //There are two ways to initialize, 1st is to write directly which i do not recommend like so -> need to write each exact variable
        //cam.AddComponent<Camera>({ {{ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f},false});
        // Second way is to create a camera outside and then copy constructor it -> Easier
        Camera gameTestCamera({ 850.0f,450.0f,0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
        cam.AddComponent<Camera>(gameTestCamera);
    }
    {
        FlexECS::Entity button = scene->CreateEntity("Start Button");
        button.AddComponent<Position>({ Vector3(200.0f, 200.0f, 0.0f) });
        button.AddComponent<Rotation>({});
        button.AddComponent<Scale>({ Vector3(200.f, 100.0f, 0.0f) });
        button.AddComponent<Transform>({});
        button.AddComponent<Button>({});
        button.AddComponent<BoundingBox2D>({});
        button.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)"), -1 });
        button.GetComponent<Sprite>()->center_aligned = true;
        button.AddComponent<Script>({ FLX_STRING_NEW("MenuButton") });

        button = scene->CreateEntity("Exit Button");
        button.AddComponent<Position>({ Vector3(200.0f, 800.f, 0.0f) });
        button.AddComponent<Rotation>({});
        button.AddComponent<Scale>({ Vector3(200.f, 100.0f, 0.0f) });
        button.AddComponent<Transform>({});
        button.AddComponent<Button>({});
        button.AddComponent<BoundingBox2D>({});
        button.AddComponent<Sprite>({});
        button.GetComponent<Sprite>()->center_aligned = true;
        button.AddComponent<Script>({ FLX_STRING_NEW("QuitButton") });
    }
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
