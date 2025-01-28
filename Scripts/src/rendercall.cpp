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
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Sprite>())
    {
      Sprite* sprite = element.GetComponent<Sprite>();

      Renderer2DProps props;

      props.asset = FLX_STRING_GET(sprite->sprite_handle);
      props.texture_index = sprite->handle;
      props.position = Vector2(0.0f, 0.0f);
      props.scale = Vector2(384.0f / 8.f, 96.0f);
      //props.scale = Vector2(384.0f / asset_spritesheet.columns, 96.0f);
      props.window_size = Vector2(1600.0f, 900.0f);
      props.alignment = Renderer2DProps::Alignment_TopLeft;

      OpenGLRenderer::DrawTexture2D(camera, props);
    }
  }

  void Stop() override
  {
    
  }
};

// Static instance to ensure registration
static RenderLoopScript RenderLoop;
