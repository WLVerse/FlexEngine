#include "editorcomponents.h"
#include "componentviewer.h"

namespace Editor
{
  COMPONENT_VIEWER_START(Position)
  COMPONENT_VIEWER_DRAG_VECTOR3(position)
  COMPONENT_VIEWER_END(Position)

  COMPONENT_VIEWER_START(Rotation)
  COMPONENT_VIEWER_DRAG_VECTOR3(rotation)
  COMPONENT_VIEWER_END(Rotation)

  COMPONENT_VIEWER_START(Scale)
  COMPONENT_VIEWER_DRAG_VECTOR3(scale)
  COMPONENT_VIEWER_END(Scale)

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

  void COMPONENT_ADDER_Audio(FlexEngine::FlexECS::Entity entity)
  {
    if (entity.HasComponent<Audio>()) return;

    entity.AddComponent<Audio>({ false, false, false, false, FLX_STRING_NEW("") });
  }
  void COMPONENT_REMOVER_Audio(FlexEngine::FlexECS::Entity entity)
  {
    if (!entity.HasComponent<Audio>()) return;

    entity.RemoveComponent<Audio>();
  }
  COMPONENT_VIEWER_START_MANUAL(Audio)
  COMPONENT_VIEWER_BOOL(should_play)
  COMPONENT_VIEWER_BOOL(is_looping);
  COMPONENT_VIEWER_AUDIO_PATH(audio_file);
  COMPONENT_VIEWER_END(Audio)
  
  void COMPONENT_ADDER_Sprite(FlexEngine::FlexECS::Entity entity)
  {
    if(entity.HasComponent<Sprite>()) return;
        
    entity.AddComponent<Sprite>({ FLX_STRING_NEW("") });
  }
  void COMPONENT_REMOVER_Sprite(FlexEngine::FlexECS::Entity entity)
  {
    if (!entity.HasComponent<Sprite>()) return;

    entity.RemoveComponent<Sprite>();
  }
  COMPONENT_VIEWER_START_MANUAL(Sprite)
  COMPONENT_VIEWER_TEXTURE_PATH(sprite_handle)
  COMPONENT_VIEWER_DRAG_VECTOR2(scale)
  COMPONENT_VIEWER_CHECKBOX(center_aligned)
  COMPONENT_VIEWER_DRAG_FLOAT(opacity)
  COMPONENT_VIEWER_END(Sprite)

  COMPONENT_VIEWER_START(Animator)
  COMPONENT_VIEWER_SPRITESHEET_PATH(spritesheet_handle)
  COMPONENT_VIEWER_BOOL(should_play)
  COMPONENT_VIEWER_DRAG_FLOAT(time)
  COMPONENT_VIEWER_END(Animator)



  /**************
  * Camera
  * Use CameraData
  **************/
  COMPONENT_VIEWER_START(Camera)
      COMPONENT_VIEWER_DRAG_VECTOR3(m_data.position)
      COMPONENT_VIEWER_DRAG_VECTOR3(m_data.target)
      COMPONENT_VIEWER_DRAG_VECTOR3(m_data.up)
      COMPONENT_VIEWER_DRAG_VECTOR3(m_data.right)

      COMPONENT_VIEWER_DRAG_FLOAT(m_data.fieldOfView)
      COMPONENT_VIEWER_DRAG_FLOAT(m_data.aspectRatio)
      COMPONENT_VIEWER_DRAG_FLOAT(m_data.nearClip)
      COMPONENT_VIEWER_DRAG_FLOAT(m_data.farClip)
      COMPONENT_VIEWER_DRAG_FLOAT(m_data.m_OrthoWidth)
      COMPONENT_VIEWER_DRAG_FLOAT(m_data.m_OrthoHeight)

      COMPONENT_VIEWER_BOOL(m_data.m_isOrthographic)
  COMPONENT_VIEWER_END(Camera)


  COMPONENT_VIEWER_START(Text)
      COMPONENT_VIEWER_EDITABLE_STRING(text)
      COMPONENT_VIEWER_DRAG_VECTOR3(color)
      COMPONENT_VIEWER_DRAG_VECTOR2(textboxDimensions)
  COMPONENT_VIEWER_END(Text)

