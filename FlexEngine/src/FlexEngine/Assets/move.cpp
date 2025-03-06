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
        else if (key == "Speed:")
            iss >> speed;
        else if (key == "Effect:")
        {
            std::getline(iss >> std::ws, key);
            iss >> key;
            std::string tmp;
            for (unsigned i = 0; i < key.length(); i++)//go through each character of the given string
                if (isalpha(key[i]) || key[i] == '_')// if the character is alphabetical ( isalpha is from <cctype> header )
                    tmp += key[i];// add the character to the temporary string
            effect.push_back(tmp);
            Log::Debug(key);
        }
        else if (key == "Value:")
        {
            int tmp = 0;
            iss >> tmp;
            value.push_back(tmp);
            //value = tmp;
            Log::Debug(std::to_string(tmp));
        }
        else if (key == "Target:")
        {
            std::getline(iss >> std::ws, key);
            iss >> key;
            std::string tmp;
            for (unsigned i = 0; i < key.length(); i++)//go through each character of the given string
                if (isalpha(key[i]) || key[i] == '_')// if the character is alphabetical ( isalpha is from <cctype> header )
                    tmp += key[i];// add the character to the temporary string
            target.push_back(tmp);
            Log::Debug(key);
        }
        else if (key == "Description:")
        {
          std::getline(iss >> std::ws, description); // Read the rest of the line, trimming leading spaces
        }
      }
    }
  } // namespace Asset
} // namespace FlexEngine
