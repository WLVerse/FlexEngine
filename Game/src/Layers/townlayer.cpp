// WLVERSE [https://wlverse.web.app]
// townlayer.cpp
//
// CPP file for town layer where user can control main character to walk around the town scene
//
// AUTHORS
// [80%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
// [20%] Ng Jia Cheng (n.jiacheng\@digipen.edu)
//   - Co Author
// Copyright (c) 2025 DigiPen, All rights reserved.
#include "Layers.h"
#include "Physics/physicssystem.h"

#include "FMOD/FMODWrapper.h"

namespace Game
{
  extern std::string town_version;

  FlexECS::Entity town_dialogue_textbox;
  //FlexECS::Entity town_press_button;
  FlexECS::Entity town_dialogue_text;
  //FlexECS::Entity character_icon;
  bool dialogue_start = false;
  int dialogue_info = 0;
  std::string dialogue = "";
  int person_talking = 2;

  // Pause Buttons
  std::string active_pause_button;
  std::string active_volume_button;
  bool is_paused = false;
  Vector3 original_camera_pos;

  void Set_Up_Town_Pause_Menu() {
    #pragma region Load pause menu data
    std::array<std::string, 9> slider_names = {
      "SFX Slider Background",
      "SFX Slider Fill",
      "SFX Knob",
      "BGM Slider Background",
      "BGM Slider Fill",
      "BGM Knob",
      "Master Slider Background",
      "Master Slider Fill",
      "Master Knob"
    };
    for (int i = 0; i < 9; i += 3) {
      FlexECS::Entity tempEntity = FlexECS::Scene::GetEntityByName(slider_names[i]);

      float min_value = tempEntity.GetComponent<Position>()->position.x - tempEntity.GetComponent<Sprite>()->scale.x - 20.f;
      float max_value = tempEntity.GetComponent<Position>()->position.x + tempEntity.GetComponent<Sprite>()->scale.x + 15.f;

      FlexECS::Entity sliderEntity = FlexECS::Scene::GetEntityByName(slider_names[i + 1]);
      sliderEntity.GetComponent<Slider>()->min_position = min_value;
      sliderEntity.GetComponent<Slider>()->max_position = max_value;
      sliderEntity.GetComponent<Slider>()->slider_length = max_value - min_value;
      sliderEntity.GetComponent<Slider>()->original_value = ((FlexECS::Scene::GetEntityByName(slider_names[i + 2]).GetComponent<Position>()->position.x - min_value)) / (max_value - min_value);

      // Set Up Volume Positions
      if (i == 0) {
        FlexECS::Scene::GetEntityByName("SFX Knob").GetComponent<Position>()->position.x =
          FMODWrapper::Core::GetGroupVolume(FMODWrapper::Core::CHANNELGROUP::SFX) * (max_value - min_value) + min_value;
      }
      else if (i == 3) {
        FlexECS::Scene::GetEntityByName("BGM Knob").GetComponent<Position>()->position.x =
          FMODWrapper::Core::GetGroupVolume(FMODWrapper::Core::CHANNELGROUP::BGM) * (max_value - min_value) + min_value;
      }
      else if (i == 6) {
        FlexECS::Scene::GetEntityByName("Master Knob").GetComponent<Position>()->position.x = max_value;
      }
    }
    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PauseUI, Slider>()) {
      entity.GetComponent<Slider>()->original_scale = entity.GetComponent<Scale>()->scale;
    }
    if (Application::GetCurrentWindow()->GetFullScreen()) {
      FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Display Mode Value").GetComponent<Text>()->text) = "Fullscreen";
    }
    else FLX_STRING_GET(FlexECS::Scene::GetEntityByName("Display Mode Value").GetComponent<Text>()->text) = "Windowed";
    #pragma endregion
  }

  void Town_Pause_Functionality() {
    is_paused ^= true;
    FlexECS::Entity cam = CameraManager::GetMainGameCameraID();

    if (is_paused) {
      cam.GetComponent<Position>()->position = original_camera_pos;
      active_pause_button = "Resume Button Sprite";
      active_volume_button = "Master Volume Sprite";
    }
    else cam.GetComponent<Position>()->position = FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position;

    for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI>()) {
      bool& state_to_set = entity.GetComponent<Transform>()->is_active;
      if (entity.HasComponent<PauseHoverUI>() || entity.HasComponent<CreditsUI>() || entity.HasComponent<QuitUI>()) state_to_set = false;
      else state_to_set ^= true;
    }

    if (is_paused) FlexECS::Scene::GetEntityByName("Resume Button Sprite").GetComponent<Transform>()->is_active = true;
  }

  void TownLayer::OnAttach()
  {
    //File& file = File::Open(Path::current("assets/saves/town_v4.flxscene"));
    File& file = File::Open(Path::current(town_version));
    FlexECS::Scene::SetActiveScene(FlexECS::Scene::Load(file));

    // Trigger music to start
    FlexECS::Scene::GetEntityByName("Town BGM").GetComponent<Audio>()->should_play = true;

    town_dialogue_textbox = FlexECS::Scene::CreateEntity("town_dialogue_textbox"); // can always use GetEntityByName to find the entity
    town_dialogue_textbox.AddComponent<Transform>({});
    town_dialogue_textbox.AddComponent<Position>({ Vector3(-1095, -500, 0) });
    town_dialogue_textbox.AddComponent<Rotation>({});
    town_dialogue_textbox.AddComponent<Scale>({ Vector3(0.55f, 0.55f, 0) });
    town_dialogue_textbox.AddComponent<ZIndex>({ 15 });
    town_dialogue_textbox.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/Battle_UI_DialogueBox.png)") });

    /**town_press_button = FlexECS::Scene::CreateEntity("town_press_button"); // can always use GetEntityByName to find the entity
    town_press_button.AddComponent<Transform>({});
    town_press_button.AddComponent<Position>({ Vector3(-1100, -530, 0) });
    town_press_button.AddComponent<Rotation>({});
    town_press_button.AddComponent<Scale>({ Vector3(0.9f, 0.75f, 0) });
    town_press_button.AddComponent<ZIndex>({ 500 });
    town_press_button.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_Win_Text_Press Any Button To Continue.png)") });
    */
    town_dialogue_text = FlexECS::Scene::CreateEntity("town_dialogue_text"); // can always use GetEntityByName to find the entity
    town_dialogue_text.AddComponent<Transform>({});
    town_dialogue_text.AddComponent<Position>({ Vector3(-1100, -510, 0) });
    town_dialogue_text.AddComponent<Rotation>({});  
    town_dialogue_text.AddComponent<Scale>({ Vector3(0.4f, 0.4f, 0) });
    town_dialogue_text.AddComponent<ZIndex>({ 16 });
    town_dialogue_text.AddComponent<Text>({
      FLX_STRING_NEW(R"(/fonts/Electrolize/Electrolize-Regular.ttf)"),
      FLX_STRING_NEW(R"(Grace: We're here. We need to find and stop the Drifter responsible for destroying the timeline. Use W, A, S, D to explore the area. I'll follow your lead.)"),
      Vector3(1.0f, 1.0, 1.0f),
      { Renderer2DText::Alignment_Center, Renderer2DText::Alignment_Center },
      {                           1400,                              320 }
    });
    /*
    character_icon = FlexECS::Scene::CreateEntity("dialogue_icon"); // can always use GetEntityByName to find the entity
    character_icon.AddComponent<Transform>({});
    character_icon.AddComponent<Position>({ Vector3(-1250, -340, 0) });
    character_icon.AddComponent<Rotation>({});
    character_icon.AddComponent<Scale>({ Vector3(2.5f, 2.5f, 0) });
    character_icon.AddComponent<ZIndex>({ 500 });
    character_icon.AddComponent<Sprite>({ FLX_STRING_NEW(R"(/images/battle ui/UI_Win_Text_Press Any Button To Continue.png)") });
    character_icon.AddComponent<Animator>({ FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)") });
    */
    if (town_version == "assets/saves/town_v7_2.flxscene")
    {
      FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position = Vector3(700.0f, -170.0f, 0);
      town_dialogue_textbox.GetComponent<Position>()->position = Vector3(605.0f, 325.0f, 0);
      town_dialogue_text.GetComponent<Position>()->position = Vector3(610.0f, 315.0f, 0);
      dialogue_info = 5;
      if (FlexECS::Scene::GetEntityByName("Jack Collider"))
      {
          
      town_dialogue_textbox.GetComponent<Transform>()->is_active = false;
      town_dialogue_text.GetComponent<Transform>()->is_active = false;
          FlexECS::Scene::DestroyEntity(FlexECS::Scene::GetEntityByName("Jack Collider"));
          FlexECS::Scene::GetEntityByName("Jack").GetComponent<Transform>()->is_active = true;
      }
    }
    FlexECS::Entity camera = CameraManager::GetMainGameCameraID();
    original_camera_pos = camera.GetComponent<Position>()->position;
    camera.GetComponent<Position>()->position = FlexECS::Scene::GetEntityByName("Renko").GetComponent<Position>()->position;

    Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 4, 1.2 });
    
    //if (town_version == "assets/saves/town_v7.flxscene")
    Set_Up_Town_Pause_Menu();
  }

  void TownLayer::OnDetach()
  {
    // Make sure nothing carries over in the way of sound
    FMODWrapper::Core::ForceFadeOut(1.f);
  }

  void TownLayer::Update()
  {
      switch (dialogue_info)
      {
      case 0:
          dialogue = "Grace: We're here. We need to find and stop the Drifter responsible for destroying the timeline. Use W, A, S, D to explore the area. I'll follow your lead.";
          break;
      case 1:
          dialogue = "Renko: Those robots are from the facility... You must be the Drifter Grace was talking about.";
          break;
      case 2:
          dialogue = "Jack: Really? Drifters sent to stop me... It never ceases to amaze me just how much Chrono Drift is committed to ruining everything.";
          break;
      case 3:
          dialogue = "Jack: The robots will take care of you. Consider this a mercy I'm not dealing with you personally.";
          break;
      case 4:
          FlexECS::Scene::GetEntityByName("Jack Anim").GetComponent<Transform>()->is_active = false;
          dialogue = "Grace: He's gone... I'll follow your commands, so let's deal with these robots first, Renko!";
          break;
      case 5:
          dialogue = "Jack: I'm running low on time, but you continue to get in my way...";
          break;
      case 6:
          dialogue = "Jack: Bothersome gnats. It seems I have no choice but to deal with you directly. Don't expect any mercy.";
          break;
          #if 0
      case 0:
          dialogue = "We need to find the Drifter responsible for destroying the timeline, and stop them. Use W, A, S, D to explore the area.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 1:
          dialogue = "Controlling robots from Chrono Drift... Just who are you?!";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 2:
          dialogue = "Who I once was no longer matters. I am Jack the Ripper, a name once feared, now forgotten.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 3:
          dialogue = "All because of you, Grace. You failed to hold a proper leash on the Drifters, who meddled with time and took it too far.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 4:
          dialogue = "What is he talking about...?";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 5:
          dialogue = "Disciplinary Case #21, the prolific serial killer, Jack the Ripper was removed from history by a rogue Drifter who travelled back in time.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 6:
          dialogue = "That Drifter's reckless actions led to the subsequent effects. One, all non-Drifters forgot about the existence of Jack the Ripper.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 7:
          dialogue = "Two, the deviation from the proper course of history caused the non-neglible disappearance of literature drawing from Jack's influence.";
         // character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 8:
          dialogue = "His actions caused an instability to form in the timeline. Not enough to collapse it, but...";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 9:
          dialogue = "That Drifter who went rogue... he wasn't the first. Nor would he be the last.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 10:
          dialogue = "And now, the timeline has finally been pushed to a full-blown collapse. It's too late for you to step in, you'd only get in my way.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 11:
          dialogue = "From the way you're talking, you imply you're not responsible for this. Just what is it then are you trying to achieve?";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 12:
          dialogue = "I will become the heinous murderer humanity feared. And in doing so, I will balance the scales Chrono Drift so callously tipped.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 13:
          dialogue = "Even if you're not responsible for the timeline sabotage, I won't stand by and let you hurt innocent people.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 14:
          dialogue = "He let out a cold, mirthless laugh, as if to mock that motion.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 15:
          dialogue = "You think you're the hero, and I'm the big, bad villain? So be it.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 16:
          dialogue = "I have more important matters to attend to than the likes of you. Take care of them.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 17:
          dialogue = "With that, he issued a command to the robots, which began to surround us.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 18:
          dialogue = "Jack is getting away!";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 19:
          dialogue = "No time for that, Renko. I'll lend you a hand, so let's take care of these robots first.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 20:
          dialogue = "You have some explanation to do, Grace. Why was Jack blaming you for not controlling Drifters...? Just what is the truth behind Chrono Drift?";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 21:
          dialogue = "It all started when the founder disappeared for unknown reasons. Due to my connections with him, I became the Director of Chrono Drift at a young age.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 22:
          dialogue = "Chrono Drift's original purpose was to observe and record history through Drifters.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 23:
          dialogue = "But due to external influence and my own inexperience, Drifters began eventually altering history with their actions.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
            //  FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 24:
          dialogue = "I knew what they were doing was a mistake. But I didn't have the power or influence to openly oppose them. All I could do was sit around and bide my time, trusting my chance would come.";
         // character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 25:
          dialogue = "They went as far as altering the past of people with potential to become Drifters to ensure they would end up joining Chrono Drift.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
            //  FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 26:
          dialogue = "Then it stands to reason a Drifter might hold a grudge against Chrono Drift for doing that.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
             // FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 27:
          dialogue = "No, it doesn't make sense after all. If all Jack wanted to do was maintain the timeline, then he had no need to destroy Chrono Drift.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
            //  FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 28:
          dialogue = "A pain flashes through my head. My repressed memory throbs to the surface. Finally revealing to me what I wanted to keep hidden to myself.";
         // character_icon.GetComponent<Animator>()->spritesheet_handle =
           //   FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 29:
          dialogue = "The one who swore vengeance, the one who destroyed Chrono Drift. The Drifter who was pulled from the past into the present before he could destroy himself with everything.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
            //  FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
      case 30:
          dialogue = "It wasn't Jack, Grace. I... I remember everything now. I am the Drifter who destroyed Chrono Drift. I am the one responsible for the timeline's destruction.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 31:
          dialogue = "This isn't the time for jokes, Renko.";
         // character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 32:
          dialogue = "Upon seeing the look of sunken guilt on my face, her expression slowly turned from disbelief to one that's hard to decipher. And then, she bursted into anger.";
          //character_icon.GetComponent<Animator>()->spritesheet_handle =
              //FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 33:
          dialogue = "You… You-! Do you realize what you'd done? I trusted you! Yet, you've put the whole world at risk for your selfishness!";
         // character_icon.GetComponent<Animator>()->spritesheet_handle =
         //     FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 34:
          dialogue = "I... I wish I could've done that, thrown you all away, used you as collateral. But I couldn't, because I had a responsibility to you all.";
         // character_icon.GetComponent<Animator>()->spritesheet_handle =
         //     FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 35:
          dialogue = "No matter how harsh it was, no matter how much blame I had to shoulder... I had to wait until I could regain full control of Chrono Drift so nobody else had to suffer.";
        //  character_icon.GetComponent<Animator>()->spritesheet_handle =
         //     FLX_STRING_NEW(R"(/images/spritesheets/Char_Grace_Idle_Attack_Anim_Sheet.flxspritesheet)");
          break;
      case 36:
          dialogue = "As if on cue, Jack appeared from the shadows.";
        //  character_icon.GetComponent<Animator>()->spritesheet_handle =
         //     FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 37:
          dialogue = "So it was you, Renko. The last straw that broke the camel's back.";
        //  character_icon.GetComponent<Animator>()->spritesheet_handle =
       //       FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 38:
          dialogue = "This certainly changes things. Originally, I would've left you alive... But if I get rid of you, the timeline's destruction will be stopped.";
         // character_icon.GetComponent<Animator>()->spritesheet_handle =
         //     FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      case 39:
          dialogue = "I hardly relish in senseless killing, but 5 lives for billions... 1 life for 5. It's only natural that you should be the one to perish, Renko. I'll make it painless.";
         // character_icon.GetComponent<Animator>()->spritesheet_handle =
         //     FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;
      default:
          dialogue = "";
        //  character_icon.GetComponent<Transform>()->is_active = false;
        //  character_icon.GetComponent<Animator>()->spritesheet_handle =
            //  FLX_STRING_NEW(R"(/images/spritesheets/Char_Jack_Idle_Anim_Sheet.flxspritesheet)");
          break;

          /*character_icon.GetComponent<Animator>()->should_play = true;
          character_icon.GetComponent<Animator>()->is_looping = true;
          character_icon.GetComponent<Animator>()->return_to_default = false;
          character_icon.GetComponent<Animator>()->frame_time = 0.f;
          character_icon.GetComponent<Animator>()->current_frame = 0;*/
          #endif
      }

    //if (town_version == "assets/saves/town_v7.flxscene") {
    std::pair<std::string, bool> active_pause_sprite = Application::MessagingSystem::Receive<std::pair<std::string, bool>>("Pause Sprite");
    std::pair<std::string, bool> active_volume_sprite = Application::MessagingSystem::Receive<std::pair<std::string, bool>>("Volume Sprite");
    
    bool resume_game = Application::MessagingSystem::Receive<bool>("Resume Game");
    // check for escape key
    if ((Input::GetKeyDown(GLFW_KEY_ESCAPE) && !is_paused) || resume_game)
    {
      FlexECS::Scene::GetEntityByName("ButtonPress").GetComponent<Audio>()->should_play = true;
      Town_Pause_Functionality();
    }
    
    FlexECS::Entity cam = CameraManager::GetMainGameCameraID();

    if (is_paused) {
      cam.GetComponent<Camera>()->SetOrthographic(-1920 / 2.f, 1920 / 2.f, -1080 / 2.f, 1080 / 2.f);

      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>()) {
        if (!entity.GetComponent<Transform>()->is_active) break;
        entity.GetComponent<PostProcessingMarker>()->enableGaussianBlur = true;
      }

      if (!FlexECS::Scene::GetEntityByName("How To Play Background").GetComponent<Transform>()->is_active &&
            !FlexECS::Scene::GetEntityByName("Quit Game Confirmation Prompt").GetComponent<Transform>()->is_active) {
        if (active_pause_sprite.second) {
          FlexECS::Scene::GetEntityByName("ButtonHover").GetComponent<Audio>()->should_play = true;
          FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Transform>()->is_active = false;
          FlexECS::Scene::GetEntityByName(active_pause_sprite.first).GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName(active_pause_sprite.first).GetComponent<Transform>()->is_active = true;
          active_pause_button = active_pause_sprite.first;

          if (active_pause_button != "Settings Button Sprite") {
            for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<Transform, PauseUI, PauseHoverUI, SettingsUI>()) {
              entity.GetComponent<Transform>()->is_active = false;
            }
          }
        }

        if (active_volume_sprite.second) {
          FlexECS::Scene::GetEntityByName("ButtonHover").GetComponent<Audio>()->should_play = true;
          if (active_pause_button != "Settings Button Sprite") {
            FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Transform>()->is_active = false;
            FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Scale>()->scale.x = 0.f;
            FlexECS::Scene::GetEntityByName("Settings Button Sprite").GetComponent<Transform>()->is_active = true;
            active_pause_button = "Settings Button Sprite";
          }
          FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Transform>()->is_active = false;
          FlexECS::Scene::GetEntityByName(active_volume_sprite.first).GetComponent<Scale>()->scale.x = 0.f;
          FlexECS::Scene::GetEntityByName(active_volume_sprite.first).GetComponent<Transform>()->is_active = true;
          active_volume_button = active_volume_sprite.first;
        }
      }

      if (FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Scale>()->scale.x !=
          FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Slider>()->original_scale.x) {
        FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Scale>()->scale.x +=
          Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Scale>()->scale.x =
          std::clamp(FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Scale>()->scale.x,
          0.f, FlexECS::Scene::GetEntityByName(active_pause_button).GetComponent<Slider>()->original_scale.x);
      }

      if (FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Scale>()->scale.x !=
          FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Slider>()->original_scale.x) {
        FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Scale>()->scale.x +=
          Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime() * 10.f;
        FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Scale>()->scale.x =
          std::clamp(FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Scale>()->scale.x,
          0.f, FlexECS::Scene::GetEntityByName(active_volume_button).GetComponent<Slider>()->original_scale.x);
      }
      return;
    }
    else {
      cam.GetComponent<Camera>()->SetOrthographic(-cam.GetComponent<Camera>()->GetOrthoWidth() / 2.f,
      cam.GetComponent<Camera>()->GetOrthoWidth() / 2.f,
      -cam.GetComponent<Camera>()->GetOrthoHeight() / 2.f,
      cam.GetComponent<Camera>()->GetOrthoHeight() / 2.f);
      for (FlexECS::Entity entity : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>()) {
        if (!entity.GetComponent<Transform>()->is_active) break;
        entity.GetComponent<PostProcessingMarker>()->enableGaussianBlur = false;
      }
    }
    
