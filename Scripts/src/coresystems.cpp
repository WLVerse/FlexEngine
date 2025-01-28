#include <FlexEngine.h>
#include "FlexEngine/Physics/physicssystem.h"
#include "camerahandler.cpp"

using namespace FlexEngine;

class CoreSystemsScript : public Script
{
public:
  CoreSystemsScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "GameplayLoops"; }

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
    FlexECS::EntityID currGameCamID = 0;
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<ScriptComponent>())
    {
        if (FLX_STRING_GET(element.GetComponent<ScriptComponent>()->script_name) == "CameraHandler")
            currGameCamID = dynamic_cast<CameraHandler*>(ScriptRegistry::GetScript(FLX_STRING_GET(element.GetComponent<ScriptComponent>()->script_name)))->GetMainGameCameraID();
    }
    // Transform
    
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

      OpenGLRenderer::DrawTexture2D(props, currGameCamID);
    }

    // Audio
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<Audio>())
    {
      if (!element.GetComponent<Transform>()->is_active) continue; // skip non active entities

      FlexEngine::Audio* audio = element.GetComponent<Audio>();
      if (audio->should_play)
      {
        if (audio->audio_file == FLX_STRING_NULL) // or you can use if (!audio->audio_file)
        {
          Log::Warning("Audio not attached to entity: " + FLX_STRING_GET(*element.GetComponent<EntityName>()));
          audio->should_play = false;
          continue;
        }

        if (audio->is_looping)
        {
          FMODWrapper::Core::PlayLoopingSound(FLX_STRING_GET(*element.GetComponent<EntityName>()),
                                              FLX_ASSET_GET(Asset::Sound, AssetKey{ FLX_STRING_GET(element.GetComponent<Audio>()->audio_file) }));
        }
        else
        {
          FMODWrapper::Core::PlaySound("test", FLX_ASSET_GET(Asset::Sound, FLX_STRING_GET(element.GetComponent<Audio>()->audio_file)));
        }

        element.GetComponent<Audio>()->should_play = false;
      }
    }

    // Scripting update loop, other calls are managed separatedly
    for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<ScriptComponent>())
    {
      if (!element.GetComponent<Transform>()->is_active) continue; // skip non active entities

      Script* script = ScriptRegistry::GetScript(
        FLX_STRING_GET(element.GetComponent<ScriptComponent>()->script_name));
      if (script)
      {
        script->Update();
      }
    }

    static std::string extra = "";
    if (Input::GetKey(GLFW_KEY_J))
      extra += "j";
    Renderer2DText sample;
    static std::string fullText = "The whole human fraternity is becoming highly dependent on the computer technology; no one can imagine life without computer. As, it has spread its wings so deeply in every area and made people used of it. It is very beneficial for the students of any class. They can use it to prepare their projects, learn poems, read different stories, download notes for exam preparations, collect large information within seconds, learn about painting, drawing, etc. However it enhances the professional skills of the students and helps in getting job easily.";
    static std::string displayedText = ""; // Start with an empty string
    static float elapsedTime = 0.0f;       // To track time
    elapsedTime += Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 100;
    if (elapsedTime >= 1.0f && displayedText.size() < fullText.size()) {
      displayedText += fullText[displayedText.size()]; // Append the next character
      elapsedTime = 0.0f; // Reset the timer
    }
    sample.m_words = displayedText;
    //sample.m_words = "hello there my name is markiplier and welcome back to another game of amnesia the dark descent" + extra;
    sample.m_color = Vector3::Zero;
    sample.m_fonttype = R"(/fonts/Bangers/Bangers-Regular.ttf)";
    sample.m_transform = Matrix4x4(1.00, 0.00, 0.00, 0.00,
        0.00, 1.00, 0.00, 0.00,
        0.00, 0.00, 1.00, 0.00,
         822.00, 248.00, 0.00, 1.00);
    sample.m_window_size = Vector2(static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()), static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight()));
    //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Top };
    //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Middle };
    //sample.m_alignment = { Renderer2DText::Alignment_Left,Renderer2DText::Alignment_Bottom };
    //sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Top };
    sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle };
    //sample.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Bottom };
    //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Top };
    //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Middle };
    //sample.m_alignment = { Renderer2DText::Alignment_Right,Renderer2DText::Alignment_Bottom };
    sample.m_maxwidthtextbox = 850.0f;

    OpenGLRenderer::DrawTexture2D(sample, currGameCamID);
  }

  void Stop() override
  {
    
  }
};

// Static instance to ensure registration
static CoreSystemsScript GameplayLoops;
