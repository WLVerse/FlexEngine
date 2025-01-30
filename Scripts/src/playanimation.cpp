#include <FlexEngine.h>
using namespace FlexEngine;

class PlayAnimationScript : public IScript
{
public:
  PlayAnimationScript() { ScriptRegistry::RegisterScript(this); }
  std::string GetName() const override { return "PlayAnimation"; }

  void Awake() override
  {

  }

  void Start() override
  {
    Log::Info("PlayAnimationScript attached to entity: " + FLX_STRING_GET(*self.GetComponent<EntityName>()) + ". This script will animate sprites.");

    self.GetComponent<Animator>()->should_play = false;
  }

  void Update() override
  {
    #pragma region Display tooltip

    Renderer2DText tooltip;
    tooltip.m_words = "Press any key to play the animation.";
    tooltip.m_color = Vector3::Zero;
    tooltip.m_fonttype = R"(/fonts/Bangers/Bangers-Regular.ttf)";
    tooltip.m_transform = Matrix4x4(
      1.00, 0.00, 0.00, 0.00,
      0.00, 1.00, 0.00, 0.00,
      0.00, 0.00, 1.00, 0.00,
      822.00, 248.00, 0.00, 1.00
    );
    tooltip.m_window_size = Vector2(
      static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()),
      static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight())
    );
    tooltip.m_alignment = { Renderer2DText::Alignment_Center,Renderer2DText::Alignment_Middle };
    tooltip.m_maxwidthtextbox = 850.0f;

    #pragma endregion

    if (Input::AnyKey())
    {
      self.GetComponent<Animator>()->should_play = true;
    }
    else
    {
      self.GetComponent<Animator>()->should_play = false;
    }
  }

  void Stop() override
  {
    self.GetComponent<Animator>()->should_play = true;
  }
};

// Static instance to ensure registration
static PlayAnimationScript PlayAnimation;
