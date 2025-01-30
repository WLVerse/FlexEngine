#include "Layers.h"
#include "FlexEngine.h"

namespace Game
{
  void BaseLayer::LoadDLL()
  {
    // guard
    if (is_scripting_dll_loaded) return;

    // Create the hotload directory if it does not exist
    if (!std::filesystem::exists(Path::current("hotload"))) std::filesystem::create_directory(Path::current("hotload"));

    // Get the path of the DLL
    std::string from = Path::current("Scripts.dll");
    std::string to = Path::current("hotload/Scripts.dll");

    auto& layerstack = Application::GetCurrentWindow()->GetLayerStack();

    // guard: DLL does not exist
    if (!std::filesystem::exists(from))
    {
      Log::Error("DLL for hotloading does not exist. Remember to build the scripting sln first.");
      layerstack.RemoveLayer(this->GetName()); // Remove self
      return;
    }

    // Copy the DLL to the hot loading directory
    bool success = std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing);
    if (!success)
    {
      Log::Error("Failed to copy DLL for hotloading.");
      layerstack.RemoveLayer(this->GetName()); // Remove self
      return;
    }

    // Load the scripting DLL
    LPCSTR dll_path = to.c_str();
    hmodule_scripting = LoadLibraryA(dll_path);
    if (!hmodule_scripting)
    {
      Log::Error("Failed to load DLL. Error code: " + std::to_string(GetLastError()));
      layerstack.RemoveLayer(this->GetName()); // Remove self
      return;
    }
    Log::Info("Loaded DLL: " + to);

    is_scripting_dll_loaded = true;
  }

  void BaseLayer::OnAttach()
  {
    // First, create a window
    FLX_COMMAND_OPEN_WINDOW(
      "Game",
      WindowProps(
        "Chrono Drift",
        1600, 900,
        {
          FLX_DEFAULT_WINDOW_HINTS,
          { GLFW_DECORATED, true },
          { GLFW_RESIZABLE, true }
        }
      )
    );

    // Second, load assets
    FLX_COMMAND_ADD_APPLICATION_LAYER(std::make_shared<LoadLayer>());

    LoadDLL();
  }

  void BaseLayer::OnDetach()
  {
    // guard
    if (!is_scripting_dll_loaded) return;

    if (hmodule_scripting) FreeLibrary(hmodule_scripting);
    hmodule_scripting = nullptr;
    ScriptRegistry::ClearScripts();

    is_scripting_dll_loaded = false;

    FLX_COMMAND_CLOSE_WINDOW("Game");
  }

  void BaseLayer::Update()
  {
    // Runs the main gameplay loop, this should not be removed
    ScriptRegistry::GetScript("RenderLoop")->Update();
    ScriptRegistry::GetScript("GameplayLoops")->Update();
    ScriptRegistry::GetScript("CameraHandler")->Update();
  }
}
