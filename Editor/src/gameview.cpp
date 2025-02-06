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
		float aspect_ratio = width / height;

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


	void GameView::EditorUI()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav;
		ImGui::Begin("Game", nullptr, window_flags);
		{
			CalculateViewportPosition();
			ImGui::SetCursorPos(m_viewport_position);

			Window::FrameBufferManager.SetCurrentFrameBuffer("Game");
			GLuint texture = Window::FrameBufferManager.GetCurrentFrameBuffer()->GetColorAttachment();

			ImGui::Image((ImTextureID)static_cast<uintptr_t>(texture), m_viewport_size, ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
	}
}