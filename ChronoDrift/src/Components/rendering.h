#pragma once
/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// rendering.h
//
// This file defines the component classes used within the game engine,
// essential for rendering and maintaining hierarchical relationships
// between entities in a scene. The file is primarily responsible for
// serializing the data of components that influence the rendering process,
// ensuring that each entity's state is stored and can be reloaded accurately.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Main Author
//   - Designed and implemented the component system for 2D rendering,
//     including support for hierarchical transformations, sprite rendering,
//     and UI components like buttons and audio.
// [10%] Yew Chong (yewchong.k\@digipen.edu)
//   - Sub Author
//   - Designed audio and scripting
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include <FlexEngine.h>
#ifndef GAME
#include "Editor/componentviewer.h"
#endif
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
    bool is_active = true;
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
    FlexECS::Scene::StringIndex shader = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\shaders\batchtexture)");;
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
    FlexECS::Scene::StringIndex texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
    Vector3 color_to_add = Vector3::Zero;
    Vector3 color_to_multiply = Vector3::One;
    int alignment = Renderer2DProps::Alignment_Center;
    GLuint vbo_id = Renderer2DProps::VBO_Basic;
    bool post_processed = false;
  };

  /*!***************************************************************************
  * \class Animation
  * \brief
  * This class represents an animation component that handles the spritesheet,
  * sprite indexing, and animation speed for rendering a sequence of images. It
  * includes functionality to manage the animation's speed, color adjustments,
  * and handling the current animation state (paused or playing).
  ***************************************************************************/
  class Animation
  {
      FLX_REFL_SERIALIZABLE
  public:
      FlexECS::Scene::StringIndex spritesheet = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
      GLuint rows = 1;
      GLuint cols = 0;
      GLuint max_sprites = 0;
      float m_animation_speed = 1.0f;
      Vector3 color_to_add = Vector3::Zero;
      Vector3 color_to_multiply = Vector3::One;
      //Dont set
      float m_animationTimer = 0.f;
      int m_currentSpriteIndex = 0; // start from first sprite
      Vector4 m_currUV = Vector4::Zero;
      bool is_paused = false;
  };

  /*!***************************************************************************
  * \class Text
  * \brief
  * This class represents a text component, storing information such as font type,
  * the text string, text color, and alignment. It is used for rendering text in
  * the 2D space, with support for different fonts and alignments.
  ***************************************************************************/
  class Text
  { FLX_REFL_SERIALIZABLE
  public:
      FlexECS::Scene::StringIndex fonttype = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\fonts\Prompt\Prompt-Black.ttf)");
      FlexECS::Scene::StringIndex text = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("TEST");
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

  /*!***************************************************************************
  * \class Button
  * \brief
  * This class represents a button UI component, handling its interactivity state,
  * color changes upon interaction (normal, highlighted, pressed, and disabled states),
  * and the color transition effects. It allows for smooth visual feedback on button presses.
  ***************************************************************************/
  class Button
  {
      FLX_REFL_SERIALIZABLE
  public:
      bool is_interactable = true;             // Whether the button is interactable
      Vector3 normalColor;           // RGBA values for normal color
      Vector3 highlightedColor;      // RGBA values for highlighted color
      Vector3 pressedColor;          // RGBA values for pressed color
      Vector3 disabledColor;         // RGBA values for disabled color
      float colorMultiplier;         // Multiplier for the color tint
      float fadeDuration;            // Duration for color fading transitions -> not done

      //Do not show
      Vector3 finalColorMul; 
      Vector3 finalColorAdd;

      //Pending
      //std::vector<std::function<void()>> onClickEvents;  // List of functions to execute on click
      // Transition type (e.g., "Color Tint", "Sprite Swap", "None")
  };


  /*!***************************************************************************
  * \class Audio
  * \brief
  * This class represents an audio component, allowing for playback of audio files
  * with options for looping and control over the playback state. It stores the audio
  * file path and the necessary flags to determine if the audio should play or loop.
  ***************************************************************************/
  class Audio
  {
    FLX_REFL_SERIALIZABLE
  public:
    bool should_play;
    bool is_looping;
    FlexECS::Scene::StringIndex audio_file = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New("");
  };

  /*!***************************************************************************
  * \class Script
  * \brief
  * This class represents a script component that holds the identifier for a script
  * that is attached to an entity. The script can be used to define behaviors for
  * entities in the game scene.
  ***************************************************************************/
  class Script
  {
    FLX_REFL_SERIALIZABLE
  public:
    int script_id;
  };

  /*!***************************************************************************
  * \function RegisterRenderingComponents
  * \brief
  * Registers all the rendering components used in the game engine. This includes
  * components related to 2D sprite rendering, animations, text rendering, UI elements,
  * camera management, and audio components, ensuring that all necessary components
  * are properly initialized and available for the rendering pipeline.
  ***************************************************************************/
  #ifndef GAME
  void RegisterRenderingComponents();
  #endif
}
