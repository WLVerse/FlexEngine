#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Editor
{

  class EditorBaseLayer : public FlexEngine::Layer
  {
    const Path default_save_directory = Path::current("saves");
    const std::string default_save_name = "default";
    Path current_save_directory = default_save_directory;
    std::string current_save_name = default_save_name;

    ImGuiID dockspace_main_id = 0;

  public:
    EditorBaseLayer() : Layer("Editor Base Layer") {}
    ~EditorBaseLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  };

}
