#include "pch.h"

#include "move.h"

namespace FlexEngine
{
  namespace Asset
  {
    Move::Move(File& _metadata)
      : metadata(_metadata)
    {
      // parse the file
      // example
      // Name: "AttackOne"
      // Damage: 10
      // Speed: 5
      std::stringstream ss(metadata.Read());
      std::string line;
      while (std::getline(ss, line))
      {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "Name:")
          iss >> name;
        else if (key == "Damage:")
          iss >> damage;
        else if (key == "Speed:")
          iss >> speed;
        else if (key == "Description:")
        {
          std::getline(iss >> std::ws, description); // Read the rest of the line, trimming leading spaces
        }
      }
    }
  } // namespace Asset
} // namespace FlexEngine
