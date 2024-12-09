#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace OpenGLRendering
{

  class MainLayer : public FlexEngine::Layer
  {
    FlexECS::Entity main_camera;
    FlexECS::Entity directional_light;
    std::vector<FlexECS::Entity> point_lights;

    FlexECS::Entity object;
    FlexECS::Entity sprite;

    const Path default_save_directory = Path::current("saves");
    const std::string default_save_name = "default";
    Path current_save_directory = default_save_directory;
    std::string current_save_name = default_save_name;

  public:
    MainLayer() : Layer("Main Layer") {}
    ~MainLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;

  private:
    void CreateDefaultScene();
  };

}
