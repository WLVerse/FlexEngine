#pragma once

#include "flx_api.h"

#include "Utilities/file.h"
#include "assetkey.h"

namespace FlexEngine
{
  namespace Asset
  {

    // TODO: use the flxfmt formatter
    // TODO: save any changes to file
    struct __FLX_API Move
    {
      File& metadata;

      std::string name = "None";
      int damage = 0;
      int speed = 0;
      std::string description = "";

      Move(File& _metadata);
    };

  } // namespace Asset
} // namespace FlexEngine
