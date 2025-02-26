#pragma once

#include "flx_api.h"

#include "Utilities/file.h"
#include "assetkey.h"

namespace FlexEngine
{
  namespace Asset
  {
    struct __FLX_API Dialogue
    {
      File& metadata;

      std::string name = "None";
      std::vector<std::string> text;

      Dialogue(File& _metadata);
    };

  } // namespace Asset
} // namespace FlexEngine
