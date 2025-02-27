#pragma once

#include "flx_api.h"

#include "FlexECS/datastructures.h"
#include "Utilities/file.h"
#include "assetkey.h"

namespace FlexEngine
{
  namespace Asset
  {

    // TODO: use the flxfmt formatter
    // TODO: save any changes to file
    struct __FLX_API AnimatorEventData
    {
      File& metadata;

      int trigger_frame = 0;
      FlexECS::EntityID target_entity;
      int event_type = 0; // animation = 1, sound = 2
      AssetKey value;     // animation name or sound file path

      AnimatorEventData(File& _metadata);
    };

  } // namespace Asset
} // namespace FlexEngine
