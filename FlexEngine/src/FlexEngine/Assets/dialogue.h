#pragma once

#include "flx_api.h"

#include "Utilities/file.h"
#include "assetkey.h"

namespace FlexEngine
{
  namespace Asset
  {
    // A single cutscene frame entry, which may have multiple dialogue blocks.
    struct DialogueEntry
    {
        std::string cutsceneName;
        // Each block is a vector of dialogue lines.
        std::vector<std::vector<std::string>> blocks;
    };

    struct __FLX_API Dialogue
    {
      File& metadata;

      // Vector to preserve the order of cutscene frames as they appear in the text file.
      std::vector<DialogueEntry> dialogues;

      Dialogue(File& _metadata);
    };

  } // namespace Asset
} // namespace FlexEngine
