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

#include "../Renderer/Camera/camera.h" // External declaration of the reflected Camera component
#include "FlexECS/datastructures.h"
#include "FlexMath/matrix4x4.h"
#include "FlexMath/vector3.h"
#include "Reflection/base.h"
#include "flx_api.h"

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
    FlexECS::Scene::StringIndex audio_file = FLX_STRING_NEW("");
    bool should_play = true;
    bool should_stop = false;
    bool is_looping = false;
    bool change_mode = false; // For tagging to flip
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
    Vector2 scale = Vector2(100.0f, 100.0f);
    bool center_aligned = false;
    float opacity = 1.0f;
    Matrix4x4 model_matrix = Matrix4x4::Identity;
  };

  class __FLX_API Animator
  {
    FLX_REFL_SERIALIZABLE

  public:
    FlexECS::Scene::StringIndex spritesheet_handle = FLX_STRING_NEW("");
    FlexECS::Scene::StringIndex default_spritesheet_handle = FLX_STRING_NEW("");
    bool should_play = true;
    bool is_looping = true;
    bool return_to_default = true;
    float frame_time = 0.f;
    int total_frames = 0;

    int current_frame = 0;
    float current_frame_time = 0.f;
    float time = 0.f;
  };


  // Grouping emission shape options
  enum ParticleEmitShape
  {
      Sphere,
      Hemisphere,
      Cone,
      Box,
      // Sprite-shape can be added later
  };
  class __FLX_API ParticleSystem
  {
      FLX_REFL_SERIALIZABLE

      // Grouping emission rate settings
      struct ParticleEmitRate
      {
          FLX_REFL_SERIALIZABLE
          float rate_over_time = 10.0f;
          float rate_over_distance = 1.0f;
      };

  public:
      // Nested Particle class representing an individual particle's runtime state.
      class __FLX_API Particle
      {
          FLX_REFL_SERIALIZABLE
      public:
          // Lifetime simulation state
          float currentLifetime = 10.0f;    // Remaining lifetime (seconds)
          float totalLifetime = 10.0f;      // Original lifetime (for interpolation)

          // Current interpolated properties (derived from spawn values)
          float currentSpeed = 1.0f;
          float currentSize = 1.0f;
          Vector3 currentColor = Vector3::One;

          // Store spawn settings for interpolation (copied from the ParticleSystem emitter)
          float start_speed = 1.0f;        // Initial speed at spawn
          float end_speed = 1.0f;         // Final speed at death
          float start_size = 1.0f;         // Initial size at spawn
          float end_size = 1.0f;           // Final size at death
          Vector3 start_color = Vector3::One;      // Initial color at spawn
          Vector3 end_color = Vector3::One;        // Final color at death

          // You might want to add a default constructor and/or helper functions here.
      };

      // ParticleSystem (emitter) configuration settings:
      int max_particles = 10;
      FlexECS::Scene::StringIndex particlesprite_handle; // Sprite handle for particles
      // FlexECS::Scene::StringIndex particlespritesheet_handle; // Not implemented yet
      bool is_looping = true;
      float duration = 5.0f;    // How long the particle system will be active
      float start_delay = 0.0f;

      // Initial settings to be provided to spawned particles:
      float lifetime = 5.0f;
      float start_speed = 100.0f;
      float end_speed = 100.0f;
      float start_size = 100.0f;
      float end_size = 100.0f;
      Vector3 start_color = Vector3::One;
      Vector3 end_color = Vector3::One;
      float simulation_speed = 1.0f;

      ParticleEmitRate particleEmissionRate;
      int particleEmissionShapeIndex = static_cast<int>(ParticleEmitShape::Sphere);

      // Settings to assign to particles upon creation with other components:
      bool is_collidable = false; // Particles have AABB Collision (isStatic = true)
      bool is_static = false;     // Particles will inherit Velocity (isStatic = false)
  
      // --- Non-reflected runtime accumulator for emission ---
      float emissionAccumulator = 0.0f;
  };

  class __FLX_API Text
  {
    FLX_REFL_SERIALIZABLE

  public:
    FlexECS::Scene::StringIndex fonttype = FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)");
    FlexECS::Scene::StringIndex text = FLX_STRING_NEW("Default Text");
    Vector3 color = Vector3::One;
    std::pair<int, int> alignment = std::make_pair(1, 1); // Default value: centered (all bits set)
    Vector2 textboxDimensions = Vector2(850.0f, 300.0f);
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
    bool is_mouse_over;
    bool is_mouse_over_cached;
  };

  // unused
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

  /**************
   * Scripting
   **************/

  class __FLX_API Script
  {
    FLX_REFL_SERIALIZABLE

  public:
    FlexECS::Scene::StringIndex script_name = FLX_STRING_NEW("");
    bool is_awake = false;
    bool is_start = false;
  };

  /**************
   * UI
   **************/

  // display text with Text component
  // display image with Sprite component
  // click detection uses BoundingBox2D component and scripting callbacks
  // TODO: replace with proper ui system
  class __FLX_API Button
  {
    FLX_REFL_SERIALIZABLE

  public:
  };

   /**************
   * Gameplay
   **************/

  class __FLX_API Character
  {
    FLX_REFL_SERIALIZABLE

  public:
  };

  class __FLX_API Drifter
  {
    FLX_REFL_SERIALIZABLE

  public:
  };

  class __FLX_API Enemy
  {
    FLX_REFL_SERIALIZABLE

  public:
  };

  class __FLX_API Healthbar
  {
    FLX_REFL_SERIALIZABLE

  public:
    Vector3 original_scale = Vector3::One;
  };

  class __FLX_API MoveUI
  {
    FLX_REFL_SERIALIZABLE

  public:
  };

#pragma region Slot

  class __FLX_API CharacterSlot
  {
    FLX_REFL_SERIALIZABLE

  public:
    int slot_number = 0; // 1-7, 1-2 for player, 3-7 for enemy
  };

  class __FLX_API HealthbarSlot
  {
    FLX_REFL_SERIALIZABLE

  public:
    int slot_number = 0; // 1-7, 1-2 for player, 3-7 for enemy
  };

  class __FLX_API SpeedBarSlot
  {
    FLX_REFL_SERIALIZABLE

  public:
    int slot_number = 0; // 1-7
    int character = 0;   // 1-5
  };

#pragma endregion

  /**************
   * Script-Component Pairs
   **************/

#pragma region Moves

  class __FLX_API MoveOneComponent
  {
    FLX_REFL_SERIALIZABLE

  public:
  };

  class __FLX_API MoveTwoComponent
  {
    FLX_REFL_SERIALIZABLE

  public:
  };

  class __FLX_API MoveThreeComponent
  {
    FLX_REFL_SERIALIZABLE

  public:
  };

#pragma endregion

} // namespace FlexEngine
