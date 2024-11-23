/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      turnorderdisplay.cpp
\author    [100%] Ho Jin Jie Donovan, h.jinjiedonovan, 2301233
\par       h.jinjiedonovan\@digipen.edu
\date      03 October 2024
\brief     This file contains the definition of the function
					 that will display the turn order of both the players
					 and enemies on the left side of the screen

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "turnorderdisplay.h"
#include "Components/battlecomponents.h"
#include "Components/rendering.h"

namespace ChronoDrift 
{

	//The Sprite2d renderer will automatically draw it, so we just set the positions, colors, etc.
	void DisplayTurnOrder(std::vector<FlexECS::Entity>& queue)
	{
		auto scene = FlexECS::Manager::GetInstance().GetActiveScene();

		int i{ 0 };
		for (auto entity : scene->Query<TurnOrderDisplay, IsActive, ZIndex, Position, Scale, Shader, Sprite>())
		{
			if (i >= queue.size())
			{
				entity.GetComponent<IsActive>()->is_active = false;
				continue;
			}
			std::vector<FlexECS::Entity>::const_iterator it = queue.begin();
			std::advance(it, i);
			auto character = *it;

			entity.GetComponent<IsActive>()->is_active = true;
			entity.GetComponent<Position>()->position = { 50.f, 180.f + (80.f * i) };
			//entity.GetComponent<Sprite>()->color = character.GetComponent<Sprite>()->color;
			entity.GetComponent<Sprite>()->color_to_add = character.GetComponent<Sprite>()->color_to_add;
			entity.GetComponent<Sprite>()->color_to_multiply = character.GetComponent<Sprite>()->color_to_multiply;
			entity.GetComponent<Sprite>()->texture = character.GetComponent<Sprite>()->texture;
			++i;
		}
	}

}

