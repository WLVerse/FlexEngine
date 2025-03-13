/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// cutscene.cpp
//
// Implements the Cutscene constructor defined in cutscene.h. This function
// reads metadata from the provided file and parses cutscene frame ranges and
// timing information. It supports lines formatted as:
// Cutscene_01_WakingUp, frames:00000-00023, time:(1.0f,0.6f,1.0f)
// The parser extracts the cutscene name, frame start/end, and timing values
// (preTime, duration, postTime), and stores them in an unordered_map for quick access.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "cutscene.h"

namespace FlexEngine
{
    namespace Asset
    {
        // Constructs a Cutscene instance by parsing the provided metadata file.
        Cutscene::Cutscene(File& _metadata)
            : metadata(_metadata)
        {
            // Read the entire metadata file into a stringstream for line-by-line processing.
            std::stringstream ss(metadata.Read());
            std::string line;

            // Process each line in the metadata file.
            while (std::getline(ss, line))
            {
                // Skip empty lines.
                if (line.empty())
                    continue;

                // Example line:
                // Cutscene_01_WakingUp, frames:00000-00023, time:(1.0f,0.6f,1.0f)

                // Extract the cutscene name (text before the first comma).
                size_t firstComma = line.find(",");
                if (firstComma == std::string::npos)
                    continue;
                std::string name = line.substr(0, firstComma);

                // Extract the frame range string.
                size_t framesPos = line.find("frames:");
                if (framesPos == std::string::npos)
                    continue;
                size_t commaAfterFrames = line.find(",", framesPos);
                std::string framesStr = (commaAfterFrames != std::string::npos) ?
                    line.substr(framesPos, commaAfterFrames - framesPos) :
                    line.substr(framesPos);
                // Remove the "frames:" prefix.
                const std::string framesPrefix = "frames:";
                std::string frameRange = framesStr.substr(framesPrefix.length());
                // Split the frame range (e.g., "00000-00023") into start and end strings.
                size_t dashPos = frameRange.find("-");
                if (dashPos == std::string::npos)
                    continue;
                std::string frameStartStr = frameRange.substr(0, dashPos);
                std::string frameEndStr = frameRange.substr(dashPos + 1);
                int frameStart = std::stoi(frameStartStr);
                int frameEnd = std::stoi(frameEndStr);

                // Extract the time information.
                size_t timePos = line.find("time:");
                if (timePos == std::string::npos)
                    continue;
                size_t openParenPos = line.find("(", timePos);
                size_t closeParenPos = line.find(")", timePos);
                if (openParenPos == std::string::npos || closeParenPos == std::string::npos)
                    continue;
                std::string timeStr = line.substr(openParenPos + 1, closeParenPos - openParenPos - 1);
                // timeStr should now be like "1.0f,0.6f,1.0f"
                std::istringstream timeStream(timeStr);
                std::string token;
                float preTime = 0, duration = 0, postTime = 0;
                int tokenIndex = 0;
                while (std::getline(timeStream, token, ','))
                {
                    // Remove any whitespace characters.
                    token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
                    // Remove trailing 'f' if present.
                    if (!token.empty() && token.back() == 'f')
                        token.pop_back();
                    float value = std::stof(token);
                    if (tokenIndex == 0)
                        preTime = value;
                    else if (tokenIndex == 1)
                        duration = value;
                    else if (tokenIndex == 2)
                        postTime = value;
                    tokenIndex++;
                }

                // Store the parsed data into a CutsceneInfo instance.
                CutsceneInfo info;
                info.frameStart = frameStart;
                info.frameEnd = frameEnd;
                info.preTime = preTime;
                info.duration = duration;
                info.postTime = postTime;

                // Insert the parsed data into the unordered_map using the cutscene name as the key.
                cutscenes[name] = info;
            }
        }
    } // namespace Asset
} // namespace FlexEngine
