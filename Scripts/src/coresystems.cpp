#include <FlexEngine.h>
#include "FlexEngine/Physics/physicssystem.h"

using namespace FlexEngine;

class CoreSystemsScript : public IScript
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
    static Camera camera(0.0f, 1600.0f, 900.0f, 0.0f, -2.0f, 2.0f);

    // Physics and Graphics excluded
    
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

    OpenGLRenderer::DrawTexture2D(camera, sample);
  }

  void Stop() override
  {
    
  }
};

// Static instance to ensure registration
static CoreSystemsScript GameplayLoops;
