/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// dialogue.cpp
//
// Implements the Dialogue constructor defined in dialogue.h.
// This constructor reads dialogue metadata from a file and parses it into
// DialogueEntry instances. It identifies cutscene frame headers (e.g. [Cutscene_01])
// and aggregates subsequent dialogue lines into blocks, preserving the order
// of cutscene frames as they appear in the file.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "dialogue.h"

namespace FlexEngine
{
    namespace Asset
    {
        // Constructs a Dialogue object by reading and parsing the provided metadata file.
        Dialogue::Dialogue(File& _metadata)
            : metadata(_metadata)
        {
            // Read the entire content of the file into a stringstream for line-by-line processing.
            std::stringstream ss(metadata.Read());
            std::string line;

            DialogueEntry* currentEntry = nullptr;   // Pointer to the current dialogue entry.
            std::vector<std::string> currentBlock;     // Accumulates dialogue lines for the current block.

            // Process each line from the file.
            while (std::getline(ss, line))
            {
                // Trim trailing carriage return and newline characters.
                while (!line.empty() && (line.back() == '\r' || line.back() == '\n'))
                {
                    line.pop_back();
                }

                // An empty line indicates the end of the current dialogue block.
                if (line.empty())
                {
                    if (currentEntry && !currentBlock.empty())
                    {
                        // Save the completed dialogue block to the current entry.
                        currentEntry->blocks.push_back(currentBlock);
                        currentBlock.clear();
                    }
                    continue;
                }

                // Check if the line is a cutscene frame header (e.g., [Cutscene_01]).
                if (line.front() == '[' && line.back() == ']')
                {
                    // If there is an active dialogue entry with a pending block, add it first.
                    if (currentEntry && !currentBlock.empty())
                    {
                        currentEntry->blocks.push_back(currentBlock);
                        currentBlock.clear();
                    }
                    // Create a new DialogueEntry and add it to the dialogues vector.
                    DialogueEntry entry;
                    // Extract the cutscene name by removing the surrounding brackets.
                    entry.cutsceneName = line.substr(1, line.size() - 2);
                    dialogues.push_back(entry);
                    // Update the current entry pointer to the newly added DialogueEntry.
                    currentEntry = &dialogues.back();
                }
                else
                {
                    // Otherwise, treat the line as part of the current dialogue block.
                    currentBlock.push_back(line);
                }
            }

            // After processing all lines, add any remaining dialogue block to the current entry.
            if (currentEntry && !currentBlock.empty())
            {
                currentEntry->blocks.push_back(currentBlock);
            }
        }
    } // namespace Asset
} // namespace FlexEngine
