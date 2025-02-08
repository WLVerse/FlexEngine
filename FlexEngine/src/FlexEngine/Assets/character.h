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
    struct __FLX_API Character
    {
      File& metadata;

      std::string character_name = "None";
      int character_id = 0; // 1-5
      int health = 100;
      int speed = 10;
      AssetKey move_one = "";
      AssetKey move_two = "";
      AssetKey move_three = "";

      Character(File& _metadata);
    };

  } // namespace Asset
} // namespace FlexEngine
