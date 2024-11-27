/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// rendering.cpp
//
// This file defines the component classes used within the game engine,
// essential for rendering and maintaining hierarchical relationships
// between entities in a scene. The file is primarily responsible for
// serializing the data of components that influence the rendering process,
// ensuring that each entity's state is stored and can be reloaded accurately.
//
// AUTHORS
// [90%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Main Author
//   - Designed and implemented the component system for 2D rendering,
//     including support for hierarchical transformations, sprite rendering,
//     and UI components like buttons.
// [10%] Yew Chong (yewchong.k\@digipen.edu)
//   - Sub Author
//   - Designed audio and scripting
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "rendering.h"

namespace ChronoDrift
{
#pragma region Register Variables
  FLX_REFL_REGISTER_START(IsActive)
    FLX_REFL_REGISTER_PROPERTY(is_active)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Parent)
    FLX_REFL_REGISTER_PROPERTY(parent)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Position)
    FLX_REFL_REGISTER_PROPERTY(position)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Scale)
    FLX_REFL_REGISTER_PROPERTY(scale)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Rotation)
    FLX_REFL_REGISTER_PROPERTY(rotation)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Transform)
      FLX_REFL_REGISTER_PROPERTY(is_dirty)
      FLX_REFL_REGISTER_PROPERTY(transform)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(ZIndex)
    FLX_REFL_REGISTER_PROPERTY(z)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Shader)
    FLX_REFL_REGISTER_PROPERTY(shader)
  FLX_REFL_REGISTER_END;
  
  FLX_REFL_REGISTER_START(Sprite)
    FLX_REFL_REGISTER_PROPERTY(texture)
    FLX_REFL_REGISTER_PROPERTY(color_to_add)
    FLX_REFL_REGISTER_PROPERTY(color_to_multiply)
    FLX_REFL_REGISTER_PROPERTY(alignment)
    FLX_REFL_REGISTER_PROPERTY(vbo_id)
    FLX_REFL_REGISTER_PROPERTY(post_processed)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Animation)
      FLX_REFL_REGISTER_PROPERTY(spritesheet)
      FLX_REFL_REGISTER_PROPERTY(rows)
      FLX_REFL_REGISTER_PROPERTY(cols)
      FLX_REFL_REGISTER_PROPERTY(max_sprites)
      FLX_REFL_REGISTER_PROPERTY(color_to_add)
      FLX_REFL_REGISTER_PROPERTY(color_to_multiply)
      FLX_REFL_REGISTER_PROPERTY(m_animationTimer)
      FLX_REFL_REGISTER_PROPERTY(m_animation_speed)
      FLX_REFL_REGISTER_PROPERTY(m_currentSpriteIndex)
      FLX_REFL_REGISTER_PROPERTY(m_currUV)
      FLX_REFL_REGISTER_PROPERTY(is_paused)
  FLX_REFL_REGISTER_END;

  FLX_REFL_REGISTER_START(Camera)
      FLX_REFL_REGISTER_PROPERTY(camera)
  FLX_REFL_REGISTER_END

  FLX_REFL_REGISTER_START(Text)
      FLX_REFL_REGISTER_PROPERTY(fonttype)
      FLX_REFL_REGISTER_PROPERTY(text)
      FLX_REFL_REGISTER_PROPERTY(color)
      FLX_REFL_REGISTER_PROPERTY(alignment)
      FLX_REFL_REGISTER_PROPERTY(refocus)
  FLX_REFL_REGISTER_END;

    FLX_REFL_REGISTER_START(Button)
        FLX_REFL_REGISTER_PROPERTY(is_interactable)
        FLX_REFL_REGISTER_PROPERTY(normalColor)
        FLX_REFL_REGISTER_PROPERTY(highlightedColor)
        FLX_REFL_REGISTER_PROPERTY(pressedColor)
        FLX_REFL_REGISTER_PROPERTY(disabledColor)
        FLX_REFL_REGISTER_PROPERTY(colorMultiplier)
        FLX_REFL_REGISTER_PROPERTY(fadeDuration)
        FLX_REFL_REGISTER_PROPERTY(finalColorMul)
        FLX_REFL_REGISTER_PROPERTY(finalColorAdd)
    FLX_REFL_REGISTER_END;

    FLX_REFL_REGISTER_START(Audio)
      FLX_REFL_REGISTER_PROPERTY(is_looping)
      FLX_REFL_REGISTER_PROPERTY(should_play)
      FLX_REFL_REGISTER_PROPERTY(audio_file)
    FLX_REFL_REGISTER_END;

    FLX_REFL_REGISTER_START(Script)
      FLX_REFL_REGISTER_PROPERTY(script_id)
    FLX_REFL_REGISTER_END;
#pragma endregion

