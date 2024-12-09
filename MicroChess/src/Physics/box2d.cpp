#include "Physics/box2d.h"

#include "Components/game.h"
#include "Components/physics.h"
#include "Components/rendering.h"

namespace MicroChess
{

  // update PieceStatus based on mouse input
  void Box2D()
  {
    Vector2 mouse_position = Input::GetMousePosition();
    bool mouse_clicked = Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);

    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, Parent, Position, Scale, BoundingBox2D, OnHover>())
    {
      if (!entity.GetComponent<IsActive>()->is_active) continue;

      auto& parent = entity.GetComponent<Parent>()->parent;
      auto global_position = parent.HasComponent<Position>() ? parent.GetComponent<Position>()->position : Vector2::Zero;
      auto global_scale = parent.HasComponent<Scale>() ? parent.GetComponent<Scale>()->scale : Vector2::One;
      
      auto& position = entity.GetComponent<Position>()->position;
      auto& scale = entity.GetComponent<Scale>()->scale;

      global_position += position;
      global_scale *= scale;

      auto boundingbox = entity.GetComponent<BoundingBox2D>()->size;
      boundingbox *= global_scale;

      auto hover_status = entity.GetComponent<OnHover>();

      // offset bounding box to be centered
      global_position -= boundingbox * 0.5f;

      // cleanup before detection
      hover_status->on_enter = false;
      hover_status->on_exit = false;

      // collision detection
      if (
        mouse_position.x > global_position.x && mouse_position.x < global_position.x + boundingbox.x &&
        mouse_position.y > global_position.y && mouse_position.y < global_position.y + boundingbox.y
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

    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, Parent, Position, Scale, BoundingBox2D, OnClick>())
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

      auto& parent = entity.GetComponent<Parent>()->parent;
      auto global_position = parent.HasComponent<Position>() ? parent.GetComponent<Position>()->position : Vector2::Zero;
      auto global_scale = parent.HasComponent<Scale>() ? parent.GetComponent<Scale>()->scale : Vector2::One;

      auto& position = entity.GetComponent<Position>()->position;
      auto& scale = entity.GetComponent<Scale>()->scale;

      global_position += position;
      global_scale *= scale;

      auto boundingbox = entity.GetComponent<BoundingBox2D>()->size;
      boundingbox *= global_scale;

      // offset bounding box to be centered
      global_position -= boundingbox * 0.5f;

      // collision detection
      click_status = (
        mouse_position.x > global_position.x && mouse_position.x < global_position.x + boundingbox.x &&
        mouse_position.y > global_position.y && mouse_position.y < global_position.y + boundingbox.y
      );
    }

  }

}