/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      turnorderdisplay.ch
\author    [100%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file contains the declaration of the function
					 that will display the sprites of the characters
					 that are in the current battle scene.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#pragma once
#include "FlexEngine.h"

using namespace FlexEngine;

namespace ChronoDrift 
{
	/*
		\brief Displays the character's sprites according to queue order
	*/
	void DisplayTurnOrder(std::vector<FlexECS::Entity>& queue);
}