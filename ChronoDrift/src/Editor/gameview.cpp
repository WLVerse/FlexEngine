#include "gameview.h"
#include "editorgui.h"
#include "imguipayloads.h"
#include <FlexEngine/Renderer/OpenGL/openglspriterenderer.h>

namespace ChronoDrift
{
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
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollWithMouse;
		ImGui::Begin("Game", nullptr, window_flags);
		{
			ImVec2 panel_size = ImGui::GetWindowSize();
			float width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth());
			float height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());

			if (height > panel_size.y)
			{
				float aspect_ratio = width / height;
				height = panel_size.y;
				width = height * aspect_ratio;
			}
			ImVec2 viewport_size(width, height);
			ImVec2 viewport_position((panel_size.x - viewport_size.x) / 2.0f, (panel_size.y - viewport_size.y) / 2.0f);

			ImGui::SetCursorPos(viewport_position);
			//Note: need to invert UVs vertically.
			ImGui::Image((ImTextureID)static_cast<uintptr_t>(FlexEngine::OpenGLFrameBuffer::GetCreatedTexture(FlexEngine::OpenGLFrameBuffer::CreatedTextureID::CID_finalRender)),
				viewport_size, ImVec2(0, 1), ImVec2(1, 0));
    
			
			// Obtain relative mouse position in the editor view
			ImVec2 window_top_left = ImGui::GetWindowPos();
			ImVec2 mouse_pos_ss = ImGui::GetMousePos(); // Screen space mouse pos
			ImVec2 mouse_pos_imgui_viewport = ImVec2(mouse_pos_ss.x - window_top_left.x - viewport_position.x, 
																							 mouse_pos_ss.y - window_top_left.y - viewport_position.y); // IMGUI space is screen space - top left of imgui window
			
      // Scale coordinates by window size. Originally it was rendered with the game as the target, but then superimposed as a texture onto the imgui window.
      unsigned scale_x = Application::GetCurrentWindow()->GetWidth();
			unsigned scale_y = Application::GetCurrentWindow()->GetHeight();

      // Normalize imgui to (0,1) space, then scale by window size, which converts to world as it is a 1 to 1 mapping.
      int imgui_to_window_x = static_cast<int>(mouse_pos_imgui_viewport.x / viewport_size.x * static_cast<int>(scale_x));
      int imgui_to_window_y = static_cast<int>(mouse_pos_imgui_viewport.y / viewport_size.y * static_cast<int>(scale_y));

      mouse_to_world = FlexEngine::Vector2((float)imgui_to_window_x, (float)imgui_to_window_y);
			//Log::Debug("Mouse pos in IMGUI/World space: " + std::to_string(imgui_to_window_x) + " " + std::to_string(imgui_to_window_y));
		}

		ImGui::End();
	}
}