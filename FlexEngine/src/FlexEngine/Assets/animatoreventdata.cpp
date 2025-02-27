#include "pch.h"

#include "animatoreventdata.h"

namespace FlexEngine
{
  namespace Asset
  {
    AnimatorEventData::AnimatorEventData(File& _metadata)
      : metadata(_metadata)
    {
      // parse the file
      // example
      // TriggerFrame: 10
      // TargetEntity: 1
      // EventType: 1
      // Value: "AttackOne"
      std::stringstream ss(metadata.Read());
      std::string line;
      while (std::getline(ss, line))
      {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "TriggerFrame:")
          iss >> trigger_frame;
        else if (key == "TargetEntity:")
          iss >> target_entity;
        else if (key == "EventType:")
          iss >> event_type;
        else if (key == "Value:")
          iss >> value;
      }
    }
  } // namespace Asset
} // namespace FlexEngine
