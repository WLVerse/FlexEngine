// WLVERSE [https://wlverse.web.app]
// layerstack.h
// 
// A manager for layers and overlays that segregates logic into
// different layers that run in a specific order.
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

#include "ilayer.h" // <string>

#include <memory>
#include <vector>

namespace FlexEngine
{

  class LayerStack
  {
    std::vector<std::shared_ptr<Layer>> m_layers{};
    std::vector<std::shared_ptr<Layer>> m_overlays{};

  public:
    LayerStack();
    ~LayerStack();

    // Removes all layers and overlays.
    void Clear();

    // Pushes a layer to the layer stack.
    void PushLayer(std::shared_ptr<Layer> layer);

    // Pops the last layer from the layer stack.
    void PopLayer();

    // Pushes an overlay to the layer stack.
    void PushOverlay(std::shared_ptr<Layer> overlay);

    // Pops the last overlay from the layer stack.
    void PopOverlay();

    // Inserts a layer at the specified index.
    void InsertLayer(std::shared_ptr<Layer> layer, size_t index);

    // Inserts an overlay at the specified index.
    void InsertOverlay(std::shared_ptr<Layer> overlay, size_t index);

    // Removes a layer at the specified index.
    void RemoveLayer(size_t index);

    // Removes an overlay at the specified index.
    void RemoveOverlay(size_t index);

    // Updates all layers and overlays.
    void Update();

    #pragma region Accessors

    void SetLayerName(size_t index, const std::string& name) { m_layers[index]->SetName(name); }
    const std::string& GetLayerName(size_t index) const { return m_layers[index]->GetName(); }

    void SetOverlayName(size_t index, const std::string& name) { m_overlays[index]->SetName(name); }
    const std::string& GetOverlayName(size_t index) const { return m_overlays[index]->GetName(); }

    size_t GetLayerCount() const { return m_layers.size(); }
    size_t GetOverlayCount() const { return m_overlays.size(); }

    std::shared_ptr<Layer> GetLayer(size_t index) const { return m_layers[index]; }
    std::shared_ptr<Layer> GetOverlay(size_t index) const { return m_overlays[index]; }

    #pragma endregion


#ifdef _DEBUG
    // Dumps the layer stack to the console.
    void Dump();
#endif
  };

}