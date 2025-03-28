/////////////////////////////////////////////////////////////////////////////
// WLVERSE [https://wlverse.web.app]
// character.cpp
//
// Character definition. This implements character logic
// 
// AUTHORS
// [100%] Ng Jia Cheng (n.jiacheng\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
/////////////////////////////////////////////////////////////////////////////
#include "pch.h"

#include "character.h"

namespace FlexEngine
{

  namespace Asset
  {

    Character::Character(File& _metadata)
      : metadata(_metadata)
    {
      // parse the file
      // example
      // CharacterName: "Renko"
      // CharacterID: 1
      // Health: 100
      // Speed: 10
      // MoveOne: "/data/MoveOne.flxmove"
      // MoveTwo: "/data/MoveTwo.flxmove"
      // MoveThree: "/data/MoveThree.flxmove"
      std::stringstream ss(metadata.Read());
      std::string line;
      while (std::getline(ss, line))
      {
        std::istringstream iss(line);
        std::string key;
        iss >> key;
        if (key == "CharacterName:")
          iss >> character_name;
        else if (key == "CharacterID:")
          iss >> character_id;
        else if (key == "Health:")
          iss >> health;
        else if (key == "Speed:")
          iss >> speed;
        else if (key == "MoveOne:")
          iss >> move_one;
        else if (key == "MoveTwo:")
          iss >> move_two;
        else if (key == "MoveThree:")
          iss >> move_three;
      }
    }

  }

} // namespace FlexEngine