#pragma region Editable Viewer

  COMPONENT_VIEWER_START(Position)
      //Inspector.cpp replacing 
    COMPONENT_VIEWER_DRAG_VECTOR2(position)
  COMPONENT_VIEWER_END(Position)

  COMPONENT_VIEWER_START(Rotation)
      //Inspector.cpp replacing 
    COMPONENT_VIEWER_DRAG_VECTOR3(rotation)
  COMPONENT_VIEWER_END(Rotation)

  COMPONENT_VIEWER_START(Scale)
      //Inspector.cpp replacing 
    COMPONENT_VIEWER_DRAG_VECTOR2(scale)
  COMPONENT_VIEWER_END(Scale)

  COMPONENT_VIEWER_START(IsActive)
      //Inspector.cpp replacing 
    COMPONENT_VIEWER_CHECKBOX(is_active)
  COMPONENT_VIEWER_END(IsActive)

  COMPONENT_VIEWER_START(Transform)
    COMPONENT_VIEWER_BOOL(is_dirty)
    COMPONENT_VIEWER_MAT44(transform)
  COMPONENT_VIEWER_END(Transform)

  COMPONENT_VIEWER_START(ZIndex)
    COMPONENT_VIEWER_DRAG_INT(z)
  COMPONENT_VIEWER_END(ZIndex)

  COMPONENT_VIEWER_START(Parent)
    COMPONENT_VIEWER_ENTITY_REFERENCE(parent)
  COMPONENT_VIEWER_END(Parent)

  COMPONENT_VIEWER_START(Shader)
    COMPONENT_VIEWER_SHADER_PATH(shader)
  COMPONENT_VIEWER_END(Shader)

  COMPONENT_VIEWER_START(Sprite)
    COMPONENT_VIEWER_TEXTURE_PATH(texture)
    COMPONENT_VIEWER_COLOR3(color_to_add)
    COMPONENT_VIEWER_COLOR3(color_to_multiply)
    COMPONENT_VIEWER_CHECKBOX(post_processed)
  COMPONENT_VIEWER_END(Sprite)

  COMPONENT_VIEWER_START(Animation)
    COMPONENT_VIEWER_CHECKBOX(is_paused)
    COMPONENT_VIEWER_DRAG_GLUINT(cols);
    COMPONENT_VIEWER_DRAG_GLUINT(rows);
    COMPONENT_VIEWER_DRAG_GLUINT(max_sprites);
    COMPONENT_VIEWER_DRAG_FLOAT(m_animation_speed);
    COMPONENT_VIEWER_TEXTURE_PATH(spritesheet)
    COMPONENT_VIEWER_COLOR3(color_to_add)
    COMPONENT_VIEWER_COLOR3(color_to_multiply)
  COMPONENT_VIEWER_END(Animation)

  COMPONENT_VIEWER_START(Camera)
     COMPONENT_VIEWER_DRAG_FLOAT(camera.fieldOfView)
     COMPONENT_VIEWER_DRAG_FLOAT(camera.aspectRatio)
     COMPONENT_VIEWER_DRAG_FLOAT(camera.nearClip)
     COMPONENT_VIEWER_DRAG_FLOAT(camera.farClip)
     COMPONENT_VIEWER_BOOL(camera.m_isOrthographic)
  COMPONENT_VIEWER_END(Camera)

  //TODO @ROCKY to add the font pic and have a dropbox of available fonts
  COMPONENT_VIEWER_START(Text)
      COMPONENT_VIEWER_FONT_PATH(fonttype)
      COMPONENT_VIEWER_EDITABLE_STRING(text)
      COMPONENT_VIEWER_COLOR3(color)
      COMPONENT_VIEWER_CHECKBOX(refocus)
  COMPONENT_VIEWER_END(Text)

  COMPONENT_VIEWER_START(Button)
     COMPONENT_VIEWER_CHECKBOX(is_interactable)
     COMPONENT_VIEWER_COLOR3(normalColor)
     COMPONENT_VIEWER_COLOR3(highlightedColor)
     COMPONENT_VIEWER_COLOR3(pressedColor)
     COMPONENT_VIEWER_COLOR3(disabledColor)
     COMPONENT_VIEWER_DRAG_FLOAT(colorMultiplier)
     COMPONENT_VIEWER_DRAG_FLOAT(fadeDuration)
  COMPONENT_VIEWER_END(Button)

  COMPONENT_VIEWER_START(Audio)
    COMPONENT_VIEWER_BOOL(should_play)
    COMPONENT_VIEWER_AUDIO_PATH(audio_file);
  COMPONENT_VIEWER_END(Audio)

  COMPONENT_VIEWER_START(Script)
    COMPONENT_VIEWER_DRAG_INT(script_id)
  COMPONENT_VIEWER_END(Script)
#pragma endregion

  void RegisterRenderingComponents()
  {
    REGISTER_COMPONENT_VIEWER(Position);
    REGISTER_COMPONENT_VIEWER(Rotation);
    REGISTER_COMPONENT_VIEWER(Scale);
    REGISTER_COMPONENT_VIEWER(IsActive);
    REGISTER_COMPONENT_VIEWER_FUNCTIONS(Transform, COMPONENT_ENABLE_ADD, COMPONENT_DISABLE_REMOVE);
    REGISTER_COMPONENT_VIEWER(ZIndex);
    REGISTER_COMPONENT_VIEWER(Parent);
    REGISTER_COMPONENT_VIEWER(Shader);
    REGISTER_COMPONENT_VIEWER(Sprite);
    REGISTER_COMPONENT_VIEWER(Text);
    REGISTER_COMPONENT_VIEWER(Camera);
    REGISTER_COMPONENT_VIEWER(Animation);
    REGISTER_COMPONENT_VIEWER(Button);
    REGISTER_COMPONENT_VIEWER(Audio); 
    REGISTER_COMPONENT_VIEWER(Script);
  }
}