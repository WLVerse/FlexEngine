// WLVERSE [https://wlverse.web.app]
// EngineComponents.h
// 
// All components that the base engine supports
//
// AUTHORS
// [100%] Yew Chong (yewchong.k\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.
#pragma once

#include "Reflection/base.h"
#include "FlexECS/datastructures.h"
#include "flx_api.h"
#include "FlexMath/vector3.h"
#include "FlexMath/matrix4x4.h"

namespace FlexEngine
{
  using EntityName = FlexECS::Scene::StringIndex;

  /*!***************************************************************************
  * \class Position
  * \brief
  * This class represents the position of an entity in 2D space. It defines
  * where the entity is located in the scene using a 2D vector.
  ******************************************************************************/
  class __FLX_API Position
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector3 position = Vector3::Zero;
  };

  /*!***************************************************************************
  * \class Scale
  * \brief
  * This class represents the scale of an entity in 2D space, determining
  * how large or small the entity should be. The scale is defined using
  * a 2D vector.
  ******************************************************************************/
  class __FLX_API Scale
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector3 scale = Vector3::One;
  };

  /*!***************************************************************************
  * \class Rotation
  * \brief
  * This class represents the rotation of an entity in 3D space. It uses
  * a 3D vector to define rotations along the x, y, and z axes.
  ******************************************************************************/
  class __FLX_API Rotation
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector3 rotation = Vector3::Zero;
  };
  
  /*!***************************************************************************
  * \class Transform
  * \brief
  * Transformation holds the final transformation matrix of an entity and decides whether it is active or not.
  * This component should only ever exist with Position, Scale, and Rotation components.
  ******************************************************************************/
  class __FLX_API Transform
  {
    FLX_REFL_SERIALIZABLE
  public:
    bool is_dirty = true; //Determines if transform needs to be updated
    Matrix4x4 transform = Matrix4x4::Identity;
    bool is_active = true;
  };

  /*!***************************************************************************
  * \class ZIndex
  * \brief
  * This class represents the z-order (depth) of an entity, determining its
  * rendering order relative to other entities. Higher `z` values are drawn
  * on top of lower ones.
  ******************************************************************************/
  class __FLX_API ZIndex
  {
    FLX_REFL_SERIALIZABLE
  public:
    int z;
  };

  /*!***************************************************************************
  * \class Parent
  * \brief
  * This class represents the parent entity of an entity, enabling hierarchical
  * transformations where a child entity inherits its parent�s transformation
  * matrix. It supports complex scene graph structures.
  ******************************************************************************/
  class __FLX_API Parent
  {
    FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Entity parent = FlexECS::Entity::Null;
  };

  /*!***************************************************************************
  * \class Audio
  * \brief 
  * This class represents the audio component of an entity
  ******************************************************************************/
  class __FLX_API Audio
  {
    FLX_REFL_SERIALIZABLE
  public:
    bool should_play;
    bool should_stop;
    bool is_looping;
    FlexECS::Scene::StringIndex audio_file;
  };

  /*!***************************************************************************
  * \class Prefab
  * \brief
  * This class represents the prefab component of an entity
  ******************************************************************************/
  class __FLX_API Prefab
  {
    FLX_REFL_SERIALIZABLE
  public:
    std::string prefab_name;
  };
  
  class __FLX_API Sprite
  {
    FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex sprite_handle;
    int handle = -1; // Indicates it is not a spritesheet by default
  };

  class __FLX_API Animator
  {
    FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex spritesheet_file;
    bool should_play = true;
    float time = 0.f;
  };

  // THESE ARE STUBS, DO NOT USE OTHERWISE IMPLEMENT FIRST!!!!
  class __FLX_API Text
  {
    FLX_REFL_SERIALIZABLE
  public:
  };


  /**************
  * Physics
  **************/

  class __FLX_API BoundingBox2D
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector2 size = Vector2::One;
    Vector2 min;
    Vector2 max;
    bool is_colliding;
  };

  class __FLX_API AABB
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector2 min;
    Vector2 max;
  };

  class __FLX_API Rigidbody
  {
    FLX_REFL_SERIALIZABLE
  public:
    Vector2 velocity;
    bool is_static;
  };

}