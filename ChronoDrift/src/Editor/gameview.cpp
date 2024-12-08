#include "gameview.h"
#include "editor.h"
#include "editorgui.h"
#include "imguipayloads.h"
#include <FlexEngine/Renderer/OpenGL/openglspriterenderer.h>
#include "windowsizes.h"

namespace ChronoDrift
{
	constexpr float TOP_PADDING = 10.0f;

	void GameView::Init()
	{
	}

	void GameView::Update()
	{

	}
	void GameView::Shutdown()
	{
	}


	void GameView::EditorUI()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav;
		SETGAMEWINDOW
		ImGui::Begin("Game", nullptr, window_flags);
		{
			ImVec2 window_top_left = ImGui::GetWindowPos();
			ImVec2 mouse_pos_ss = ImGui::GetMousePos(); // Screen space mouse pos
			//m_window_position = window_top_left;

			//Panel = entire imgui window
			//Content = exclude title bar
			//Viewport = the image 
			float title_bar_height = ImGui::GetCurrentWindow()->TitleBarHeight();
			ImVec2 panel_size = ImGui::GetWindowSize();
			ImVec2 content_size = { panel_size.x, panel_size.y - title_bar_height };

			//app width and height
			float app_width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth());
			float app_height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());
			float width = app_width;
			float height = app_height;
			float aspect_ratio = width / height;

			//also have a tiny bit of extra padding to ensure the whole image is in frame
			if (height > content_size.y - TOP_PADDING)
			{
				height = content_size.y - TOP_PADDING;
				width = height * aspect_ratio;
			}
			ImVec2 viewport_size(width, height);
			ImVec2 viewport_position((panel_size.x - viewport_size.x) / 2.0f, title_bar_height + TOP_PADDING / 2.0f);

			//m_viewport_size = viewport_size;
			//m_viewport_position = viewport_position;

			ImGui::SetCursorPos(viewport_position);
			//Note: need to invert UVs vertically.
			ImGui::Image((ImTextureID)static_cast<uintptr_t>(FlexEngine::OpenGLFrameBuffer::GetCreatedTexture(FlexEngine::OpenGLFrameBuffer::CreatedTextureID::CID_finalRender)),
				viewport_size, ImVec2(0, 1), ImVec2(1, 0));
    
			if (Editor::GetInstance().GetCamManager().GetMainCamera() != INVALID_ENTITY_ID)//You didn't put a guard @Rocky
			{
				auto cam_data = Editor::GetInstance().GetCamManager().GetCameraData(Editor::GetInstance().GetCamManager().GetMainCamera());

				// Get Mouse position relative to the viewport
				ImVec2 relative_pos = ImVec2(mouse_pos_ss.x - window_top_left.x - viewport_position.x,
																		 mouse_pos_ss.y - window_top_left.y - viewport_position.y); // IMGUI space is screen space - top left of imgui window

				//normalize 0, 1 coords relative to viewport, then scale by app height
				//This is mouse relative and scaled to "game" screen 
				Vector2 screen_pos = { (relative_pos.x / viewport_size.x) * app_width,
															 (relative_pos.y / viewport_size.y) * app_height };


				Vector2 ndc_click_pos = { (2 * screen_pos.x / app_width) - 1, 1 - 2 * screen_pos.y / app_height };
				Matrix4x4 inverse = (cam_data->proj_viewMatrix).Inverse();
				Vector4 clip = { ndc_click_pos.x,
												 ndc_click_pos.y,
												 1.0f,
												 1 };
				Vector4 world_pos = inverse * clip;
				world_pos.x = -world_pos.x;

				mouse_to_world = { world_pos.x, world_pos.y };

			}
			//std::cout << "Mouse Ingame: " << EditorGUI::MouseInGameToWorldCoords().x << ", " << EditorGUI::MouseInGameToWorldCoords().y << "\n";
			//std::cout << "MTWMTWTM: " << world_pos.x <<  ", " << world_pos.y << "\n";
		}

		ImGui::End();
	}
}