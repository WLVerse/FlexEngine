#include <FlexEngine.h>
#include "FlexEngine/Physics/physicssystem.h"
#include "camerahandler.cpp"
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
    // Camera 
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Camera>())
    {
        auto ref = element.GetComponent<Camera>();
        if (ref->getIsActive() /*&& isdirty*/)
            element.GetComponent<Camera>()->Update();
    }
    FlexECS::EntityID currGameCamID = dynamic_cast<CameraHandler*>(ScriptRegistry::GetScript("CameraHandler"))->GetMainGameCameraID();

    // Physics
    FlexEngine::PhysicsSystem::UpdatePhysicsSystem();

    // Graphics
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Animator>())
    {
        if (!element.GetComponent<Transform>()->is_active) continue;

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
        if (!element.GetComponent<Transform>()->is_active) continue;

        Sprite* sprite = element.GetComponent<Sprite>();

        Renderer2DProps props;

        props.asset = FLX_STRING_GET(sprite->sprite_handle);
        props.texture_index = sprite->handle;
        props.position = Vector2(0.0f, 0.0f);
        props.scale = Vector2(384.0f / 8.f, 96.0f);
        //props.scale = Vector2(384.0f / asset_spritesheet.columns, 96.0f);
        props.window_size = Vector2(1600.0f, 900.0f);
        props.alignment = Renderer2DProps::Alignment_TopLeft;

        OpenGLRenderer::DrawTexture2D(props, currGameCamID);

    }

    //Text
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Text>())
    {
        if (!element.GetComponent<Transform>()->is_active) continue;

        const auto const textComponent = element.GetComponent<Text>();

        Renderer2DText sample;
        sample.m_window_size = Vector2(static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()), static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight()));
        sample.m_words = FLX_STRING_GET(textComponent->text);
        sample.m_color = textComponent->color;
        sample.m_fonttype = FLX_STRING_GET(textComponent->fonttype);
        //TODO: Need to convert text to similar to camera class
        //Temp
        sample.m_transform = Matrix4x4(element.GetComponent<Scale>()->scale.x, 0.00, 0.00, 0.00,
                                       0.00, element.GetComponent<Scale>()->scale.y, 0.00, 0.00,
                                       0.00, 0.00, element.GetComponent<Scale>()->scale.z, 0.00,
                                       element.GetComponent<Position>()->position.x, element.GetComponent<Position>()->position.y, element.GetComponent<Position>()->position.z, 1.00);
        sample.m_alignment = std::pair{ static_cast<Renderer2DText::AlignmentX>(textComponent->alignment.first), static_cast<Renderer2DText::AlignmentY>(textComponent->alignment.second)};
        sample.m_textboxDimensions = textComponent->textboxDimensions;

        OpenGLRenderer::DrawTexture2D(sample, currGameCamID);
    }
  }

  void Stop() override
  {
    
  }
};

// Static instance to ensure registration
static RenderLoopScript RenderLoop;
