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

namespace ChronoShift
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
    FlexECS::Entity parent;
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
    FlexECS::Scene::StringIndex shader;
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
    FlexECS::Scene::StringIndex texture;
    Vector3 color = Vector3(1.0f, 0.0f, 1.0f);
    Vector3 color_to_add = Vector3::Zero;
    Vector3 color_to_multiply = Vector3::One;
    int alignment = Renderer2DProps::Alignment_Center;
    GLuint vbo_id = Renderer2DProps::VBO_Basic;
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
      bool is_dirty = true;
      Vector3 front;
      Vector3 right;
      Vector3 up;
      const Vector3 world_up = Vector3::Up;
      Matrix4x4 view;
      Matrix4x4 projection;
      bool perspective = true;
      float fov = 45.0f;
      float near = 0.1f;
      float far = 100.0f;
  };

  void RegisterRenderingComponents();
}
