#pragma once

#include "flx_api.h"

#include "Utilities/file.h"
#include "assetkey.h"

namespace FlexEngine
{
  namespace Asset
  {
      // Structure to store frame range and timing info for a cutscene.
      struct CutsceneInfo 
      {
          int frameStart;
          int frameEnd;
          float preTime;
          float duration;
          float postTime;
      };

      struct __FLX_API Cutscene 
      {
          File& metadata;

          // Unordered map for quick access: key = cutscene name, value = CutsceneInfo.
          std::unordered_map<std::string, CutsceneInfo> cutscenes;

          Cutscene(File& _metadata);
      };

  } // namespace Asset
} // namespace FlexEngine