  COMPONENT_VIEWER_START(ParticleSystem)
      COMPONENT_VIEWER_DRAG_INT(ParticleSystem::max_particles)
      COMPONENT_VIEWER_TEXTURE_PATH(particlesprite_handle)
      COMPONENT_VIEWER_BOOL(ParticleSystem::is_looping)
      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::duration)
      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::start_delay)

      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::lifetime)
      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::start_speed)
      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::end_speed)
      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::start_size)
      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::end_size)

      COMPONENT_VIEWER_DRAG_VECTOR3(ParticleSystem::start_color)
      COMPONENT_VIEWER_DRAG_VECTOR3(ParticleSystem::end_color)

      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::simulation_speed)
      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::particleEmissionRate.rate_over_time)
      COMPONENT_VIEWER_DRAG_FLOAT(ParticleSystem::particleEmissionRate.rate_over_distance)
      COMPONENT_VIEWER_DRAG_INT(ParticleSystem::particleEmissionShapeIndex)

      COMPONENT_VIEWER_BOOL(ParticleSystem::is_collidable)
      COMPONENT_VIEWER_BOOL(ParticleSystem::is_static)
  COMPONENT_VIEWER_END(ParticleSystem)
  /**************
   * Physics
   **************/
  COMPONENT_VIEWER_START(BoundingBox2D)
  COMPONENT_VIEWER_DRAG_VECTOR2(min)
  COMPONENT_VIEWER_DRAG_VECTOR2(max)
  COMPONENT_VIEWER_BOOL(is_colliding)
  COMPONENT_VIEWER_BOOL(is_mouse_over)
  // COMPONENT_VIEWER_BOOL(is_mouse_over_cached) // no need to show this
  COMPONENT_VIEWER_END(BoundingBox2D)

  COMPONENT_VIEWER_START(Rigidbody)
  COMPONENT_VIEWER_DRAG_VECTOR2(velocity)
  COMPONENT_VIEWER_BOOL(is_static)
  COMPONENT_VIEWER_END(Rigidbody)


  /**************
   * Scripting
   **************/
  COMPONENT_VIEWER_START(Script)
  COMPONENT_VIEWER_EDITABLE_STRING(script_name)
  COMPONENT_VIEWER_END(Script)


  /**************
   * UI
   **************/
  COMPONENT_VIEWER_START(Button)
  COMPONENT_VIEWER_END(Button)

  void RegisterComponents()
  {
    REGISTER_COMPONENT_VIEWER(Position);
    REGISTER_COMPONENT_VIEWER(Rotation);
    REGISTER_COMPONENT_VIEWER(Scale);
    REGISTER_COMPONENT_VIEWER_FUNCTIONS(Transform, COMPONENT_ENABLE_ADD, COMPONENT_DISABLE_REMOVE);
    REGISTER_COMPONENT_VIEWER(ZIndex);
    REGISTER_COMPONENT_VIEWER(Parent);
    //REGISTER_COMPONENT_VIEWER(Audio);
    //REGISTER_COMPONENT_VIEWER(Sprite);
    REGISTER_COMPONENT_VIEWER_FUNCTIONS(Audio, COMPONENT_ENABLE_ADD, COMPONENT_ENABLE_REMOVE);
    REGISTER_COMPONENT_VIEWER_FUNCTIONS(Sprite, COMPONENT_ENABLE_ADD, COMPONENT_ENABLE_REMOVE);
    REGISTER_COMPONENT_VIEWER(Animator);
    REGISTER_COMPONENT_VIEWER(Camera);
    REGISTER_COMPONENT_VIEWER(Text);
    REGISTER_COMPONENT_VIEWER(ParticleSystem);
    REGISTER_COMPONENT_VIEWER(BoundingBox2D);
    REGISTER_COMPONENT_VIEWER(Rigidbody);

    REGISTER_COMPONENT_VIEWER(Script);

    REGISTER_COMPONENT_VIEWER(Button);
  }
} // namespace Editor
