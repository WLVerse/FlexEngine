// WLVERSE [https://wlverse.web.app]
// layerstack.cpp
//
// A manager for layers and overlays that segregates logic into different layers
// that run in a specific order. 
// Add and remove operations are queued to ensure that they happen at the end of
// the update function to prevent undefined behavior and invalidating iterators. 
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
// Copyright (c) 2025 DigiPen, All rights reserved.

#include "pch.h"

#include "layerstack.h"

namespace FlexEngine
{

  LayerStack::LayerStack()
  {

  }

  LayerStack::~LayerStack()
  {
    Internal_Clear();
  }

  void LayerStack::Clear()
  {
    m_to_clear = true;
  }

  void LayerStack::Internal_Clear()
  {
    FLX_FLOW_FUNCTION();

    for (auto& overlay : m_overlays)
    {
      overlay->OnDetach();
      overlay.reset();
    }
    m_overlays.clear();

    for (auto& layer : m_layers)
    {
      layer->OnDetach();
      layer.reset();
    }
    m_layers.clear();

    m_to_add.Clear();
    m_to_remove.Clear();
  }

  void LayerStack::Update()
  {
    for (auto& layer : m_layers) layer->Update();
    for (auto& overlay : m_overlays) overlay->Update();
  }

  bool LayerStack::HasLayer(const std::string& layer_name) const
  {
    for (auto& layer : m_layers)
    {
      if (layer->GetName() == layer_name) return true;
    }
    return false;
  }

  void LayerStack::AddLayer(std::shared_ptr<Layer> layer, size_t index, const std::string& layer_name)
  {
    std::string _layer_name = (layer_name.empty() ? layer->GetName() : layer_name);

    Log::Debug("Adding layer: " + _layer_name);

    // guard: check if layer is nullptr
    if (!layer)
    {
      Log::Error("Can't add a null layer to the layerstack.");
      return;
    }

    // guard: check if name already exists
    for (auto& l : m_layers)
    {
      if (l->GetName() == _layer_name)
      {
        Log::Error("The layer name already exists. It's either a duplicate AddLayer call or simply conflicting naming. Layer name: " + _layer_name);
        return;
      }
    }

    // guard: check if index is out of bounds
#if defined(LAYERSTACK_ALLOW_OOB_INDEX)
    if (index > m_layers.size())
    {
      Log::Error(
        "Can't add the layer at the specified index because it is out of bounds. "
        "Layer name: " + _layer_name + " Index: " + std::to_string(index) + " Layer stack size: " + std::to_string(m_layers.size())
      );
      return;
    }
#else
    if (index > m_layers.size())
    {
      index = m_layers.size();
    }
#endif

    m_layers.insert(m_layers.begin() + index, layer);
    layer->OnAttach();
  }

  void LayerStack::AddOverlay(std::shared_ptr<Layer> overlay, size_t index, const std::string& overlay_name)
  {
    std::string _overlay_name = (overlay_name.empty() ? overlay->GetName() : overlay_name);

    Log::Debug("Adding overlay: " + _overlay_name);

    // guard: check if overlay is nullptr
    if (!overlay)
    {
      Log::Error("Can't add a null overlay to the layerstack.");
      return;
    }

    // guard: check if name already exists
    for (auto& o : m_overlays)
    {
      if (o->GetName() == _overlay_name)
      {
        Log::Error("The overlay name already exists. It's either a duplicate AddOverlay call or simply conflicting naming. Overlay name: " + _overlay_name);
        return;
      }
    }

    // guard: check if index is out of bounds
#if defined(LAYERSTACK_ALLOW_OOB_INDEX)
    if (index > m_overlays.size())
    {
      Log::Error(
        "Can't add the overlay at the specified index because it is out of bounds. "
        "Overlay name: " + _overlay_name + " Index: " + std::to_string(index) + " Overlay stack size: " + std::to_string(m_overlays.size())
      );
      return;
    }
#else
    if (index > m_overlays.size())
    {
      index = m_overlays.size();
    }
#endif

    m_overlays.insert(m_overlays.begin() + index, overlay);
    overlay->OnAttach();
  }

