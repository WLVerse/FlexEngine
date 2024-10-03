#include "overworld.h"

#include "States.h"
#include "Layers.h"

#include "Components/battlecomponents.h"
#include "Components/physics.h"
#include "Components/rendering.h"
#include "Components/characterinput.h"

#include "BattleSystems/physicssystem.h"

#include "Renderer/sprite2d.h"


namespace ChronoShift {
    float OverworldLayer::m_ScaleDebugTest = 0.8f;
    Vector3 OverworldLayer::m_RotateDebugTest = Vector3(0.f, 0.f, 0.f);

    void OverworldLayer::SetupWorld()
    {
        auto scene = FlexECS::Scene::GetActiveScene();

        //@anyone, liase with me(wei jie) for if you want the tilt of the background to be more slanted or anything you wish
        //Tried to just simply rotate on x-axis of a wrapped vbo but kenah the orthographic camera cut off
        //so no choice do lame method
        FlexECS::Entity background = FlexECS::Scene::CreateEntity("bg");
        background.AddComponent<IsActive>({ true });
        background.AddComponent<Position>({ {650, 600} });
        background.AddComponent<Scale>({ { 2500,1500 } });
        background.AddComponent<Transform>({});
        background.AddComponent<ZIndex>({ 0 });
        background.AddComponent<Sprite>({
            scene->Internal_StringStorage_New(R"(\images\misc\wireframe_darkbg.png)"),
            Vector3::One,
            Vector3::Zero,
            Vector3::One,
            Renderer2DProps::Alignment_Center,
            Renderer2DProps::VBO_Basic
           });
        background.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

        FlexECS::Entity player1 = FlexECS::Scene::CreateEntity("player1");
        player1.AddComponent<CharacterInput>({ });
        player1.AddComponent<Rigidbody>({ {}, false });
        player1.AddComponent<BoundingBox2D>({ });
        player1.AddComponent<IsActive>({ true });
        player1.AddComponent<Position>({ {200, 600} });
        player1.AddComponent<Rotation>({ });
        player1.AddComponent<Scale>({ { 100,100 } });
        player1.AddComponent<Transform>({});
        player1.AddComponent<ZIndex>({ 10 });
        player1.AddComponent<Sprite>({
          scene->Internal_StringStorage_New(R"(\images\chess_queen.png)"),
          //scene->Internal_StringStorage_New(R"()"),
          Vector3::One,
          Vector3::Zero,
          Vector3::One,
          Renderer2DProps::Alignment_Center
         });
        player1.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });


        FlexECS::Entity house = FlexECS::Scene::CreateEntity("house");
        house.AddComponent<Rigidbody>({ {}, true });
        house.AddComponent<BoundingBox2D>({ });
        house.AddComponent<IsActive>({ true });
        house.AddComponent<Position>({ {800, 500 } });
        house.AddComponent<Rotation>({ });
        house.AddComponent<Scale>({ { 250,250 } });
        house.AddComponent<Transform>({});
        house.AddComponent<ZIndex>({ 10 });
        house.AddComponent<Sprite>({
            scene->Internal_StringStorage_New(R"()"),
            { 0.45f, 0.58f, 0.32f },
            Vector3::Zero,
            Vector3::One,
            Renderer2DProps::Alignment_Center
           });
        house.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

        FlexECS::Entity box = FlexECS::Scene::CreateEntity("box");
        box.AddComponent<Rigidbody>({ {}, false });
        box.AddComponent<BoundingBox2D>({ });
        box.AddComponent<IsActive>({ true });
        box.AddComponent<Position>({ {350, 500 } });
        box.AddComponent<Scale>({ { 150,150 } });
        box.AddComponent<Transform>({});
        box.AddComponent<ZIndex>({ 10 });
        box.AddComponent<Sprite>({
            scene->Internal_StringStorage_New(R"()"),
            { 0.35f, 0.58f, 0.80f },
            Vector3::Zero,
            Vector3::One,
            Renderer2DProps::Alignment_Center
           });
        box.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

        //Parent Test
        {
            FlexECS::Entity box2 = FlexECS::Scene::CreateEntity("box2");
            box2.AddComponent<IsActive>({ true });
            box2.AddComponent<Position>({ {1, 0 } });
            box2.AddComponent<Rotation>({ {0,0,20} });
            box2.AddComponent<Scale>({ { 0.9f, 0.9f } });
            box2.AddComponent<Transform>({});
            box2.AddComponent<ZIndex>({ 10 });
            box2.AddComponent<Sprite>({
                scene->Internal_StringStorage_New(R"()"),
                { 0.35f, 0.08f, 1.80f },
                Vector3::Zero,
                Vector3::One,
                Renderer2DProps::Alignment_Center
               });
            box2.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
            box2.AddComponent<Parent>({ box });

            FlexECS::Entity box3 = FlexECS::Scene::CreateEntity("box3");
            box3.AddComponent<IsActive>({ true });
            box3.AddComponent<Position>({ {1, 0 } });
            box3.AddComponent<Rotation>({ {0,0,20} });
            box3.AddComponent<Scale>({ { 0.9f, 0.9f } });
            box3.AddComponent<Transform>({});
            box3.AddComponent<ZIndex>({ 10 });
            box3.AddComponent<Sprite>({
                scene->Internal_StringStorage_New(R"()"),
                { 1.35f, 0.08f, 0.80f },
                Vector3::Zero,
                Vector3::One,
                Renderer2DProps::Alignment_Center
               });
            box3.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
            box3.AddComponent<Parent>({ box2 });

            FlexECS::Entity box4 = FlexECS::Scene::CreateEntity("box4");
            box4.AddComponent<IsActive>({ true });
            box4.AddComponent<Position>({ {1, 0 } });
            box4.AddComponent<Rotation>({ {0,0,20} });
            box4.AddComponent<Scale>({ { 0.9f, 0.9f } });
            box4.AddComponent<Transform>({});
            box4.AddComponent<ZIndex>({ 10 });
            box4.AddComponent<Sprite>({
                scene->Internal_StringStorage_New(R"()"),
                { 0.1f, 0.08f, 0.80f },
                Vector3::Zero,
                Vector3::One,
                Renderer2DProps::Alignment_Center
               });
            box4.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
            box4.AddComponent<Parent>({ box3 });

            FlexECS::Entity box5 = FlexECS::Scene::CreateEntity("box5");
            box5.AddComponent<IsActive>({ true });
            box5.AddComponent<Position>({ {1, 0 } });
            box5.AddComponent<Rotation>({ {0,0,20} });
            box5.AddComponent<Scale>({ { 0.9f, 0.9f } });
            box5.AddComponent<Transform>({});
            box5.AddComponent<ZIndex>({ 10 });
            box5.AddComponent<Sprite>({
                scene->Internal_StringStorage_New(R"()"),
                { 0.2f, 0.78f, 0.30f },
                Vector3::Zero,
                Vector3::One,
                Renderer2DProps::Alignment_Center
               });
            box5.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
            box5.AddComponent<Parent>({ box4 });

            FlexECS::Entity box6 = FlexECS::Scene::CreateEntity("box6");
            box6.AddComponent<IsActive>({ true });
            box6.AddComponent<Position>({ {1, 0 } });
            box6.AddComponent<Rotation>({ {0,0,20} });
            box6.AddComponent<Scale>({ { 0.9f, 0.9f } });
            box6.AddComponent<Transform>({});
            box6.AddComponent<ZIndex>({ 10 });
            box6.AddComponent<Sprite>({
                scene->Internal_StringStorage_New(R"()"),
                { 1.2f, 0.78f, 0.30f },
                Vector3::Zero,
                Vector3::One,
                Renderer2DProps::Alignment_Center
               });
            box6.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
            box6.AddComponent<Parent>({ box5 });

            FlexECS::Entity box7 = FlexECS::Scene::CreateEntity("box7");
            box7.AddComponent<IsActive>({ true });
            box7.AddComponent<Position>({ {1, 0 } });
            box7.AddComponent<Rotation>({ {0,0,20} });
            box7.AddComponent<Scale>({ { 0.9f, 0.9f } });
            box7.AddComponent<Transform>({});
            box7.AddComponent<ZIndex>({ 10 });
            box7.AddComponent<Sprite>({
                scene->Internal_StringStorage_New(R"()"),
                { 1.2f, 0.78f, 1.30f },
                Vector3::Zero,
                Vector3::One,
                Renderer2DProps::Alignment_Center
               });
            box7.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
            box7.AddComponent<Parent>({ box6 });
        }
  }



  void OverworldLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();

    // ECS Setup
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);

    RegisterRenderingComponents();

    SetupWorld();
  }

  void OverworldLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
  }

  void OverworldLayer::Update()
  {
    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<CharacterInput>())
    {
      entity.GetComponent<CharacterInput>()->up = Input::GetKey(GLFW_KEY_W);
      entity.GetComponent<CharacterInput>()->down = Input::GetKey(GLFW_KEY_S);
      entity.GetComponent<CharacterInput>()->left = Input::GetKey(GLFW_KEY_A);
      entity.GetComponent<CharacterInput>()->right = Input::GetKey(GLFW_KEY_D);
    }

    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<CharacterInput, Rigidbody>())
    {
      auto& velocity = entity.GetComponent<Rigidbody>()->velocity;
      velocity.x = 0.0f;
      velocity.y = 0.0f;

      if (entity.GetComponent<CharacterInput>()->up)
      {
        velocity.y = -300.0f;
      }

      if (entity.GetComponent<CharacterInput>()->down)
      {
        velocity.y = 300.0f;
      }

      if (entity.GetComponent<CharacterInput>()->left)
      {
        velocity.x = -300.0f;
      }

      if (entity.GetComponent<CharacterInput>()->right)
      {
        velocity.x = 300.0f;
      }
    }

    //For testing 2500 objects
    //Create one, then clone the rest
    if (Input::GetKeyDown(GLFW_KEY_0))
    {
      auto scene = FlexECS::Scene::GetActiveScene();
      for (auto& entity : FlexECS::Scene::GetActiveScene()->View<EntityName>()) 
      {
        scene->DestroyEntity(entity);
      }

      FlexECS::Entity thing = FlexECS::Scene::CreateEntity("White Queen");
      thing.AddComponent<IsActive>({ true });
      thing.AddComponent<Position>({ {0,0} });
      thing.AddComponent<Rotation>({ });
      thing.AddComponent<Scale>({ { 15,15 } });
      thing.AddComponent<ZIndex>({ 10 });
      thing.AddComponent<Transform>({ });
      thing.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(R"(\images\chess_queen.png)"),
        Vector3::One,
        Vector3::Zero,
        Vector3::One,
        Renderer2DProps::Alignment_Center
       });
      thing.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });

      for (size_t x = 0; x < 50; x++)
      {
        for (size_t y = 0; y < 50; y++)
        {
          FlexECS::Entity cloned_thing = scene->CloneEntity(thing);
          auto& position = cloned_thing.GetComponent<Position>()->position;
          position.x = static_cast<float>(15 * (x + 1));
          position.y = static_cast<float>(15 * (y + 1));
        }
      }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Debug Tests
