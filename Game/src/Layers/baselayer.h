#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{

  class BaseLayer : public FlexEngine::Layer
  {
  public:
    BaseLayer() : Layer("Base Layer") {}
    ~BaseLayer() = default;

    bool is_scripting_dll_loaded = false;
    HMODULE hmodule_scripting{};

    void LoadDLL();
    void UnloadDLL();

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
