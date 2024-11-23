#pragma once
/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// rendering.h
//
// This file defines the component classes used within the game
// engine, essential for rendering and maintaining hierarchical relationships
// between entities in a scene. The file is primarily responsible for
// serializing the data of components that influence the rendering process,
// ensuring that each entity's state is stored and can be reloaded accurately.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#include <FlexEngine.h>
#include "Editor/componentviewer.h"

namespace ChronoDrift
{
using namespace FlexEngine;

  using EntityName = FlexEngine::FlexECS::Scene::StringIndex;

  /*!***************************************************************************
  * \class IsActive
  * \brief
  * This class represents the activity state of an entity in the scene.
  * The `is_active` flag determines whether the entity should be rendered
  * or processed.
  ******************************************************************************/
  class IsActive
  { FLX_REFL_SERIALIZABLE
  public:
    bool is_active;
  };
  /*!***************************************************************************
  * \class Parent
  * \brief
  * This class represents the parent entity of an entity, enabling hierarchical
  * transformations where a child entity inherits its parent�s transformation
  * matrix. It supports complex scene graph structures.
  ******************************************************************************/
  class Parent
  { FLX_REFL_SERIALIZABLE
  public:
      FlexECS::Entity parent = FlexECS::Entity::Null;
  };
  /*!***************************************************************************
  * \class Position
  * \brief 
  * This class represents the position of an entity in 2D space. It defines 
  * where the entity is located in the scene using a 2D vector.
  ******************************************************************************/
  class Position
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 position = Vector2::Zero;
  };
  /*!***************************************************************************
  * \class Scale
  * \brief
  * This class represents the scale of an entity in 2D space, determining
  * how large or small the entity should be. The scale is defined using
  * a 2D vector.
  ******************************************************************************/
  class Scale
  { FLX_REFL_SERIALIZABLE
  public:
    Vector2 scale = Vector2::One;
  };
  /*!***************************************************************************
  * \class Rotation
  * \brief
  * This class represents the rotation of an entity in 3D space. It uses
  * a 3D vector to define rotations along the x, y, and z axes.
  ******************************************************************************/
  class Rotation 
  {
      FLX_REFL_SERIALIZABLE
  public:
      Vector3 rotation = Vector3::Zero;
  };
  /*!***************************************************************************
  * \class Transform
  * \brief
  * This class represents the complete transformation of an entity, combining
  * position, rotation, and scale into a 4x4 matrix. It supports hierarchical
  * transformations by allowing an entity to inherit its parent�s transform.
  ******************************************************************************/
  class Transform
  {
      FLX_REFL_SERIALIZABLE
  public:
      bool is_dirty = true; //Determines if transform needs to be updated
      Matrix4x4 transform = Matrix4x4::Identity;
  };
  /*!***************************************************************************
  * \class ZIndex
  * \brief
  * This class represents the z-order (depth) of an entity, determining its
  * rendering order relative to other entities. Higher `z` values are drawn
  * on top of lower ones.
  ******************************************************************************/
  class ZIndex
  { FLX_REFL_SERIALIZABLE
  public:
    int z;
  };
  /*!***************************************************************************
  * \class Shader
  * \brief
  * This class stores the shader program used to render the entity. It provides
  * a link between an entity and the shader resource identified by an index.
  ******************************************************************************/
  class Shader
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex shader = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_New(R"(\shaders\texture)");;
  };
  /*!***************************************************************************
  * \class Sprite
  * \brief
  * This class represents a 2D sprite component, storing texture and color data
  * for rendering. It also includes alignment and VBO information for the sprite's
  * display properties.
  ******************************************************************************/
  class Sprite
  { FLX_REFL_SERIALIZABLE
  public:
    FlexECS::Scene::StringIndex texture = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_New("");
    Vector3 color_to_add = Vector3::Zero;
    Vector3 color_to_multiply = Vector3::One;
    int alignment = Renderer2DProps::Alignment_Center;
    GLuint vbo_id = Renderer2DProps::VBO_Basic;
    bool post_processed = false;
  };
  
  class Animation
  {
      FLX_REFL_SERIALIZABLE
  public:
      FlexECS::Scene::StringIndex spritesheet = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_New("");
      GLuint rows = 1;
      GLuint cols = 0;
      GLuint max_sprites = 0;
      float m_animationDurationPerFrame = 0.02f;
      Vector3 color_to_add = Vector3::Zero;
      Vector3 color_to_multiply = Vector3::One;
      //Dont set
      float m_animationTimer = 0.f;
      int m_currentSpriteIndex = 0; // start from first sprite
      Vector4 m_currUV = Vector4::Zero;
  };

  class Text
  { FLX_REFL_SERIALIZABLE
  public:
      FlexECS::Scene::StringIndex fonttype = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_New(R"(..\\..\\..\\FlexEngine\\assets\\fonts\\Noto_Sans\\static\\NotoSans-Regular.ttf)");
      FlexECS::Scene::StringIndex text = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_New("TEST");
      Vector3 color = Vector3::One;
      // border color, border size, underline, etc
      std::pair<int,int> alignment = {Renderer2DText::Alignment_Center, Renderer2DText::Alignment_Middle}; // Default value: centered (all bits set)
      bool refocus;
  };

  /*!***************************************************************************
  * \class Camera
  * \brief
  * This class represents a camera in the scene, responsible for generating
  * view and projection matrices. It supports both perspective and orthographic
  * projections, with settings for field of view, near/far clipping planes, and
  * camera orientation.
  ******************************************************************************/
  class Camera
  {
      FLX_REFL_SERIALIZABLE
  public:
      CameraData camera;
  };

  // Button class for UI
  class Button
  {
      FLX_REFL_SERIALIZABLE
  public:
      bool interactable;             // Whether the button is interactable
      //std::string transition;        // Transition type (e.g., "Color Tint", "Sprite Swap", "None")
      FlexECS::Scene::StringIndex targetGraphic;             // Reference to the graphic for the button
      Vector3 normalColor;           // RGBA values for normal color
      Vector3 highlightedColor;      // RGBA values for highlighted color
      Vector3 pressedColor;          // RGBA values for pressed color
      Vector3 selectedColor;         // RGBA values for selected color
      Vector3 disabledColor;         // RGBA values for disabled color
      float colorMultiplier;         // Multiplier for the color tint
      float fadeDuration;            // Duration for color fading transitions
      //std::string navigation;        // Navigation mode (e.g., "Automatic", "None")
      //std::vector<std::function<void()>> onClickEvents;  // List of functions to execute on click
  };


  // Audio component requires a string to store the audio file path, and the flags to determine if it is meant to play or loop.
  class Audio
  {
    FLX_REFL_SERIALIZABLE
  public:
    bool should_play;
    bool is_looping;
    FlexECS::Scene::StringIndex audio_file = FlexECS::ecs_manager.GetActiveScene()->Internal_StringStorage_New("");
  };

  void RegisterRenderingComponents();
}
