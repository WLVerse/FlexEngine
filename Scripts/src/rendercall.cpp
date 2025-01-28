#include <FlexEngine.h>
#include "FlexEngine/Physics/physicssystem.h"

using namespace FlexEngine;

class RenderLoopScript : public Script
{
public:
  RenderLoopScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "RenderLoop"; }

  void Awake() override
  {

  }

  void Start() override
  {
   
  }

  void Update() override
  {
    static Camera camera(0.0f, 1600.0f, 900.0f, 0.0f, -2.0f, 2.0f);

    // Physics
    FlexEngine::PhysicsSystem::UpdatePhysicsSystem();

    // Update Transform component
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Position, Rotation, Scale, Transform>())
    {
      auto position = element.GetComponent<Position>()->position;
      auto rotation = element.GetComponent<Rotation>()->rotation;
      auto scale = element.GetComponent<Scale>()->scale;
      auto transform = element.GetComponent<Transform>();

      Matrix4x4 translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, position);
      Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(rotation).ToRotationMatrix();
      Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, scale);

      transform->transform = translation_matrix * rotation_matrix * scale_matrix;
    }

    Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
    // Graphics
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Animator>())
    {
      Animator* animator = element.GetComponent<Animator>();
      auto& asset_spritesheet = FLX_ASSET_GET(Asset::Spritesheet, FLX_STRING_GET(animator->spritesheet_file));
      animator->time += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
      int index = (int)(animator->time * asset_spritesheet.columns) % asset_spritesheet.columns;

      // Override sprite component
      Sprite* sprite = element.GetComponent<Sprite>();
      if (sprite != nullptr)
      {
        sprite->sprite_handle = animator->spritesheet_file;
        sprite->handle = index;
      }
      else Log::Fatal("Somehow, a animator exists without a sprite component attached to it. This should be impossible with editor creation.");
    }


    //Sprite pass always happens after animator to ensure it renders the right image.
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite, Position, Rotation, Scale, Transform>())
    {
      auto sprite = element.GetComponent<Sprite>();
      auto position = element.GetComponent<Position>()->position;
      auto rotation = element.GetComponent<Rotation>()->rotation;
      auto scale = element.GetComponent<Scale>()->scale;
      auto transform = element.GetComponent<Transform>();

      Renderer2DProps props;

      props.asset = FLX_STRING_GET(sprite->sprite_handle);
      props.texture_index = sprite->handle;
      props.position = { position.x, position.y };
      props.scale = { scale.x, scale.y };
      //props.position = Vector2(400.0f, 300.0f);
      //props.scale = Vector2(300.0f, 300.0f);
      //props.position = Vector2(0.0f, 0.0f);
      //props.scale = Vector2(384.0f / 8.f, 96.0f);
      //props.scale = Vector2(384.0f / asset_spritesheet.columns, 96.0f);
      //props.window_size = Vector2(1600.0f, 900.0f);
      props.alignment = Renderer2DProps::Alignment_TopLeft;

      OpenGLRenderer::DrawTexture2D(camera, props);
    }


    OpenGLFrameBuffer::Unbind();
  }

  void Stop() override
  {
    
  }
};

// Static instance to ensure registration
static RenderLoopScript RenderLoop;
