#include "pch.h"

#include "dialogue.h"

namespace FlexEngine
{
    namespace Asset
    {
        Dialogue::Dialogue(File& _metadata)
            : metadata(_metadata)
        {
            std::stringstream ss(metadata.Read());
            std::string line;

            DialogueEntry* currentEntry = nullptr;   // Pointer to the current dialogue entry.
            std::vector<std::string> currentBlock;     // Accumulates dialogue lines for the current block.

            while (std::getline(ss, line))
            {
                // Trim trailing carriage return or newline characters.
                while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) 
                {
                    line.pop_back();
                }

                // Empty line indicates the end of the current dialogue block.
                if (line.empty()) 
                {
                    if (currentEntry && !currentBlock.empty()) {
                        currentEntry->blocks.push_back(currentBlock);
                        currentBlock.clear();
                    }
                    continue;
                }

                // Check if the line is a cutscene frame header (e.g. [Cutscene_01])
                if (line.front() == '[' && line.back() == ']') 
                {
                    // If we already have a current entry, save any pending dialogue block.
                    if (currentEntry && !currentBlock.empty()) {
                        currentEntry->blocks.push_back(currentBlock);
                        currentBlock.clear();
                    }
                    // Create a new DialogueEntry and add it to the vector.
                    DialogueEntry entry;
                    entry.cutsceneName = line.substr(1, line.size() - 2); // Strip off the square brackets.
                    dialogues.push_back(entry);
                    // Set the current entry pointer to the newly added entry.
                    currentEntry = &dialogues.back();
                }
                else 
                {
                    // Otherwise, add the line to the current dialogue block.
                    currentBlock.push_back(line);
                }
            }

            // After processing all lines, add any remaining dialogue block.
            if (currentEntry && !currentBlock.empty()) 
            {
                currentEntry->blocks.push_back(currentBlock);
            }
        }
    } // namespace Asset
} // namespace FlexEngine
