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

      std::vector<std::string> effect;
      std::vector<int> value;
      std::vector<std::string> target;
     
      //std::string effect = "";
      //int value = 0;
      //std::string target = "";
      
     // std::vector<std::tuple<std::string, std::string, int>> effect;

      /*int effects = 0;
      int values = 0;
      int targets = 0;*/

      std::string description = "";

      Move(File& _metadata);
    };

  } // namespace Asset
} // namespace FlexEngine
