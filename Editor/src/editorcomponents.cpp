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
  COMPONENT_VIEWER_BOOL(is_active)
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

    entity.AddComponent<Audio>({ FLX_STRING_NEW(""), false, false, false, false});
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
  COMPONENT_VIEWER_SPRITESHEET_PATH(default_spritesheet_handle)
  COMPONENT_VIEWER_BOOL(should_play)
  COMPONENT_VIEWER_BOOL(is_looping)
  COMPONENT_VIEWER_BOOL(return_to_default)
  COMPONENT_VIEWER_END(Animator)


  COMPONENT_VIEWER_START(VideoPlayer)
    COMPONENT_VIEWER_VIDEO_PATH(video_file)
    COMPONENT_VIEWER_CHECKBOX(should_play)
    COMPONENT_VIEWER_CHECKBOX(is_looping)
  COMPONENT_VIEWER_END(VideoPlayer)

  /**************
  * Camera
  * Use CameraData
  **************/
  COMPONENT_VIEWER_START(Camera)
      COMPONENT_VIEWER_DRAG_FLOAT(m_ortho_width)
      COMPONENT_VIEWER_DRAG_FLOAT(m_ortho_height)
      COMPONENT_VIEWER_BOOL(is_active)
  COMPONENT_VIEWER_END(Camera)


  COMPONENT_VIEWER_START(Text)
      COMPONENT_VIEWER_FONT_PATH(fonttype)
      COMPONENT_VIEWER_EDITABLE_STRING(text)
      COMPONENT_VIEWER_COLOR3(color)
      COMPONENT_VIEWER_TEXT_ALIGNMENT(alignment)
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

  COMPONENT_VIEWER_START(MoveUI)
  COMPONENT_VIEWER_END(MoveUI)

  COMPONENT_VIEWER_START(SettingsUI)
  COMPONENT_VIEWER_END(SettingsUI)

  COMPONENT_VIEWER_START(CreditsUI)
  COMPONENT_VIEWER_END(CreditsUI)

  COMPONENT_VIEWER_START(QuitUI)
  COMPONENT_VIEWER_END(QuitUI)

  COMPONENT_VIEWER_START(PauseUI)
  COMPONENT_VIEWER_END(PauseUI)

  COMPONENT_VIEWER_START(PauseHoverUI)
  COMPONENT_VIEWER_END(PauseHoverUI)

  COMPONENT_VIEWER_START(Slider)
  COMPONENT_VIEWER_END(Slider)

   /**************
   * Gameplay
   **************/

  COMPONENT_VIEWER_START(Healthbar)
    //pixelLength, original_position and original_scale no need show ba
  COMPONENT_VIEWER_END(CharacterSlot)

  COMPONENT_VIEWER_START(SpeedBarSlot)
    COMPONENT_VIEWER_DRAG_INT(slot_number)
    COMPONENT_VIEWER_DRAG_INT(character)
  COMPONENT_VIEWER_END(SpeedBarSlot)

  COMPONENT_VIEWER_START(SpeedBarSlotTarget)
    COMPONENT_VIEWER_DRAG_INT(slot_number)
  COMPONENT_VIEWER_END(SpeedBarSlotTarget)

  COMPONENT_VIEWER_START(CharacterSlot)
    COMPONENT_VIEWER_DRAG_INT(slot_number)
  COMPONENT_VIEWER_END(CharacterSlot)


  /****************
  * Post_Processing
  ****************/
  COMPONENT_VIEWER_START(PostProcessingMarker)
  COMPONENT_VIEWER_BOOL(enableGaussianBlur)
  COMPONENT_VIEWER_BOOL(enableChromaticAberration)
  COMPONENT_VIEWER_BOOL(enableBloom)
  COMPONENT_VIEWER_BOOL(enableVignette)
  COMPONENT_VIEWER_BOOL(enableColorGrading)
  COMPONENT_VIEWER_BOOL(enableFilmGrain)
  COMPONENT_VIEWER_BOOL(enablePixelate)
  COMPONENT_VIEWER_BOOL(enableWarp)
  COMPONENT_VIEWER_DRAG_FLOAT(globalIntensity)
  COMPONENT_VIEWER_END(PostProcessingMarker)
  COMPONENT_VIEWER_START(PostProcessingGaussianBlur)
  COMPONENT_VIEWER_DRAG_INT(intensity)
  COMPONENT_VIEWER_DRAG_FLOAT(distance)
  COMPONENT_VIEWER_DRAG_INT(blurPasses)
  COMPONENT_VIEWER_END(PostProcessingGaussianBlur)
  COMPONENT_VIEWER_START(PostProcessingChromaticAbberation)
  COMPONENT_VIEWER_DRAG_FLOAT(intensity)
  COMPONENT_VIEWER_DRAG_VECTOR2(redOffset)
  COMPONENT_VIEWER_DRAG_VECTOR2(greenOffset)
  COMPONENT_VIEWER_DRAG_VECTOR2(blueOffset)
  COMPONENT_VIEWER_DRAG_VECTOR2(edgeRadius)
  COMPONENT_VIEWER_DRAG_VECTOR2(edgeSoftness)
  COMPONENT_VIEWER_END(PostProcessingChromaticAbberation)
  COMPONENT_VIEWER_START(PostProcessingBloom)
  COMPONENT_VIEWER_DRAG_FLOAT(threshold)
  COMPONENT_VIEWER_DRAG_FLOAT(intensity)
  COMPONENT_VIEWER_DRAG_FLOAT(radius)
  COMPONENT_VIEWER_END(PostProcessingBloom)
  COMPONENT_VIEWER_START(PostProcessingVignette)
  COMPONENT_VIEWER_DRAG_FLOAT(intensity)
  COMPONENT_VIEWER_DRAG_VECTOR2(radius)
  COMPONENT_VIEWER_DRAG_VECTOR2(softness)
  COMPONENT_VIEWER_END(PostProcessingVignette)
  COMPONENT_VIEWER_START(PostProcessingColorGrading)
  COMPONENT_VIEWER_DRAG_FLOAT(brightness)
  COMPONENT_VIEWER_DRAG_FLOAT(contrast)
  COMPONENT_VIEWER_DRAG_FLOAT(saturation)
  COMPONENT_VIEWER_END(PostProcessingColorGrading)
  COMPONENT_VIEWER_START(PostProcessingPixelate)
  COMPONENT_VIEWER_DRAG_INT(pixelWidth)
  COMPONENT_VIEWER_DRAG_INT(pixelHeight)
  COMPONENT_VIEWER_END(PostProcessingPixelate)
  COMPONENT_VIEWER_START(PostProcessingFilmGrain)
  COMPONENT_VIEWER_DRAG_FLOAT(grainIntensity)
  COMPONENT_VIEWER_DRAG_FLOAT(grainSize)
  COMPONENT_VIEWER_BOOL(animateGrain)
  COMPONENT_VIEWER_END(PostProcessingFilmGrain)
  COMPONENT_VIEWER_START(PostProcessingWarp)
  COMPONENT_VIEWER_DRAG_FLOAT(warpStrength)
  COMPONENT_VIEWER_DRAG_FLOAT(warpRadius)
  COMPONENT_VIEWER_END(PostProcessingWarp)

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
    REGISTER_COMPONENT_VIEWER(VideoPlayer);
    REGISTER_COMPONENT_VIEWER(Camera);
    REGISTER_COMPONENT_VIEWER(Text);
    REGISTER_COMPONENT_VIEWER(ParticleSystem);
    REGISTER_COMPONENT_VIEWER(BoundingBox2D);
    REGISTER_COMPONENT_VIEWER(Rigidbody);

    REGISTER_COMPONENT_VIEWER(Script);

    REGISTER_COMPONENT_VIEWER(Button);

    REGISTER_COMPONENT_VIEWER(MoveUI);
    REGISTER_COMPONENT_VIEWER(SettingsUI);
    REGISTER_COMPONENT_VIEWER(CreditsUI);
    REGISTER_COMPONENT_VIEWER(QuitUI);
    REGISTER_COMPONENT_VIEWER(PauseUI);
    REGISTER_COMPONENT_VIEWER(PauseHoverUI);
    REGISTER_COMPONENT_VIEWER(Slider);
    
    REGISTER_COMPONENT_VIEWER(Healthbar);
    REGISTER_COMPONENT_VIEWER(SpeedBarSlot);
    REGISTER_COMPONENT_VIEWER(SpeedBarSlotTarget);
    REGISTER_COMPONENT_VIEWER(CharacterSlot);

    REGISTER_COMPONENT_VIEWER(PostProcessingMarker);
    REGISTER_COMPONENT_VIEWER(PostProcessingGaussianBlur);
    REGISTER_COMPONENT_VIEWER(PostProcessingChromaticAbberation);
    REGISTER_COMPONENT_VIEWER(PostProcessingBloom);
    REGISTER_COMPONENT_VIEWER(PostProcessingVignette);
    REGISTER_COMPONENT_VIEWER(PostProcessingColorGrading);
    REGISTER_COMPONENT_VIEWER(PostProcessingPixelate);
    REGISTER_COMPONENT_VIEWER(PostProcessingFilmGrain);
    REGISTER_COMPONENT_VIEWER(PostProcessingWarp);
  }
} // namespace Editor
