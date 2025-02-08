/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// gameview.cpp
//
// Game Panel for the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "gameview.h"
#include "editorgui.h"
#include "imguipayloads.h"

namespace Editor
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

	void GameView::CalculateViewportPosition()
	{
		ImVec2 window_top_left = ImGui::GetWindowPos();
		ImVec2 mouse_pos_ss = ImGui::GetMousePos(); // Screen space mouse pos

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
		float aspect_ratio = CameraManager::GetMainGameCamera()->GetOrthoWidth() / CameraManager::GetMainGameCamera()->GetOrthoHeight();

		//also have a tiny bit of extra padding to ensure the whole image is in frame
		if (height > content_size.y - TOP_PADDING)
		{
			height = content_size.y - TOP_PADDING;
			width = height * aspect_ratio;
		}
		m_viewport_size = { width, height };
		m_viewport_position = { (panel_size.x - m_viewport_size.x) / 2.0f, title_bar_height + TOP_PADDING / 2.0f }; // relative to imgui window
		m_viewport_screen_position = { window_top_left.x + m_viewport_position.x, window_top_left.y + m_viewport_position.y };
	}

	void GameView::SetMousePosition()
	{
		if (!ImGui::IsWindowFocused()) return;

		ImVec2 window_top_left = ImGui::GetWindowPos();
		ImVec2 mouse_pos_ss = ImGui::GetMousePos(); // Screen space mouse pos
		float app_width = Window::FrameBufferManager.GetFrameBuffer("Game")->GetWidth();
		float app_height = Window::FrameBufferManager.GetFrameBuffer("Game")->GetHeight();

		bool within_viewport = (ImGui::GetMousePos().x >= m_viewport_screen_position.x && ImGui::GetMousePos().x <= m_viewport_screen_position.x + m_viewport_size.x &&
														ImGui::GetMousePos().y >= m_viewport_screen_position.y && ImGui::GetMousePos().y <= m_viewport_screen_position.y + m_viewport_size.y);

		if (!within_viewport)
		{
			FlexEngine::Input::SetCursorPosition(m_mouse_pos);
			return;
		}

		// Get Mouse position relative to the viewport
		ImVec2 relative_pos = ImVec2(mouse_pos_ss.x - window_top_left.x - m_viewport_position.x,
																 mouse_pos_ss.y - window_top_left.y - m_viewport_position.y); // IMGUI space is screen space - top left of imgui window

		//normalize 0, 1 coords relative to viewport, then scale by app height
		//This is mouse relative and scaled to "game" screen 
		m_mouse_pos = { (relative_pos.x / m_viewport_size.x) * app_width,
													 (relative_pos.y / m_viewport_size.y) * app_height };

		FlexEngine::Input::SetCursorPosition(m_mouse_pos);
	}


	void GameView::EditorUI()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav;
		ImGui::Begin("Game", nullptr, window_flags);
		{
			if (CameraManager::has_main_camera)
			{
				CalculateViewportPosition();
				ImGui::SetCursorPos(m_viewport_position);

				Window::FrameBufferManager.SetCurrentFrameBuffer("Game");
				GLuint texture = Window::FrameBufferManager.GetCurrentFrameBuffer()->GetColorAttachment();

				ImGui::Image((ImTextureID)static_cast<uintptr_t>(texture), m_viewport_size, ImVec2(0, 1), ImVec2(1, 0));
			
				SetMousePosition();	//so our mouse can work inside editor-game, not only game-game
			}
		}

		ImGui::End();
	}
}