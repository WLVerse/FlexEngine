// WLVERSE [https://wlverse.web.app]
// layerstack.h
// 
// 
// 
// AUTHORS
// [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include "layer.h" // <string>

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

    void Clear();

    void PushLayer(std::shared_ptr<Layer> layer);
    void PopLayer();

    void PushOverlay(std::shared_ptr<Layer> overlay);
    void PopOverlay();

    void InsertLayer(std::shared_ptr<Layer> layer, size_t index);
    void InsertOverlay(std::shared_ptr<Layer> overlay, size_t index);

    void RemoveLayer(size_t index);
    void RemoveOverlay(size_t index);

    void Update();

#ifdef _DEBUG
    void Dump();
#endif
  };

}