/* Start Header
*****************************************************************/
/*!
WLVERSE [https://wlverse.web.app]
\file      targetingsystem.cpp
\author    [100%] Rocky Sutarius
\par       rocky.sutarius@digipen.edu
\date      03 October 2024
\brief     This file contains the functions to get move and
           target selection via mouse click

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include <FlexEngine.h>

#include "targetingsystem.h"
#include "Components/battlecomponents.h"
#include "Components/physics.h"
#include "Components/rendering.h"


namespace ChronoDrift 
{

	void GetTargetSelection()
	{
		Vector2 mouse_position = Input::GetMousePosition();
		bool mouse_clicked = Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);

		for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<IsActive, Position, Scale, OnHover, BattleSlot>())
		{
			if (!entity.GetComponent<IsActive>()->is_active) continue;

			auto position = entity.GetComponent<Position>()->position;
			auto scale = entity.GetComponent<Scale>()->scale;
			auto hover_status = entity.GetComponent<OnHover>();

			position -= scale * 0.5f;

			hover_status->on_enter = false;
			hover_status->on_exit = false;
      if (
        mouse_position.x > position.x && mouse_position.x < position.x + scale.x &&
        mouse_position.y > position.y && mouse_position.y < position.y + scale.y
        )
      {
        if (!hover_status->is_hovering)
        {
          hover_status->is_hovering = true;
          hover_status->on_enter = true;
        }
      }
      else
      {
        if (hover_status->is_hovering)
        {
          hover_status->is_hovering = false;
          hover_status->on_exit = true;
        }
      }
		}
	
  
    for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<IsActive, Position, Scale, OnHover, OnClick, BattleSlot>())
    {
      if (!entity.GetComponent<IsActive>()->is_active) continue;

      auto& click_status = entity.GetComponent<OnClick>()->is_clicked;

      // guard
      // skip detection if mouse is not clicked
      if (!mouse_clicked)
      {
        click_status = false;
        continue;
      }

      auto& hover_status = entity.GetComponent<OnHover>()->is_hovering;

      // collision detection
      click_status = hover_status;
    }
  }

  void GetMoveSelection()
  {

    Vector2 mouse_position = Input::GetMousePosition();
    bool mouse_clicked = Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);

    for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<IsActive, Position, Scale, OnHover, MoveButton>())
    {
      if (!entity.GetComponent<IsActive>()->is_active) continue;

      auto position = entity.GetComponent<Position>()->position;
      auto scale = entity.GetComponent<Scale>()->scale;
      auto hover_status = entity.GetComponent<OnHover>();

      position -= scale * 0.5f;

      hover_status->on_enter = false;
      hover_status->on_exit = false;
      if (
        mouse_position.x > position.x && mouse_position.x < position.x + scale.x &&
        mouse_position.y > position.y && mouse_position.y < position.y + scale.y
        )
      {
        if (!hover_status->is_hovering)
        {
          hover_status->is_hovering = true;
          hover_status->on_enter = true;
        }
      }
      else
      {
        if (hover_status->is_hovering)
        {
          hover_status->is_hovering = false;
          hover_status->on_exit = true;
        }
      }
    }


    for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<IsActive, Position, Scale, OnHover, OnClick, MoveButton>())
    {
      if (!entity.GetComponent<IsActive>()->is_active) continue;

      auto& click_status = entity.GetComponent<OnClick>()->is_clicked;

      // guard
      // skip detection if mouse is not clicked
      if (!mouse_clicked)
      {
        click_status = false;
        continue;
      }

      auto& hover_status = entity.GetComponent<OnHover>()->is_hovering;

      // collision detection
      click_status = hover_status;
    }
  }

}


