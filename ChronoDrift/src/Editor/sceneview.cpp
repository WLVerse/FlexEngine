#include "sceneview.h"
#include "editorgui.h"
#include "imguipayloads.h"
#include <FlexEngine/Renderer/OpenGL/openglspriterenderer.h>

#include "Renderer/camera2d.h"
namespace ChronoDrift
{
	constexpr float TOP_PADDING = 10.0f;

	void SceneView::Init()
	{
		//TODO Setup editor cam from saved data upon close of program
		m_EditorCam = std::make_unique<FlexEngine::CameraData>();
	}

	void SceneView::Update()
	{
	}

	void SceneView::Shutdown()
	{
		m_EditorCam.reset();
	}

	void SceneView::CalculateViewportPosition()
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

	Vector4 SceneView::GetWorldClickPosition()
	{
		ImVec2 window_top_left = ImGui::GetWindowPos();
		ImVec2 mouse_pos_ss = ImGui::GetMousePos(); // Screen space mouse pos
		float app_width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()); 
		float app_height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());


		// Get Mouse position relative to the viewport
		ImVec2 relative_pos = ImVec2(mouse_pos_ss.x - window_top_left.x - m_viewport_position.x,
																 mouse_pos_ss.y - window_top_left.y - m_viewport_position.y); // IMGUI space is screen space - top left of imgui window

		//normalize 0, 1 coords relative to viewport, then scale by app height
		//This is mouse relative and scaled to "game" screen 
		Vector2 screen_pos = { (relative_pos.x / m_viewport_size.x) * app_width,
													 (relative_pos.y / m_viewport_size.y) * app_height };


		Vector2 ndc_click_pos = { (2 * screen_pos.x / app_width) - 1, 1 - 2 * screen_pos.y / app_height };
		Matrix4x4 inverse = (m_EditorCam->proj_viewMatrix).Inverse();
		Vector4 clip = { ndc_click_pos.x,
										 ndc_click_pos.y,
										 1.0f,
										 1 };
		Vector4 world_pos = inverse * clip;
		world_pos.x = -world_pos.x;

		return world_pos;
	}

	ImVec2 SceneView::WorldToScreen(const FlexEngine::Vector2& position)
	{
		Vector4 world_pos = { -position.x, position.y, 0.0f, 1.0f };
		Vector4 clip = m_EditorCam->proj_viewMatrix * world_pos;
		if (clip.w != 0.0f)
		{
			clip.x /= clip.w;
			clip.y /= clip.w;
			clip.z /= clip.w;
		}

		// Convert to coordinates relative to viewport 
		float x_screen = ((clip.x + 1.0f) * 0.5f) * m_viewport_size.x;
		float y_screen = ((1.0f - clip.y) * 0.5f) * m_viewport_size.y;

		//finally translate viewport relative position to screen relative coords
		ImVec2 screen_pos = { x_screen, y_screen };
		screen_pos.x += ImGui::GetWindowPos().x + m_viewport_position.x;
		screen_pos.y += ImGui::GetWindowPos().y + m_viewport_position.y;

		return screen_pos;
	}

	FlexECS::Entity SceneView::FindClickedEntity()
	{
		FlexECS::Entity clicked_entity = FlexECS::Entity::Null;
		int selected_z_index = INT_MIN;
		Vector4 mouse_world_pos = GetWorldClickPosition();

		//AABB tiem
		auto scene = FlexECS::Scene::GetActiveScene();
		for (auto entity : scene->CachedQuery<Position, Scale, Transform, Shader, ZIndex>()) //you probably only wanna click on things that are rendered
		{
			auto& pos = entity.GetComponent<Position>()->position;
			auto& scale = entity.GetComponent<Scale>()->scale;
			//auto& transform = entity.GetComponent<Transform>()->transform;
			//Vector2 pos = { -transform.m30, transform.m31 };
			//Vector2 scale = { transform.m00, transform.m11 };

			if (mouse_world_pos.x >= (pos.x - (scale.x / 2)) &&
					mouse_world_pos.x <= (pos.x + (scale.x / 2)) &&
					mouse_world_pos.y >= (pos.y - (scale.y / 2)) &&
					mouse_world_pos.y <= (pos.y + (scale.y / 2)))
			{
				if (entity.GetComponent<ZIndex>()->z > selected_z_index)
				{
					clicked_entity = entity;
					selected_z_index = entity.GetComponent<ZIndex>()->z;
				}
			}
		}

		return clicked_entity;
	}

	void SceneView::HandleMouseAndKeyboardEvents()
	{
		if (ImGui::IsWindowFocused())
		{
			if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
			{
				if (!m_gizmo_hovered)
				{
					//check if mouse click was within the viewport
					bool within_viewport = (ImGui::GetMousePos().x >= m_viewport_screen_position.x && ImGui::GetMousePos().x <= m_viewport_screen_position.x + m_viewport_size.x &&
																	ImGui::GetMousePos().y >= m_viewport_screen_position.y && ImGui::GetMousePos().y <= m_viewport_screen_position.y + m_viewport_size.y);
					FlexECS::Entity entity = FindClickedEntity();
					if (entity != FlexECS::Entity::Null && within_viewport)
					{
						Editor::GetInstance().SelectEntity(entity);
					}
					else
					{
						Editor::GetInstance().SelectEntity(FlexECS::Entity::Null);
					}
				}
			}
			
			//Changing Gizmos type
			if (ImGui::IsKeyPressed(ImGuiKey_W))
			{
				m_current_gizmo_type = GizmoType::TRANSLATE;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_E))
			{
				m_current_gizmo_type = GizmoType::SCALE;
			}
			else if (ImGui::IsKeyPressed(ImGuiKey_R))
			{
				m_current_gizmo_type = GizmoType::ROTATE;
			}

			//Delete Entity
			if (ImGui::IsKeyPressed(ImGuiKey_Delete))
			{
				Editor::GetInstance().DeleteSelectedEntity();
			}
		}
	}

	void SceneView::DrawGizmos()
	{
		m_gizmo_hovered = false;
		FlexECS::Entity selected_entity = Editor::GetInstance().GetSelectedEntity();
		if (selected_entity == FlexECS::Entity::Null) return;

		selected_entity.GetComponent<Transform>()->is_dirty = true;
		auto& entity_transform = selected_entity.GetComponent<Transform>()->transform;
		auto& entity_position = selected_entity.GetComponent<Position>()->position;
		auto& entity_scale		= selected_entity.GetComponent<Scale>()->scale;

		//Global pos for accuracy (esp for entities with parent)
		Vector2 global_pos = { -entity_transform.m30, entity_transform.m31 };
		Vector2 global_scale = { entity_transform.m00, entity_transform.m11 };

		//Find out where on the screen to draw the gizmos
		//Take entity position and convert it back to screen position
		ImVec2 gizmo_origin_pos = WorldToScreen(global_pos);
		//Display an imgui rect around the sprite so we know where we are clicking, at least
		Vector2 half_scale = global_scale; half_scale /= 2;
		ImVec2 entity_min = WorldToScreen(global_pos - half_scale);
		ImVec2 entity_max = WorldToScreen(global_pos + half_scale);

		ImRect bounding_box (entity_min, entity_max);
		ImGui::GetWindowDrawList()->AddRect(bounding_box.Min, bounding_box.Max, IM_COL32(255, 255, 0, 150));

		//Draw Translate Gizmo
		if (m_current_gizmo_type == GizmoType::TRANSLATE)
		{
			Vector2 pos_change{};
			bool right, up, xy;
			EditorGUI::GizmoTranslateRight(&pos_change.x, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &right);
			EditorGUI::GizmoTranslateUp(&pos_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &up);
			EditorGUI::GizmoTranslateXY(&pos_change.x, &pos_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &xy);
			m_gizmo_hovered = right || up || xy;

			//Scale the change in position with relation to screen size
			//pos_change represents how much the gizmo moved in absolute screen coordinates.
			//Need to convert screen space to world space
			pos_change.x *= FlexEngine::Application::GetCurrentWindow()->GetWidth()  / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
			pos_change.y *= FlexEngine::Application::GetCurrentWindow()->GetHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
			entity_position += pos_change;
		}
		else if (m_current_gizmo_type == GizmoType::SCALE)
		{
			Vector2 scale_change{};
			float value{};
			bool right, up, xy;
			EditorGUI::Gizmo_Scale_X(&scale_change.x, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &right);
			EditorGUI::Gizmo_Scale_Y(&scale_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &up);
			EditorGUI::Gizmo_Scale_XY(&value, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &xy);
			m_gizmo_hovered = right || up || xy;
			if (value != 0)	//if using xy scale
			{
				scale_change.x = value;
				scale_change.y = value;
			}
			else 
			{
				scale_change.y = -scale_change.y;
			}

			scale_change.x *= FlexEngine::Application::GetCurrentWindow()->GetWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
			scale_change.y *= FlexEngine::Application::GetCurrentWindow()->GetHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
			entity_scale += scale_change;
		}
		else if (m_current_gizmo_type == GizmoType::ROTATE)
		{
			if (selected_entity.HasComponent<Rotation>())
			{
				auto& entity_rotation = selected_entity.GetComponent<Rotation>()->rotation;
				float value{};
				bool hovered;
				EditorGUI::Gizmo_Rotate_Z(&value, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &hovered);
				m_gizmo_hovered = hovered;
				entity_rotation.z += value * (180 / IM_PI);
			
				//Clamp to -360 and 360
				if (entity_rotation.z > 360.0f) entity_rotation.z -= 360.0f;
				if (entity_rotation.z < -360.0f) entity_rotation.z += 360.0f;
			}
		}
	}


	void SceneView::EditorUI()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav;

		ImGui::Begin("Scene", nullptr, window_flags);
		{
			CalculateViewportPosition();

			//Display Scene texture
			ImGui::SetCursorPos(m_viewport_position);
			ImGui::Image((ImTextureID)static_cast<uintptr_t>(FlexEngine::OpenGLFrameBuffer::GetCreatedTexture(FlexEngine::OpenGLFrameBuffer::CreatedTextureID::CID_editor)),
				m_viewport_size, ImVec2(0, 1), ImVec2(1, 0));

			HandleMouseAndKeyboardEvents();
			DrawGizmos();
			UpdateEditorCam();

			//Create new entity when dragging an image from assets to scene
			if (auto image = EditorGUI::StartWindowPayloadReceiver<const char>(PayloadTags::IMAGE))
			{
				auto scene = FlexECS::Scene::GetActiveScene();

				std::string image_key(image);
				std::filesystem::path path = image_key;
				Vector4 mouse_world_pos = GetWorldClickPosition();
				Vector2 position{ mouse_world_pos.x, mouse_world_pos.y };
				FlexECS::Entity new_entity = scene->CreateEntity(path.filename().string());
				new_entity.AddComponent<IsActive>({ true });
				new_entity.AddComponent<Position>({ position });
				new_entity.AddComponent<Rotation>({});
				new_entity.AddComponent<Scale>({ Vector2::One * 100.0f });
				new_entity.AddComponent<Transform>({});
				new_entity.AddComponent<ZIndex>({});
				new_entity.AddComponent<Sprite>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(image_key) });
				new_entity.AddComponent<Shader>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\shaders\texture)") });
				EditorGUI::EndPayloadReceiver();
			}
		}
		ImGui::End();
	}

	void SceneView::UpdateEditorCam()
	{
		if (ImGui::IsMouseDown(2)) //Use middle mouse button to drag and move
		{
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(2);
			if(drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(2);
			Vector2 camera_pos_change{-drag_delta.x, -drag_delta.y};

			camera_pos_change.x *= FlexEngine::Application::GetCurrentWindow()->GetWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
			camera_pos_change.y *= FlexEngine::Application::GetCurrentWindow()->GetHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
			m_EditorCam->position += Vector3(camera_pos_change.x, camera_pos_change.y);
		}
		else if (ImGui::IsMouseDown(1) && ImGui::IsKeyDown(ImGuiKey_LeftAlt)) //This additional control is because my middle mouse button is broken T_T
		{
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(1);
			if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(1);
			Vector2 camera_pos_change{ -drag_delta.x, -drag_delta.y };

			camera_pos_change.x *= FlexEngine::Application::GetCurrentWindow()->GetWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
			camera_pos_change.y *= FlexEngine::Application::GetCurrentWindow()->GetHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
			m_EditorCam->position += Vector3(camera_pos_change.x, camera_pos_change.y);
		}

		ImGuiIO& io = ImGui::GetIO();
		if (io.MouseWheel != 0.0f)
		{
			if (io.MouseWheel > 0.0f)
			{
				//zoom in
			}
			else
			{
				//zoom out
			}
		}

		//Update data
		Camera2D::UpdateProjectionMatrix(*m_EditorCam.get());
		Camera2D::UpdateViewMatrix(*m_EditorCam.get());

		//Update Camera Manager
		FlexECS::EntityID currEditorID = Editor::GetInstance().GetCamManager().GetEditorCamera();
		Editor::GetInstance().GetCamManager().UpdateData(currEditorID, *m_EditorCam.get());
	}
}
