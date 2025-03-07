#include "Layers.h"
#include "FlexEngine.h"

#include "FMOD/FMODWrapper.h"

namespace Game
{
  std::vector<FlexECS::Entity> menu_buttons;
  int selected_button = 0;

  void MenuLayer::OnAttach()
  {
    File& file = File::Open(Path::current("assets/saves/mainmenu_v5.flxscene"));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));
    
    // Trigger music to start
    FlexECS::Scene::GetEntityByName("Main Menu BGM").GetComponent<Audio>()->should_play = true;

    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Start Game"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Settings"));
    menu_buttons.emplace_back(FlexECS::Scene::GetEntityByName("Quit"));
    
    // find camera
    for (auto& entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
    {
      CameraManager::SetMainGameCameraID(entity);
      break;
    }

    menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Hover.png)");
  }

  void MenuLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceStop();
  }

  void MenuLayer::Update()
  {
    if (Input::GetKeyDown(GLFW_KEY_S))
    {
      menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)");
      ++selected_button;
      if (selected_button > 2)
        selected_button = 0;

      menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Hover.png)");
    }
    else if (Input::GetKeyDown(GLFW_KEY_W))
    {
      menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Normal.png)");
      --selected_button;
      if (selected_button < 0)
        selected_button = 2;
      menu_buttons[selected_button].GetComponent<Sprite>()->sprite_handle = FLX_STRING_NEW(R"(/images/MainMenu/UI_Main_Menu_Button_Hover.png)");
    }

    if (Input::GetKeyDown(GLFW_KEY_SPACE))
    {
      // shit copy paste
      if (selected_button == 0)
      {
        Application::MessagingSystem::Send("Start Cutscene", true);
      }
      else if (selected_button == 2)
      {
        Application::QueueCommand(Application::Command::QuitApplication);
      }
    }

    if (Input::GetKey(GLFW_KEY_T))
    {
        Application::MessagingSystem::Send("CameraShakeStart", std::pair<double,double>(1.0,100.0));
    }
    if (Input::GetKey(GLFW_KEY_U))
    {
        Application::MessagingSystem::Send("CameraZoomStart", std::pair<double, double>(5.0, 1000.0));
    }
    if (Input::GetKey(GLFW_KEY_J))
    {
        Application::MessagingSystem::Send("CameraShakeLerpStart", std::pair<double, double>(5.0, 10.0));
    }
    if (Input::GetKey(GLFW_KEY_L))
    {
        Application::MessagingSystem::Send("CameraZoomAutoReturnStart", std::tuple<double, double, double>(3.0, 3.0, 100.0));
    }
  }
}