  void LayerStack::RemoveLayer(const std::string& layer_name)
  {
    // guard: check if layer exists
    auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const auto& layer) { return layer->GetName() == layer_name; });
    if (it == m_layers.end())
    {
      Log::Error("Cannot remove a layer that doesn't exist. Layer name: " + layer_name);
      return;
    }

    RemoveLayer(std::distance(m_layers.begin(), it));
  }

  void LayerStack::RemoveLayer(size_t index)
  {
    //FLX_FLOW_FUNCTION();
    Log::Debug("Removing layer at index: " + std::to_string(index));

    // guard: check if index is out of bounds
    if (index >= m_layers.size())
    {
      Log::Error("Cannot remove layer because the specified index is out of bounds. Index: " + std::to_string(index));
      return;
    }

    m_layers[index]->OnDetach();
    m_layers.erase(m_layers.begin() + index);
  }

  void LayerStack::RemoveOverlay(const std::string& overlay_name)
  {
    // guard: check if overlay exists
    auto it = std::find_if(m_overlays.begin(), m_overlays.end(), [&](const auto& overlay) { return overlay->GetName() == overlay_name; });
    if (it == m_overlays.end())
    {
      Log::Error("Cannot remove an overlay that doesn't exist. Overlay name: " + overlay_name);
      return;
    }

    RemoveOverlay(std::distance(m_overlays.begin(), it));
  }

  void LayerStack::RemoveOverlay(size_t index)
  {
    //FLX_FLOW_FUNCTION();
    Log::Debug("Removing overlay at index: " + std::to_string(index));

    // guard: check if index is out of bounds
    if (index >= m_overlays.size())
    {
      Log::Error("Cannot remove overlay because the specified index is out of bounds. Index: " + std::to_string(index));
      return;
    }

    m_overlays[index]->OnDetach();
    m_overlays.erase(m_overlays.begin() + index);
  }

  std::shared_ptr<Layer> LayerStack::GetLayer(const std::string& layer_name) const
  {
    auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const auto& layer) { return layer->GetName() == layer_name; });
    return it != m_layers.end() ? *it : nullptr;
  }

  std::shared_ptr<Layer> LayerStack::GetOverlay(const std::string& overlay_name) const
  {
    auto it = std::find_if(m_overlays.begin(), m_overlays.end(), [&](const auto& overlay) { return overlay->GetName() == overlay_name; });
    return it != m_overlays.end() ? *it : nullptr;
  }

  void LayerStack::Internal_Add(LayerType type, std::shared_ptr<Layer> layer, size_t index)
  {
    switch (type)
    {
    case LayerType::Layer:
      m_layers.insert(m_layers.begin() + index, layer);
      layer->OnAttach();
      break;
    case LayerType::Overlay:
      m_overlays.insert(m_overlays.begin() + index, layer);
      layer->OnAttach();
      break;
    default:
      Log::Error("Tried to add a layer with an invalid type in LayerStack::Internal_Add.");
      break;
    }
  }

  void LayerStack::Internal_Remove(LayerType type, size_t index)
  {
    switch (type)
    {
    case LayerType::Layer:
      m_layers[index]->OnDetach();
      m_layers.erase(m_layers.begin() + index);
      break;
    case LayerType::Overlay:
      m_overlays[index]->OnDetach();
      m_overlays.erase(m_overlays.begin() + index);
      break;
    default:
      Log::Error("Tried to remove a layer with an invalid type in LayerStack::Internal_Remove.");
      break;
    }
  }

#ifdef _DEBUG
  void LayerStack::Dump()
  {
    Log::Debug(__FUNCTION__);

    Log::Debug("Layers (" + std::to_string(m_layers.size()) + ")");
    for (auto& layer : m_layers)
    {
      Log::Debug(std::string(2, ' ') + layer->GetName().c_str());
    }

    Log::Debug("Overlays (" + std::to_string(m_overlays.size()) + ")");
    for (auto& overlay : m_overlays)
    {
      Log::Debug(std::string(2, ' ') + overlay->GetName().c_str());
    }

    Log::Debug("End of dump.");
  }
#endif

}