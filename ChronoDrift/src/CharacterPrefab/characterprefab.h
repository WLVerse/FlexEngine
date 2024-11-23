/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      characterprefab.h
\author    [80%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\co-author [20%] Kuan Yew Chong
\par       yewchong.k\@digipen.edu
\date      03 October 2024
\brief     This file contains the functions to load and save
           character to and from prefab file.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include <FlexEngine.h> // Utility functions
#include "Components/battlecomponents.h"
#include "Components/rendering.h"

using namespace ChronoDrift; // This is solution specific...

/*!
  \brief Spawns an entity with configurations from a prefab file.
  \param prefabName Name of the prefab file.

  Usage: Include Prefab.h in the source file where you want to spawn entities from prefab files.
                EntityID e = SpawnEntityFromPrefab("test") where test is the name of the prefab

                From there, you can use the entityID handle to manipulate the entity as you wish.

         Note: The prefab creation function is available from the Scene class and can be invoked as follows:
                FlexECS::Manager::GetInstance().GetActiveScene()->SaveEntityAsPrefab(object.Get() , "test");
*/
FlexECS::Entity& LoadCharacter(const std::string& prefabName);
/*
  \brief Resets the HP and SPD of all characters in active scene back to MAX HP and initial SPD
*/
void ResetCharacters();
void CreateCharacter(Character parameters);
void EditCharacter(std::string character_name, Character parameters);