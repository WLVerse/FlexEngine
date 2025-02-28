#include "pch.h"

#include "cutscene.h"

namespace FlexEngine
{
    namespace Asset
    {
        Cutscene::Cutscene(File& _metadata)
            : metadata(_metadata)
        {
            std::stringstream ss(metadata.Read());
            std::string line;

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
                // Split the frame range (e.g. "00000-00023") into start and end.
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
                    // Remove any spaces.
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

                // Insert into the unordered map using the cutscene name as key.
                cutscenes[name] = info;
            }
        } // namespace Asset
    } // namespace FlexEngine
}
