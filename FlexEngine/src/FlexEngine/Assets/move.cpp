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
            //effect = key;
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
            //target = key;
            Log::Debug(key);
        }
        //change to alphabet encryption if doesn't work.
        /*else if (key == "Effect:")
        {
            iss >> key;
            effects *= 10;
            if (key == "Heal")
            {
                effects += 1;
            }
            else if (key == "SPD buff")
            {
                effects += 2;
            }
            else if (key == "SPD debuff")
            {
                effects += 3;
            }
            else if (key == "ATK buff")
            {
                effects += 4;
            }
            else if (key == "ATK buff")
            {
                effects += 5;
            }
            else if (key == "Invuln buff")
            {
                effects += 6;
            }
            else if (key == "Stun debuff")
            {
                effects += 7;
            }
            else if (key == "Remove buff")
            {
                effects += 8;
            }
            else if (key == "Remove debuff")
            {
                effects += 9;
            }
        }
        else if (key == "Value:")
        {
           
        }
        else if (key == "Target:")
        {
            iss >> key;
            targets *= 10;
            if (key == "ST ally")
            {
                targets += 1;
            }
            else if (key == "ADJ ally")
            {
                targets += 2;
            }
            else if (key == "AOE ally")
            {
                targets += 3;
            }
            else if (key == "NEXT ally")
            {
                targets += 4;
            }
            else if (key == "ST enemy")
            {
                targets += 5;
            }
            else if (key == "ADJ enemy")
            {
                targets += 6;
            }
            else if (key == "AOE enemy")
            {
                targets += 7;
            }
            else if (key == "NEXT enemy")
            {
                targets += 8;
            }
            else //default targets self
            {
                targets += 9;
            }
        }*/
        else if (key == "Description:")
        {
          std::getline(iss >> std::ws, description); // Read the rest of the line, trimming leading spaces
        }
      }
    }
  } // namespace Asset
} // namespace FlexEngine
