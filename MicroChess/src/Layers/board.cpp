#include "board.h"

#include "States.h"
#include "Layers.h"

#include "Components/game.h"
#include "Components/physics.h"
#include "Components/rendering.h"

#include "Physics/box2d.h"
#include "Renderer/sprite2d.h"

namespace MicroChess
{

  void BoardLayer::SetupBoard()
  {
    auto scene = FlexECS::Scene::GetActiveScene();

    // constants

    const Vector3 color_board_white = { 0.92f, 0.93f, 0.82f };
    const Vector3 color_board_black = { 0.45f, 0.58f, 0.32f };
    Window* window = Application::GetCurrentWindow();

    // create entities

    FlexECS::Entity board = FlexECS::Scene::CreateEntity("Board");
    board.AddComponent<IsActive>({ true });
    board.AddComponent<Position>({ {
      (static_cast<float>(window->GetWidth ()) * 0.5f) - (4 * 0.5f * 100.f) + (100.f * 0.5f),
      (static_cast<float>(window->GetHeight()) * 0.5f) - (4 * 0.5f * 100.f) + (100.f * 0.5f)
    } });
    board.AddComponent<Scale>({ { 1, 1 } });

    for (std::size_t i = 0; i < 4; i++) // rows -
    {
      for (std::size_t j = 0; j < 4; j++) // columns |
      {
        std::string name = "Tile" + std::to_string(i * 4 + j);
        FlexECS::Entity tile = FlexECS::Scene::CreateEntity(name);
        tile.AddComponent<IsActive>({ true });
        tile.AddComponent<ZIndex>({ 10 });
        tile.AddComponent<BoardTile>({});
        tile.AddComponent<Parent>({ board });
        tile.AddComponent<Position>({ { j * 100.0f, i * 100.0f } });
        tile.AddComponent<Scale>({ { 100, 100 } });
        tile.AddComponent<Sprite>({
          //scene->Internal_StringStorage_New(R"(\images\chess_king.png)"),
          scene->Internal_StringStorage_New(R"()"),
          ((i + j) % 2 ? color_board_black : color_board_white),
          Vector3::Zero,
          Vector3::One,
          Renderer2DProps::Alignment_Center
        });
        tile.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
      }
    }

    std::array<PieceTypes, 16> pieces = {
      PIECETYPE_ROOK, PIECETYPE_QUEEN, PIECETYPE_KING, PIECETYPE_ROOK,
      PIECETYPE_PAWN, PIECETYPE_PAWN,  PIECETYPE_PAWN, PIECETYPE_PAWN,
      PIECETYPE_PAWN, PIECETYPE_PAWN,  PIECETYPE_PAWN, PIECETYPE_PAWN,
      PIECETYPE_ROOK, PIECETYPE_QUEEN, PIECETYPE_KING, PIECETYPE_ROOK,
    };
    std::array<std::string, 16> piece_asset = {
      R"(\images\chess_rook.png)", R"(\images\chess_queen.png)", R"(\images\chess_king.png)", R"(\images\chess_rook.png)",
      R"(\images\chess_pawn.png)", R"(\images\chess_pawn.png)",  R"(\images\chess_pawn.png)", R"(\images\chess_pawn.png)",
      R"(\images\chess_pawn.png)", R"(\images\chess_pawn.png)",  R"(\images\chess_pawn.png)", R"(\images\chess_pawn.png)",
      R"(\images\chess_rook.png)", R"(\images\chess_queen.png)", R"(\images\chess_king.png)", R"(\images\chess_rook.png)",
    };

    for (std::size_t i = 0; i < 16; i++)
    {
      std::string name = "Piece" + std::to_string(i);
      FlexECS::Entity piece = FlexECS::Scene::CreateEntity(name);
      piece.AddComponent<IsActive>({ true });
      piece.AddComponent<ZIndex>({ 20 });
      piece.AddComponent<IsWhite>({ (i >= 8) });
      piece.AddComponent<PieceType>({ pieces[i] });
      piece.AddComponent<PieceStatus>({});
      piece.AddComponent<Parent>({ board });
      //Vector2 pos = { static_cast<int>(i / 4), i % 4 };
      //piece.AddComponent<PiecePosition>({ pos });
      piece.AddComponent<Position>({ { (i % 4) * 100.0f, (i / 4) * 100.0f } });
      piece.AddComponent<Scale>({ { 50, 50 } });
      piece.AddComponent<Sprite>({
        scene->Internal_StringStorage_New(piece_asset[i]),
        Vector3::One,
        Vector3::Zero,
        (i >= 8) ? Vector3::One : Vector3::Zero,
        Renderer2DProps::Alignment_Center
      });
      piece.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
      piece.AddComponent<BoundingBox2D>({ Vector2::One });
      piece.AddComponent<OnHover>({});
      piece.AddComponent<OnClick>({});
    }

  }