//Key hold (Can just alter here, not very elegant but will do for now)
    #if 1 //DEBUG
    if (Input::GetKey(GLFW_KEY_F))
    {
        m_ScaleDebugTest -= 0.008f;
    }
    else if (Input::GetKey(GLFW_KEY_G))
    {
        m_ScaleDebugTest += 0.008f;
    }

    if (Input::GetKey(GLFW_KEY_Q))
    {
        m_RotateDebugTest.z += 1.0f;
    }
    else if (Input::GetKey(GLFW_KEY_E))
    {
        m_RotateDebugTest.z -= 1.0f;
    }

    //Altering entities scale and rotation while game is in debug mode
    // TEST ON EVERYTHING
    for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, Transform>())
    {
        if (!entity.GetComponent<IsActive>()->is_active) continue;

        //Search function for a specific object to test and NOT everything
        auto entity_name_component = entity.GetComponent<EntityName>();
        //Change "" to whatever object or comment the line to affect everything
        if ("box2" != FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity_name_component)) continue;

        auto& scale = entity.GetComponent<Scale>()->scale;
        auto& rotation = entity.GetComponent<Rotation>()->rotation;

        scale = Vector2(m_ScaleDebugTest, m_ScaleDebugTest);
        rotation = m_RotateDebugTest;

        entity.GetComponent<Transform>()->is_dirty = true;
    }
    #endif
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

    UpdatePhysicsSystem();

    //Update Transformation Matrix of All Entities
    UpdateSprite2DMatrix();

    //Render All Entities
    RendererSprite2D();
  }
}