#pragma region Camera Follow System

    // move camera to follow main character
    FlexECS::Entity camera = CameraManager::GetMainGameCameraID();

    FlexECS::Entity main_character;
    main_character = FlexECS::Scene::GetEntityByName("Renko");

    

    // Option 1: Damped Spring Effect ("lagging behind" effect like a spring following the player)
    //float damping = 5.0f; // Higher values = snappier response
    //camera.GetComponent<Position>()->position += (main_character.GetComponent<Position>()->position -
    //  camera.GetComponent<Position>()->position) * damping * Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();

    // Option 2: Lerp (Linear Interpolation) for smooth camera movement over time
    float lerpFactor = 0.1f; // Adjust as needed (higher = faster, lower = smoother)
    camera.GetComponent<Position>()->position.x =
      lerp(camera.GetComponent<Position>()->position.x,
                main_character.GetComponent<Position>()->position.x,
                lerpFactor);
    camera.GetComponent<Position>()->position.y =
      lerp(camera.GetComponent<Position>()->position.y,
                main_character.GetComponent<Position>()->position.y,
                lerpFactor);

    //camera.GetComponent<Position>()->position = main_character.GetComponent<Position>()->position;
    camera.GetComponent<Position>()->position.x = std::clamp(camera.GetComponent<Position>()->position.x, -1080.f, 710.f);
    camera.GetComponent<Position>()->position.y = std::clamp(camera.GetComponent<Position>()->position.y, -275.f, 930.f);

