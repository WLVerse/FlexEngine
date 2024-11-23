/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      base.h
\author    [100%] Rocky Sutarius
\par       rocky.sutarius@digipen.edu
\date      03 October 2024
\brief     This file is the header file for base.cpp and contains
           the declarations of the functions defined in base.cpp

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace ChronoDrift
{

  class BaseLayer : public FlexEngine::Layer
  {
    const Path default_save_directory = Path::current("saves");
    const std::string default_save_name = "default";
    Path current_save_directory = default_save_directory;
    std::string current_save_name = default_save_name;

  public:
    BaseLayer() : Layer("Base Layer") {}
    ~BaseLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void Update() override;
  };

}
