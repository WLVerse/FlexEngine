// WLVERSE [https://wlverse.web.app]
// layerstack.cpp
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

#include "pch.h"

#include "layerstack.h"

namespace FlexEngine
{

  LayerStack::LayerStack()
  {

  }

  LayerStack::~LayerStack()
  {
    Clear();
  }

  void LayerStack::Clear()
  {
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

    m_to_add.clear();
    m_to_remove.clear();
  }

  void LayerStack::Update()
  {
    for (auto& layer : m_layers) layer->Update();
    for (auto& overlay : m_overlays) overlay->Update();

    // execute deferred commands
    Internal_ExecuteDeferredCommands();
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
    FLX_FLOW_FUNCTION();

    // guard: check if layer is nullptr
    if (!layer)
    {
      Log::Error("Layer is nullptr.");
      return;
    }

    // guard: check if name already exists
    for (auto& l : m_layers)
    {
      if (l->GetName() == layer_name)
      {
        Log::Error("Layer name already exists.");
        return;
      }
    }

    // guard: check if index is out of bounds
#if defined(LAYERSTACK_ALLOW_OOB_INDEX)
    if (index > m_layers.size())
    {
      Log::Error("Index out of bounds.");
      return;
    }
#else
    if (index > m_layers.size())
    {
      index = m_layers.size();
    }
#endif

    m_to_add.insert({ LayerType::Layer, layer, index });
  }

  void LayerStack::AddOverlay(std::shared_ptr<Layer> overlay, size_t index, const std::string& overlay_name)
  {
    FLX_FLOW_FUNCTION();

    // guard: check if overlay is nullptr
    if (!overlay)
    {
      Log::Error("Overlay is nullptr.");
      return;
    }

    // guard: check if name already exists
    for (auto& o : m_overlays)
    {
      if (o->GetName() == overlay_name)
      {
        Log::Error("Overlay name already exists.");
        return;
      }
    }

    // guard: check if index is out of bounds
#if defined(LAYERSTACK_ALLOW_OOB_INDEX)
    if (index > m_overlays.size())
    {
      Log::Error("Index out of bounds.");
      return;
    }
#else
    if (index > m_overlays.size())
    {
      index = m_overlays.size();
    }
#endif

    m_to_add.insert({ LayerType::Overlay, overlay, index });
  }

  void LayerStack::RemoveLayer(const std::string& layer_name)
  {
    FLX_FLOW_FUNCTION();

    // guard: check if layer exists
    auto it = std::find_if(m_layers.begin(), m_layers.end(), [&](const auto& layer) { return layer->GetName() == layer_name; });
    if (it == m_layers.end())
    {
      Log::Error("Layer does not exist.");
      return;
    }

    m_to_remove.insert({ LayerType::Layer, std::distance(m_layers.begin(), it) });
  }

  void LayerStack::RemoveLayer(size_t index)
  {
    FLX_FLOW_FUNCTION();

    // guard: check if index is out of bounds
    if (index >= m_layers.size())
    {
      Log::Error("Index out of bounds.");
      return;
    }

    m_to_remove.insert({ LayerType::Layer, index });
  }

  void LayerStack::RemoveOverlay(const std::string& overlay_name)
  {
    FLX_FLOW_FUNCTION();

    // guard: check if overlay exists
    auto it = std::find_if(m_overlays.begin(), m_overlays.end(), [&](const auto& overlay) { return overlay->GetName() == overlay_name; });
    if (it == m_overlays.end())
    {
      Log::Error("Overlay does not exist.");
      return;
    }

    m_to_remove.insert({ LayerType::Overlay, std::distance(m_overlays.begin(), it) });
  }

  void LayerStack::RemoveOverlay(size_t index)
  {
    FLX_FLOW_FUNCTION();

    // guard: check if index is out of bounds
    if (index >= m_overlays.size())
    {
      Log::Error("Index out of bounds.");
      return;
    }

    m_to_remove.insert({ LayerType::Overlay, index });
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

  void LayerStack::Internal_ExecuteDeferredCommands()
  {
    // quick out if there's nothing to do
    if (m_to_add.empty() && m_to_remove.empty()) return;

    // apply deferred removals
    for (const auto& [type, index] : m_to_remove) Internal_Remove(type, index);
    m_to_remove.clear();

    // apply deferred additions
    for (const auto& [type, layer, index] : m_to_add) Internal_Add(type, layer, index);
    m_to_add.clear();
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
      Log::Error("Invalid layer type.");
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
      Log::Error("Invalid layer type.");
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