#pragma endregion
#pragma region Scene Transition
   /*if (main_enemy.GetComponent<BoundingBox2D>()->is_colliding)
   {
     // transition lorhhhhhhhh
     Application::MessagingSystem::Send("Enter Boss", true);
   }*/
    static bool startcombat = false;
    if (town_version == "assets/saves/town_v7_2.flxscene")
    {
        
        if (FlexECS::Scene::GetEntityByName("Jack").GetComponent<BoundingBox2D>()->is_colliding && !startcombat)
        {
            // Awful code to delete renko's script so he can't move haha...
            FlexECS::Entity renko = FlexECS::Scene::GetEntityByName("Renko");
            if (renko.HasComponent<Script>())
            {
                renko.RemoveComponent<Script>();
                renko.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)");
            }
            dialogue_start = true;
        }

        if (dialogue_start)
        {
            FLX_STRING_GET(town_dialogue_text.GetComponent<Text>()->text) = dialogue;
            if (dialogue_info < 7)
            {
                //dialogue go brr
                town_dialogue_textbox.GetComponent<Transform>()->is_active = true;
                //town_dialogue_textbox.GetComponent<Position>()->position = Vector3{ 680, 540, 0 };

                town_dialogue_text.GetComponent<Transform>()->is_active = true;
                //town_dialogue_text.GetComponent<Position>()->position = Vector3{ 680, 530, 0 };

                /*town_press_button.GetComponent<Transform>()->is_active = true;
                town_press_button.GetComponent<Position>()->position = Vector3{ 680, 450, 0 };
                
                character_icon.GetComponent<Transform>()->is_active = true;
                if (dialogue_info == 20 || dialogue_info == 26 || dialogue_info == 28)
                {
                    person_talking = 1;
                }
                else if (dialogue_info == 21 || dialogue_info == 27 || dialogue_info == 31)
                {
                    person_talking = 2;
                }
                else if (dialogue_info == 36)
                {
                    FlexECS::Scene::GetEntityByName("Jack").GetComponent<Transform>()->is_active = true;
                    person_talking = 3;
                }
                if (person_talking == 1)
                    character_icon.GetComponent<Position>()->position = Vector3{ 530, 680, 0 };
                else if (person_talking == 2)
                    character_icon.GetComponent<Position>()->position = Vector3{ 530, 640, 0 };
                else character_icon.GetComponent<Position>()->position = Vector3{ 830, 600, 0 };*/
                if (Input::AnyKeyDown())
                {
                    dialogue_info++;
                }
            }
            else
            {
                dialogue_start = false;
                town_dialogue_textbox.GetComponent<Transform>()->is_active = false;
                town_dialogue_text.GetComponent<Transform>()->is_active = false;
               // town_press_button.GetComponent<Transform>()->is_active = false;
               // character_icon.GetComponent<Transform>()->is_active = false;
                Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 3, 1.2 });
                FlexECS::Scene::GetEntityByName("TransitionSound").GetComponent<Audio>()->should_play = true;
                startcombat = true;
            }
        }

        int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
        if (transitionMSG == 3)
        {
            Application::MessagingSystem::Send("Enter Boss", true);
            startcombat = false;
        }
    }
    else
    {
        if (dialogue_info == 0)
        {
            town_dialogue_textbox.GetComponent<Transform>()->is_active = true;
            town_dialogue_text.GetComponent<Transform>()->is_active = true;
            //town_press_button.GetComponent<Transform>()->is_active = true;
            //character_icon.GetComponent<Transform>()->is_active = true;
            if (Input::AnyKeyDown())
            {
                dialogue_info++;
            }
        }
        if (dialogue_info == 1)
        {
            town_dialogue_textbox.GetComponent<Transform>()->is_active = false;
            town_dialogue_text.GetComponent<Transform>()->is_active = false;
            //town_press_button.GetComponent<Transform>()->is_active = false;
            //character_icon.GetComponent<Transform>()->is_active = false;
        }

        if (FlexECS::Scene::GetEntityByName("Encounter1").GetComponent<BoundingBox2D>()->is_colliding && !startcombat)
        {

            // Awful code to delete renko's script so he can't move haha...
            FlexECS::Entity renko = FlexECS::Scene::GetEntityByName("Renko");
            if (renko.HasComponent<Script>())
            {
                renko.RemoveComponent<Script>();
                renko.GetComponent<Animator>()->spritesheet_handle = FLX_STRING_NEW(R"(/images/spritesheets/Char_Renko_Idle_Relaxed_Right_Anim_Sheet.flxspritesheet)");
            }
            dialogue_start = true;
        }

        if (dialogue_start)
        {
            if (dialogue_info < 5)
            {
                town_dialogue_textbox.GetComponent<Position>()->position = Vector3(695.0f, -410.0f, 0);
                town_dialogue_text.GetComponent<Position>()->position = Vector3(700.0f, -420.0f, 0);
                FLX_STRING_GET(town_dialogue_text.GetComponent<Text>()->text) = dialogue;
                town_dialogue_textbox.GetComponent<Transform>()->is_active = true;
                //town_dialogue_textbox.GetComponent<Position>()->position = Vector3{ 680, -420, 0 };

                town_dialogue_text.GetComponent<Transform>()->is_active = true;
                //town_dialogue_text.GetComponent<Position>()->position = Vector3{ 680, -430, 0 };

                /*town_press_button.GetComponent<Transform>()->is_active = true;
                town_press_button.GetComponent<Position>()->position = Vector3{ 680, -410, 0 };

                character_icon.GetComponent<Transform>()->is_active = true;
                if (dialogue_info == 4 || dialogue_info == 13 || dialogue_info == 18)
                {
                    person_talking = 1;
                }
                else if (dialogue_info == 5 || dialogue_info == 11 || dialogue_info == 19)
                {
                    person_talking = 2;
                }
                else if (dialogue_info == 2 || dialogue_info == 9 || dialogue_info == 12 || dialogue_info == 14)
                {
                    person_talking = 3;
                }
                if (person_talking == 1)
                    character_icon.GetComponent<Position>()->position = Vector3{ 530, -180, 0 };
                else if (person_talking == 2)
                    character_icon.GetComponent<Position>()->position = Vector3{ 530, -220, 0 };
                else character_icon.GetComponent<Position>()->position = Vector3{ 830, -260, 0 };*/
                if (Input::AnyKeyDown())
                {
                    dialogue_info++;
                }
            }
            else
            {
                dialogue_start = false;
                town_dialogue_textbox.GetComponent<Transform>()->is_active = false;
                town_dialogue_text.GetComponent<Transform>()->is_active = false;
                //town_press_button.GetComponent<Transform>()->is_active = false;
                //character_icon.GetComponent<Transform>()->is_active = false;
                Application::MessagingSystem::Send("TransitionStart", std::pair<int, double>{ 3, 1.2 });
                FlexECS::Scene::GetEntityByName("TransitionSound").GetComponent<Audio>()->should_play = true;
                startcombat = true;
            }
        }


        int transitionMSG = Application::MessagingSystem::Receive<int>("TransitionCompleted");
        if (transitionMSG == 3)
        {
            Application::MessagingSystem::Send("Enter Battle 1", true);
            startcombat = false;
        }
    }
#pragma endregion

    // Randomly toggle light on and off between range 1 to 2 seconds only in the other scene, but of course when added this can be added to the other scene as well
    if (town_version == "assets/saves/town_v7_2.flxscene")
    {
      static float light_timer = (float)(rand() % 1 + 1);
      light_timer -= Application::GetCurrentWindow()->GetFramerateController().GetDeltaTime();
      if (light_timer <= 0)
      {
        // Lazy toggle, on and off
        FlexECS::Scene::GetEntityByName("Flicker").GetComponent<Transform>()->is_active = 
          !FlexECS::Scene::GetEntityByName("Flicker").GetComponent<Transform>()->is_active;
      
        light_timer = (float)(rand() % 1 + 1);
      }
    }
    
  }
  float TownLayer::lerp(float a, float b, float t) {
    return a + t * (b - a);
  }
} // namespace Game
