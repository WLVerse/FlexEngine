// WLVERSE [https://wlverse.web.app]
// layerstack.cpp
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

#include "pch.h"

#include "layerstack.h"

namespace FlexEngine
{

  LayerStack::LayerStack()
  {
    //FLX_FLOW_BEGINSCOPE();
  }

  LayerStack::~LayerStack()
  {
    Clear();
    //FLX_FLOW_ENDSCOPE();
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
  }

  void LayerStack::PushLayer(std::shared_ptr<Layer> layer)
  {
    FLX_FLOW_FUNCTION();

    m_layers.push_back(layer);
    m_layers.back()->OnAttach();
  }

  void LayerStack::PushOverlay(std::shared_ptr<Layer> overlay)
  {
    FLX_FLOW_FUNCTION();

    m_overlays.push_back(overlay);
    m_overlays.back()->OnAttach();
  }

  void LayerStack::PopLayer()
  {
    FLX_FLOW_FUNCTION();

    // guard
    if (m_layers.size() == 1)
    {
      Log::Error("Cannot pop last layer");
      return;
    }

    m_layers.back()->OnDetach();
    m_layers.pop_back();
  }

  void LayerStack::PopOverlay()
  {
    FLX_FLOW_FUNCTION();

    // guard
    if (m_overlays.size() == 0)
    {
      Log::Error("No more overlays to pop.");
      return;
    }

    m_overlays.back()->OnDetach();
    m_overlays.pop_back();
  }

  void LayerStack::InsertLayer(std::shared_ptr<Layer> layer, size_t index)
  {
    FLX_FLOW_FUNCTION();

    // guard
    if (index > m_layers.size())
    {
      Log::Error("Index out of bounds.");
      return;
    }

    m_layers.insert(m_layers.begin() + index, layer);
    m_layers[index]->OnAttach();
  }

  void LayerStack::InsertOverlay(std::shared_ptr<Layer> overlay, size_t index)
  {
    FLX_FLOW_FUNCTION();

    // guard
    if (index > m_overlays.size())
    {
      Log::Error("Index out of bounds.");
      return;
    }

    m_overlays.insert(m_overlays.begin() + index, overlay);
    m_overlays[index]->OnAttach();
  }

  void LayerStack::RemoveLayer(size_t index)
  {
    FLX_FLOW_FUNCTION();

    // guard
    if (index >= m_layers.size())
    {
      Log::Error("Index out of bounds.");
      return;
    }

    m_layers[index]->OnDetach();
    m_layers.erase(m_layers.begin() + index);
  }

  void LayerStack::RemoveOverlay(size_t index)
  {
    FLX_FLOW_FUNCTION();

    // guard
    if (index >= m_overlays.size())
    {
      Log::Error("Index out of bounds.");
      return;
    }

    m_overlays[index]->OnDetach();
    m_overlays.erase(m_overlays.begin() + index);
  }

  void LayerStack::Update()
  {
    for (auto& layer : m_layers) layer->Update();
    for (auto& overlay : m_overlays) overlay->Update();
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