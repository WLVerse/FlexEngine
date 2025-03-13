/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// dialogue.h
//
// Provides data structures for cutscene dialogue assets. This header defines
// the DialogueEntry and Dialogue structures used to store and preserve the
// order of cutscene dialogue frames and their associated dialogue blocks.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "flx_api.h"

#include "Utilities/file.h"
#include "assetkey.h"

namespace FlexEngine
{
    namespace Asset
    {
        /**
         * @brief Represents a single cutscene frame entry.
         *
         * Each DialogueEntry contains a cutscene name and a collection of dialogue blocks,
         * where each block is represented as a vector of dialogue lines.
         */
        struct DialogueEntry
        {
            std::string cutsceneName;  ///< Name of the cutscene frame.

            // Each block is a vector of dialogue lines.
            std::vector<std::vector<std::string>> blocks;
        };

        /**
         * @brief Encapsulates dialogue metadata and entries for cutscenes.
         *
         * The Dialogue structure holds a reference to a File that contains metadata about
         * the dialogue and a vector of DialogueEntry instances preserving the order of cutscene
         * frames as they appear in the source file.
         */
        struct __FLX_API Dialogue
        {
            File& metadata;  ///< Reference to the file containing dialogue metadata.

            // Vector preserving the order of cutscene frames from the text file.
            std::vector<DialogueEntry> dialogues;

            /**
             * @brief Constructs a Dialogue instance with the provided metadata.
             *
             * @param _metadata Reference to the File containing dialogue metadata.
             */
            Dialogue(File& _metadata);
        };

    } // namespace Asset
} // namespace FlexEngine
