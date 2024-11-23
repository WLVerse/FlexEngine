#include "Physics/box2d.h"

#include "Components/game.h"
#include "Components/physics.h"
#include "Components/rendering.h"

namespace ChronoDrift
{

  // update PieceStatus based on mouse input
  void Box2D()
  {
    Vector2 mouse_position = Input::GetMousePosition();
    //bool mouse_clicked = Input::GetMouseButtonDown(GLFW_MOUSE_BUTTON_LEFT);

    //On Hover
    //for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->Query<IsActive, Parent, Position, Scale, Rotation, Transform, BoundingBox2D, OnHover>())
    //{
    //  if (!entity.GetComponent<IsActive>()->is_active) continue;

    //  auto& local_transform = entity.GetComponent<Transform>()->transform;
    //  if (!entity.GetComponent<Transform>()->is_dirty) continue;

    //  auto& parent = entity.GetComponent<Parent>()->parent;
    //  auto parent_transform = parent.GetComponent<Transform>();

    //  auto& local_position = entity.GetComponent<Position>()->position;
    //  auto& local_scale = entity.GetComponent<Scale>()->scale;
    //  auto& local_rotation = entity.GetComponent<Rotation>()->rotation;

    //  // calculate the transform

    //  Matrix4x4 local_translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, Vector3(-local_position.x, local_position.y, 0.0f));
    //  Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(local_rotation).ToRotationMatrix();
    //  Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, local_scale);

    //  local_transform = parent_transform->transform * local_translation_matrix * rotation_matrix * scale_matrix;
    //  //parent_transform->transform.Dump();
    //  //{
    //  //    1.0f, 0.0f, 0.0f, 0.0f,
    //  //        1.0f, 0.0f, 0.0f, 0.0f,
    //  //        1.0f, 0.0f, 0.0f, 0.0f,
    //  //        1.0f, 0.0f, 0.0f, 0.0f
    //  //}
    //  //TODO: Will have to change this to move outside
    //  //auto boundingbox = entity.GetComponent<BoundingBox2D>()->size;
    //  //boundingbox *= global_scale;

    //  //auto hover_status = entity.GetComponent<OnHover>();

    //  //// offset bounding box to be centered
    //  //global_position -= boundingbox * 0.5f;

    //  //// cleanup before detection
    //  //hover_status->on_enter = false;
    //  //hover_status->on_exit = false;

    //  //// collision detection
    //  //if (
    //  //  mouse_position.x > global_position.x && mouse_position.x < global_position.x + boundingbox.x &&
    //  //  mouse_position.y > global_position.y && mouse_position.y < global_position.y + boundingbox.y
    //  //)
    //  //{
    //  //  if (!hover_status->is_hovering)
    //  //  {
    //  //    hover_status->is_hovering = true;
    //  //    hover_status->on_enter = true;
    //  //  }
    //  //}
    //  //else
    //  //{
    //  //  if (hover_status->is_hovering)
    //  //  {
    //  //    hover_status->is_hovering = false;
    //  //    hover_status->on_exit = true;
    //  //  }
    //  //}
    //}
    //On Click
    //for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->Query<IsActive, Parent, Position, Scale, Rotation, Transform, BoundingBox2D, OnClick>())
    //{
    //  if (!entity.GetComponent<IsActive>()->is_active) continue;

    //  auto& click_status = entity.GetComponent<OnClick>()->is_clicked;

    //  // guard
    //  // skip detection if mouse is not clicked
    //  if (!mouse_clicked)
    //  {
    //    click_status = false;
    //    continue;
    //  }

    //  auto& parent = entity.GetComponent<Parent>()->parent;
    //  auto global_position = parent.HasComponent<Position>() ? parent.GetComponent<Position>()->position : Vector2::Zero;
    //  auto global_scale = parent.HasComponent<Scale>() ? parent.GetComponent<Scale>()->scale : Vector2::One;
    //  auto global_rotation = parent.HasComponent<Rotation>() ? parent.GetComponent<Rotation>()->rotation : Vector3::Zero;

    //  auto& position = entity.GetComponent<Position>()->position;
    //  auto& scale = entity.GetComponent<Scale>()->scale;
    //  auto& rotation = entity.GetComponent<Rotation>()->rotation;

    //  global_position += position;
    //  global_scale *= scale;
    //  global_rotation += rotation;

    //  auto boundingbox = entity.GetComponent<BoundingBox2D>()->size;
    //  boundingbox *= global_scale;

    //  // offset bounding box to be centered
    //  global_position -= boundingbox * 0.5f;

    //  // collision detection
    //  click_status = (
    //    mouse_position.x > global_position.x && mouse_position.x < global_position.x + boundingbox.x &&
    //    mouse_position.y > global_position.y && mouse_position.y < global_position.y + boundingbox.y
    //  );
    //}
    
    //TODO -> RUN THROUGH FOR LOOP FOR CHILDREN
    //No parent
    //for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->Query<IsActive, Transform>())
    //{
    //    Parent* test = nullptr;
    //    if (!entity.GetComponent<IsActive>()->is_active /*|| entity.TryGetComponent<Parent>(test)*/) continue;
    //
    //    auto& local_transform = entity.GetComponent<Transform>()->transform;
    //    if (!entity.GetComponent<Transform>()->is_dirty) continue;
    //
    //    auto& local_position = entity.GetComponent<Position>()->position;
    //    auto& local_scale = entity.GetComponent<Scale>()->scale;
    //    auto& local_rotation = entity.GetComponent<Rotation>()->rotation;
    //
    //    // calculate the transform
    //
    //    Matrix4x4 local_translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, Vector3(-local_position.x, local_position.y, 0.0f));
    //    Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(local_rotation).ToRotationMatrix();
    //    Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, local_scale);
    //
    //    local_transform = local_translation_matrix * rotation_matrix * scale_matrix;
    //    
    //    Log::Debug(FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()));
    //}
    //// With parent
    //for (auto& entity : FlexECS::ecs_manager.GetActiveScene()->Query<IsActive, Transform, Parent>())
    //{
    //    if (!entity.GetComponent<IsActive>()->is_active) continue;

    //    auto& local_transform = entity.GetComponent<Transform>()->transform;
    //    if (!entity.GetComponent<Transform>()->is_dirty) continue;

    //    Matrix4x4 parent_transform = entity.GetComponent<Parent>()->parent.GetComponent<Transform>()->transform;

    //    auto& local_position = entity.GetComponent<Position>()->position;
    //    auto& local_scale = entity.GetComponent<Scale>()->scale;
    //    auto& local_rotation = entity.GetComponent<Rotation>()->rotation;

    //    // calculate the transform

    //    Matrix4x4 local_translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, Vector3(-local_position.x, local_position.y, 0.0f));
    //    Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(local_rotation).ToRotationMatrix();
    //    Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, local_scale);

    //    local_transform = parent_transform * local_translation_matrix * rotation_matrix * scale_matrix;

    //    Log::Debug(FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()));
    //}
  }

}