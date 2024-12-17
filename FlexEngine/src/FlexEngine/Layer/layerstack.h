// WLVERSE [https://wlverse.web.app]
// layerstack.h
// 
// A manager for layers and overlays that segregates logic into
// different layers that run in a specific order.
// Add and remove operations are queued to ensure that they
// happen at the end of the update function to prevent
// undefined behavior and invalidating iterators.
// 
// The layer manager implementation itself uses the Template Method Pattern.
// 
// Usage:
// layerstack.AddLayer(std::make_shared<YourLayer>(), "YourLayerName", 2);
// layerstack.RemoveLayer("YourLayerName");
//
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "flx_api.h"

#include "DataStructures/functionqueue.h"
#include "ilayer.h" // <string>

#include <memory>
#include <vector>
#include <set>

// enables the check if index is out of bounds
// clamping might cause undefined behavior
#define LAYERSTACK_ALLOW_OOB_INDEX

namespace FlexEngine
{

  class __FLX_API LayerStack
  {
    std::vector<std::shared_ptr<Layer>> m_layers{};
    std::vector<std::shared_ptr<Layer>> m_overlays{};

    // Lists for adding and removing layers and overlays.
    // Add and remove operations are queued to ensure that they
    // happen at the end of the update function to prevent
    // undefined behavior and invalidating iterators.

    enum class LayerType
    {
      Layer,
      Overlay
    };
    
    //std::set<std::tuple<LayerType, std::shared_ptr<Layer>, size_t>> m_to_add;
    //std::set<std::tuple<LayerType, size_t>> m_to_remove;

    // Command pattern

  private:
    FunctionQueue m_to_add{};
    FunctionQueue m_to_remove{};

  public:
    FunctionQueue& GetToAdd() { return m_to_add; }
    FunctionQueue& GetToRemove() { return m_to_remove; }

  public:
    LayerStack();
    ~LayerStack();

    // Removes all layers and overlays.
    void Clear();

    bool m_to_clear = false;

    // Checks if the layer stack is in the process of clearing.
    bool IsClearing() const { return m_to_clear; }

    // Updates all layers and overlays.
    void Update();

    // Checks if a layer exists in the layer stack.
    bool HasLayer(const std::string& layer_name) const;

    // Adds a layer or overlay to the layer stack.
    #pragma region Add

  public:
    void AddLayer(std::shared_ptr<Layer> layer) { AddLayer(layer, m_layers.size()); }
    void AddLayer(std::shared_ptr<Layer> layer, size_t index) { AddLayer(layer, index, ""); }
    void AddLayer(std::shared_ptr<Layer> layer, const std::string& layer_name) { AddLayer(layer, m_layers.size(), layer_name); }
    void AddLayer(std::shared_ptr<Layer> layer, size_t index, const std::string& layer_name);
    
    void AddOverlay(std::shared_ptr<Layer> overlay) { AddOverlay(overlay, m_overlays.size()); }
    void AddOverlay(std::shared_ptr<Layer> overlay, size_t index) { AddOverlay(overlay, index, ""); }
    void AddOverlay(std::shared_ptr<Layer> overlay, const std::string& overlay_name) { AddOverlay(overlay, m_overlays.size(), overlay_name); }
    void AddOverlay(std::shared_ptr<Layer> overlay, size_t index, const std::string& overlay_name);

    #pragma endregion

    // Removes a layer or overlay from the layer stack.
    #pragma region Remove

  public:
    void RemoveLayer(const std::string& layer_name);
    void RemoveLayer(size_t index);
    
    void RemoveOverlay(const std::string& overlay_name);
    void RemoveOverlay(size_t index);

    #pragma endregion

    #pragma region Accessors

  public:
    void SetLayerName(size_t index, const std::string& name) { m_layers[index]->SetName(name); }
    const std::string& GetLayerName(size_t index) const { return m_layers[index]->GetName(); }

    void SetOverlayName(size_t index, const std::string& name) { m_overlays[index]->SetName(name); }
    const std::string& GetOverlayName(size_t index) const { return m_overlays[index]->GetName(); }

    size_t GetLayerCount() const { return m_layers.size(); }
    size_t GetOverlayCount() const { return m_overlays.size(); }

    std::shared_ptr<Layer> GetLayer(size_t index) const { return m_layers[index]; }
    std::shared_ptr<Layer> GetLayer(const std::string& layer_name) const;

    std::shared_ptr<Layer> GetOverlay(size_t index) const { return m_overlays[index]; }
    std::shared_ptr<Layer> GetOverlay(const std::string& overlay_name) const;

    #pragma endregion

    #pragma region Internal Functions

  public:
    // INTERNAL FUNCTION
    // This function is called at the end of the update function to
    // add or remove layers and overlays.
    // Add and remove operations are queued to ensure that they
    // happen at the end of the update function to prevent
    // undefined behavior and invalidating iterators.
    void Internal_ExecuteDeferredCommands();

    void Internal_Clear();
    void Internal_Add(LayerType type, std::shared_ptr<Layer> layer, size_t index);
    void Internal_Remove(LayerType type, size_t index);

    #pragma endregion

#ifdef _DEBUG
    // Dumps the layer stack to the console.
    void Dump();
#endif
  };

}