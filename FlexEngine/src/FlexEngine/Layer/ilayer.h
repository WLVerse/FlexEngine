// WLVERSE [https://wlverse.web.app]
// ilayer.h
// 
// The interface for layers in the state manager.
// 
// Usage:
// 
// class YourLayer : public Layer
// {
// public:
//   void OnAttach() override
//   {
//     // your code here
//   }
// 
//   void Update() override
//   {
//     // your code here
//   }
// 
//   void OnDetach() override
//   {
//     // your code here
//   }
// };
// 
// The layer manager implementation itself uses the Template Method Pattern.
// 
// Usage: layerstack.PushLayer(std::make_shared<YourLayer>());
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <string>

namespace FlexEngine
{

  // abstract class Layer
  // build upon this class to create your own layers
  class Layer
  {
    std::string m_layer_name = "";

  public:
    Layer(const std::string& name = "Layer") : m_layer_name(name) {}
    virtual ~Layer() = default;

    // Set the name of the layer
    void SetName(const std::string& name) { m_layer_name = name; }

    // Get the name of the layer
    const std::string& GetName() const { return m_layer_name; }

    // called once when layer is attached to the layer stack
    virtual void OnAttach() = 0;

    // called once when layer is detached from the layer stack
    virtual void OnDetach() = 0;

    // automatically updated by the layer stack
    virtual void Update() = 0;
  };

}