  void BoardLayer::OnAttach()
  {
    FLX_FLOW_BEGINSCOPE();

    // ECS Setup
    auto scene = FlexECS::Scene::CreateScene();
    FlexECS::Scene::SetActiveScene(scene);

    // Custom cursor
    // IsActive, ZIndex, Parent, Position, Scale, Shader, Sprite
    FlexECS::Entity cursor = FlexECS::Scene::CreateEntity("Custom Cursor");
    cursor.AddComponent<CustomCursor>({ CURSORTYPE_DEFAULT });
    cursor.AddComponent<IsActive>({ true });
    cursor.AddComponent<ZIndex>({ 100 });
    cursor.AddComponent<Position>({ { 0, 0 } });
    cursor.AddComponent<Scale>({ { 32, 32 } });
    cursor.AddComponent<Shader>({ scene->Internal_StringStorage_New(R"(\shaders\texture)") });
    cursor.AddComponent<Sprite>({
      scene->Internal_StringStorage_New(R"(\images\cursor\cursor_none.png)"),
      Vector3::One,
      Vector3::Zero,
      Vector3::One,
      Renderer2DProps::Alignment_Center
    });

    // Setup the board
    SetupBoard();
  }

  void BoardLayer::OnDetach()
  {
    FLX_FLOW_ENDSCOPE();
  }

  void BoardLayer::Update()
  {
    // display a custom cursor
    for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<IsActive, CustomCursor, Position, Sprite>())
    {
      auto& is_active = entity.GetComponent<IsActive>()->is_active;
      auto& cursor = entity.GetComponent<CustomCursor>()->type;
      auto& position = entity.GetComponent<Position>()->position;
      auto& texture = entity.GetComponent<Sprite>()->texture;
      auto window = Application::GetCurrentWindow();

      if (
        Input::GetKey(GLFW_KEY_LEFT_ALT) || Input::GetKey(GLFW_KEY_RIGHT_ALT) ||  // override key
        !window->IsFocused() ||                                                   // window not in focus
        ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)                       // imgui window override
      )
      {
        cursor = CURSORTYPE_NONE;
      }
      else
      {
        cursor = CURSORTYPE_DEFAULT;
      }

      if (cursor == CURSORTYPE_NONE)
      {
        is_active = false;
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
        continue;
      }
      else
      {
        is_active = true;
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
      }

      std::string texture_path = R"(\images\cursor\cursor_none.png)";

      switch (cursor)
      {
      case CURSORTYPE_DEFAULT: texture_path = R"(\images\cursor\cursor_none.png)"; break;
      case CURSORTYPE_POINTER: texture_path = R"(\images\cursor\hand_point.png)"; break;
      case CURSORTYPE_DISABLED: texture_path = R"(\images\cursor\disabled.png)"; break;
      case CURSORTYPE_DRAG_HOVER: texture_path = R"(\images\cursor\hand_open.png)"; break;
      case CURSORTYPE_DRAG_CLICK: texture_path = R"(\images\cursor\hand_closed.png)"; break;
      case CURSORTYPE_CROSS: texture_path = R"(\images\cursor\cross_small.png)"; break;
      case CURSORTYPE_BUSY: texture_path = R"(\images\cursor\busy_circle_fade.png)"; break;
      case CURSORTYPE_HELP: texture_path = R"(\images\cursor\cursor_help.png)"; break;
      default: break;
      }

      texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(texture_path);

      position = Input::GetCursorPosition() + Vector2(7, 12);
    }

    Box2D();

    // make the piece bigger when hovered
    for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<IsActive, OnHover, Scale>())
    {
      if (!entity.GetComponent<IsActive>()->is_active) continue;
    
      auto on_hover = entity.GetComponent<OnHover>();
      auto& scale = entity.GetComponent<Scale>()->scale;
    
      if (on_hover->on_enter) scale *= Vector2(1.5f, 1.5f);
      if (on_hover->on_exit) scale /= Vector2(1.5f, 1.5f);
    }

    // remove the piece when clicked
    FunctionQueue destroy_queue;
    for (auto& entity : FlexECS::Scene::GetActiveScene()->Query<IsActive, OnClick>())
    {
      if (!entity.GetComponent<IsActive>()->is_active) continue;
    
      auto on_click = entity.GetComponent<OnClick>();
    
      if (on_click->is_clicked)
      {
        destroy_queue.Insert({ [entity]() { FlexECS::Scene::GetActiveScene()->DestroyEntity(entity); }, "", 0 });
      }
    }
    destroy_queue.Flush();

    RendererSprite2D();
  }

}
