#include "sceneview.h"
#include "editorgui.h"
#include "imguipayloads.h"
#include <set>

namespace Editor
{
	constexpr float TOP_PADDING = 10.0f;

	void SceneView::Init()
	{
		auto scene = FlexECS::Scene::GetActiveScene();
		m_editor_camera = scene->CreateEntity("Editor Cam :3");
		m_editor_camera.AddComponent<Position>({});
		m_editor_camera.AddComponent<Rotation>({});
		m_editor_camera.AddComponent<Scale>({});
		m_editor_camera.AddComponent<Transform>({});

		Camera cam({ 850.0f, 450.0f, 0 }, 1600.0f, 900.0f, -2.0f, 2.0f);
		m_editor_camera.AddComponent<Camera>(cam);

		CameraManager::SetEditorCameraID(m_editor_camera);
		CameraManager::SetCamera(m_editor_camera, m_editor_camera.GetComponent<Camera>());
	}

	void SceneView::Update()
	{
		CameraManager::GetEditorCamera()->Update();
	}

	void SceneView::Shutdown()
	{

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

	FlexEngine::Vector4 SceneView::GetWorldClickPosition()
	{
		ImVec2 window_top_left = ImGui::GetWindowPos();
		ImVec2 mouse_pos_ss = ImGui::GetMousePos(); // Screen space mouse pos
		float app_width = CameraManager::GetEditorCamera()->m_data.m_OrthoWidth;
		float app_height = CameraManager::GetEditorCamera()->m_data.m_OrthoHeight;

		// Get Mouse position relative to the viewport
		ImVec2 relative_pos = ImVec2(mouse_pos_ss.x - window_top_left.x - m_viewport_position.x,
																 mouse_pos_ss.y - window_top_left.y - m_viewport_position.y); // IMGUI space is screen space - top left of imgui window

		//normalize 0, 1 coords relative to viewport, then scale by app height
		//This is mouse relative and scaled to "game" screen 
		Vector2 screen_pos = { (relative_pos.x / m_viewport_size.x) * app_width,
													 (relative_pos.y / m_viewport_size.y) * app_height };


		Vector2 ndc_click_pos = { (2 * screen_pos.x / app_width) - 1, 1 - 2 * screen_pos.y / app_height };
		Matrix4x4 inverse = (CameraManager::GetEditorCamera()->GetProjectionMatrix()).Inverse();
		Vector4 clip = { ndc_click_pos.x,
										 ndc_click_pos.y,
										 1.0f,
										 1 };
		Vector4 world_pos = inverse * clip;

		return world_pos;
	}

	ImVec2 SceneView::WorldToScreen(const FlexEngine::Vector2& position)
	{
		Vector4 world_pos = { position.x, position.y, 0.0f, 1.0f };
		Vector4 clip = CameraManager::GetEditorCamera()->GetProjViewMatrix() * world_pos;
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
		Vector4 mouse_world_pos = GetWorldClickPosition();
		
		auto z_index_sort = [](const std::pair<FlexECS::EntityID, float>& a, const std::pair<FlexECS::EntityID, float>& b) 
		{
			if (a.second == b.second)
					return a.first < b.first; 
			return a.second >= b.second;
		};
		
		std::set<std::pair<FlexECS::EntityID, float>, decltype(z_index_sort)> clicked_entities(z_index_sort);
		
		//Gets all entities under the mouse (including entities overlapping each other)
		//Sorts them based on z-index (or should it be Position.z???)
		auto scene = FlexECS::Scene::GetActiveScene();
		for (auto entity : scene->CachedQuery<Position, Scale, Transform, Sprite>()) //you probably only wanna click on things that are rendered
		{
			auto& active = entity.GetComponent<Transform>()->is_active;
			if (!active) continue;

			auto& pos = entity.GetComponent<Position>()->position;
			auto& scale = entity.GetComponent<Scale>()->scale;
			if (mouse_world_pos.x >= (pos.x - (scale.x / 2)) &&
					mouse_world_pos.x <= (pos.x + (scale.x / 2)) &&
					mouse_world_pos.y >= (pos.y - (scale.y / 2)) &&
					mouse_world_pos.y <= (pos.y + (scale.y / 2)))
			{
				clicked_entities.insert(std::make_pair(entity, entity.GetComponent<Position>()->position.z));
			}
		}

		//Cycles to the next z-index entity, so we can select the entity even when covered by another.
		if (!clicked_entities.empty())
		{
			FlexECS::Entity current_selected = FlexECS::Entity::Null;
			auto& selected_list = Editor::GetInstance().GetSystem<SelectionSystem>()->GetSelectedEntities();
			if (selected_list.size() == 1)
			{
				current_selected = *selected_list.begin();
			}

			//if (current_selected != FlexECS::Entity::Null && !current_selected.HasComponent<ZIndex>()) current_selected = FlexECS::Entity::Null;

			if (current_selected != FlexECS::Entity::Null)
			{
				auto current = clicked_entities.find({ FlexECS::EntityID(current_selected), current_selected.GetComponent<Position>()->position.z });
				
				if (current == clicked_entities.end() || ++current == clicked_entities.end())
				{
					clicked_entity = clicked_entities.begin()->first;
				}
				else 
				{
					clicked_entity = current->first;
				}
			}
			else
			{
				clicked_entity = clicked_entities.begin()->first;
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
						if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
						{
							Editor::GetInstance().GetSystem<SelectionSystem>()->AddSelectedEntity(entity);
						}
						else
						{
							Editor::GetInstance().GetSystem<SelectionSystem>()->SelectEntity(entity);
						}
					}
					else
					{
						Editor::GetInstance().GetSystem<SelectionSystem>()->ClearSelection();
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
				Editor::GetInstance().GetSystem<SelectionSystem>()->DeleteSelectedEntities();
			}
		}
	}

	void SceneView::DrawGizmos()
	{
		m_gizmo_hovered = false;
		auto& selected_list = Editor::GetInstance().GetSystem<SelectionSystem>()->GetSelectedEntities();
		//Single entity gizmos and multiple gizmos settled differently
		if (selected_list.size() == 1)
		{
			FlexECS::Entity selected_entity = FlexECS::Entity::Null;
			selected_entity = *selected_list.begin();

			if (!selected_entity.HasComponent<Transform>() || !selected_entity.HasComponent<Position>()
				 || !selected_entity.HasComponent<Scale>() || !selected_entity.HasComponent<Rotation>()) {
				return;
			}
			else {
				selected_entity.GetComponent<Transform>()->is_dirty = true;
			}
			
			auto& entity_transform = selected_entity.GetComponent<Transform>()->transform;
			auto& entity_position = selected_entity.GetComponent<Position>()->position;
			auto& entity_scale	= selected_entity.GetComponent<Scale>()->scale;

			//Global pos for accuracy (esp for entities with parent)
			//Vector2 global_pos = { -entity_transform.m30, entity_transform.m31 };
			Vector2 global_pos = { entity_transform.m30, entity_transform.m31 };
			Vector2 global_scale = { entity_transform.m00, entity_transform.m11 };

			//Find out where on the screen to draw the gizmos
			//Take entity position and convert it back to screen position
			ImVec2 gizmo_origin_pos = WorldToScreen(global_pos);
			Vector2 half_scale = global_scale; half_scale /= 2;
			ImVec2 entity_min = WorldToScreen(global_pos - half_scale);
			ImVec2 entity_max = WorldToScreen(global_pos + half_scale);

			//Display an imgui rect around the sprite so we know where we are clicking, at least
			ImRect bounding_box (entity_min, entity_max);
			ImGui::GetWindowDrawList()->AddRect(bounding_box.Min, bounding_box.Max, IM_COL32(255, 255, 0, 150));

			//Draw Translate Gizmo
			if (m_current_gizmo_type == GizmoType::TRANSLATE)
			{
				Vector2 pos_change{};
				bool right, up, xy;
				bool recording_ended = false;
				switch (EditorGUI::GizmoTranslateRight(&pos_change.x, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &right))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					m_recorded_position.position = entity_position;
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}

				switch (EditorGUI::GizmoTranslateUp(&pos_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &up))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					m_recorded_position.position = entity_position;
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}

				switch (EditorGUI::GizmoTranslateXY(&pos_change.x, &pos_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &xy))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					//start recording old position
					m_recorded_position.position = entity_position;
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					//End recording, new position
					recording_ended = true;
					break;
				default:
					break;
				}

				m_gizmo_hovered = right || up || xy;

				//Scale the change in position with relation to screen size
				//pos_change represents how much the gizmo moved in absolute screen coordinates.
				//Need to convert screen space to world space
				pos_change.x *= CameraManager::GetEditorCamera()->m_data.m_OrthoWidth  / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				pos_change.y *= CameraManager::GetEditorCamera()->m_data.m_OrthoHeight / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
				entity_position += Vector3(pos_change.x, -pos_change.y, 0.0f);

				if (recording_ended)
				{
					if (m_recorded_position.position != entity_position)
					{
						//auto cmd = reinterpret_cast<EditorCommands*>(Editor::GetInstance().GetSystem("EditorCommands"));
						//cmd->UpdateComponent(selected_entity, "Position", &m_recorded_position, selected_entity.GetComponent<Position>(), sizeof(Position));
					}
				}
			}
			else if (m_current_gizmo_type == GizmoType::SCALE)
			{
				Vector2 scale_change{};
				float value{};
				bool right, up, xy;
				bool recording_ended = false;
				switch (EditorGUI::Gizmo_Scale_X(&scale_change.x, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &right))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					m_recorded_scale.scale = entity_scale;
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}
				switch (EditorGUI::Gizmo_Scale_Y(&scale_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &up))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					m_recorded_scale.scale = entity_scale;
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}
				switch (EditorGUI::Gizmo_Scale_XY(&value, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &xy))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					m_recorded_scale.scale = entity_scale;
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}
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

				scale_change.x *= CameraManager::GetEditorCamera()->m_data.m_OrthoWidth / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				scale_change.y *= CameraManager::GetEditorCamera()->m_data.m_OrthoHeight / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
				entity_scale += Vector3(scale_change.x, -scale_change.y, 0.0f);

				if (recording_ended)
				{
					if (m_recorded_scale.scale != entity_scale)
					{
						//auto cmd = reinterpret_cast<EditorCommands*>(Editor::GetInstance().GetSystem("EditorCommands"));
						//cmd->UpdateComponent(selected_entity, "Scale", &m_recorded_scale, selected_entity.GetComponent<Scale>(), sizeof(Scale));
					}
				}
			}
			else if (m_current_gizmo_type == GizmoType::ROTATE)
			{
				if (selected_entity.HasComponent<Rotation>())
				{
					auto& entity_rotation = selected_entity.GetComponent<Rotation>()->rotation;
					float value{};
					bool hovered;
					bool recording_ended = false;
					switch (EditorGUI::Gizmo_Rotate_Z(&value, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &hovered))
					{
					case EditorGUI::GizmoStatus::START_DRAG:
						m_recorded_rotation.rotation = entity_rotation;
						break;
					case EditorGUI::GizmoStatus::DRAGGING:
						break;
					case EditorGUI::GizmoStatus::END_DRAG:
						recording_ended = true;
						break;
					default:
						break;
					}
					m_gizmo_hovered = hovered;
					entity_rotation.z -= value * (180 / IM_PI);

					//Clamp to -360 and 360
					if (entity_rotation.z > 360.0f) entity_rotation.z -= 360.0f;
					if (entity_rotation.z < -360.0f) entity_rotation.z += 360.0f;

					if (recording_ended)
					{
						if (m_recorded_rotation.rotation != entity_rotation)
						{
							//auto cmd = reinterpret_cast<EditorCommands*>(Editor::GetInstance().GetSystem("EditorCommands"));
							//cmd->UpdateComponent(selected_entity, "Rotation", &m_recorded_rotation, selected_entity.GetComponent<Rotation>(), sizeof(Rotation));
						}
					}
				}
			}
		}
		else if (selected_list.size() > 1)	//code copied rn because of preparations for undo delete, but think about bringing it up
		{
			//Get average of positions of the entities.
			//That is where we'll place the gizmo
			Vector3 average_pos;
			for (FlexECS::Entity entity : selected_list)
			{
				if (!entity.HasComponent<Transform>() || !entity.HasComponent<Position>() ||
					  !entity.HasComponent<Scale>() || !entity.HasComponent<Rotation>())
				{
					return;
				}
				auto& entity_transform = entity.GetComponent<Transform>()->transform;
				average_pos += { entity_transform.m30, entity_transform.m31, entity_transform.m32};
				
				Vector2 global_scale = { entity_transform.m00, entity_transform.m11 };
				Vector2 half_scale = global_scale; half_scale /= 2;
				ImVec2 entity_min = WorldToScreen(Vector2(entity_transform.m30, entity_transform.m31) - half_scale);
				ImVec2 entity_max = WorldToScreen(Vector2(entity_transform.m30, entity_transform.m31) + half_scale);
				ImRect bounding_box(entity_min, entity_max);
				ImGui::GetWindowDrawList()->AddRect(bounding_box.Min, bounding_box.Max, IM_COL32(255, 255, 0, 150));
			}
			
			average_pos /= selected_list.size();
			ImVec2 gizmo_origin_pos = WorldToScreen({average_pos.x, average_pos.y});

			//Draw Translate Gizmo
			if (m_current_gizmo_type == GizmoType::TRANSLATE)
			{
				Vector2 pos_change{};
				bool right, up, xy;
				bool recording_ended = false;
				switch (EditorGUI::GizmoTranslateRight(&pos_change.x, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &right))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}

				switch (EditorGUI::GizmoTranslateUp(&pos_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &up))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}

				switch (EditorGUI::GizmoTranslateXY(&pos_change.x, &pos_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &xy))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					//start recording old position
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					//End recording, new position
					recording_ended = true;
					break;
				default:
					break;
				}

				m_gizmo_hovered = right || up || xy;

				pos_change.x *= CameraManager::GetEditorCamera()->m_data.m_OrthoWidth/ ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				pos_change.y *= CameraManager::GetEditorCamera()->m_data.m_OrthoHeight / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);

				for (FlexECS::Entity entity : selected_list)
				{
					entity.GetComponent<Position>()->position += Vector3(pos_change.x, -pos_change.y, 0.0f);
				}
			}
			else if (m_current_gizmo_type == GizmoType::SCALE)
			{
				Vector2 scale_change{};
				float value{};
				bool right, up, xy;
				bool recording_ended = false;
				switch (EditorGUI::Gizmo_Scale_X(&scale_change.x, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &right))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}
				switch (EditorGUI::Gizmo_Scale_Y(&scale_change.y, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &up))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}
				switch (EditorGUI::Gizmo_Scale_XY(&value, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &xy))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}
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

				scale_change.x *= CameraManager::GetEditorCamera()->m_data.m_OrthoWidth / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				scale_change.y *= CameraManager::GetEditorCamera()->m_data.m_OrthoHeight / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);

				for (FlexECS::Entity entity : selected_list)
				{
					entity.GetComponent<Scale>()->scale += Vector3(scale_change.x, -scale_change.y, 0.0f);
				}
			}
			else if (m_current_gizmo_type == GizmoType::ROTATE)
			{
				float value{};
				bool hovered;
				bool recording_ended = false;
				switch (EditorGUI::Gizmo_Rotate_Z(&value, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &hovered))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}
				m_gizmo_hovered = hovered;

				for (FlexECS::Entity entity : selected_list)
				{
					auto& entity_rotation = entity.GetComponent<Rotation>()->rotation;
					entity_rotation.z -= value * (180 / IM_PI);
					if (entity_rotation.z > 360.0f) entity_rotation.z -= 360.0f;
					if (entity_rotation.z < -360.0f) entity_rotation.z += 360.0f;
				}
			}
		}
	}

	void SceneView::EditorUI()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoNav;

		ImGui::Begin("Scene", nullptr, window_flags);
			CalculateViewportPosition();

			//Display Scene texture
			ImGui::SetCursorPos(m_viewport_position);

			if (CameraManager::HasCamera(CameraManager::GetMainGameCameraID()))
			{
				Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
				GLuint texture = Window::FrameBufferManager.GetCurrentFrameBuffer()->GetColorAttachment();

				ImGui::Image((ImTextureID)static_cast<uintptr_t>(texture),
					m_viewport_size, ImVec2(0, 1), ImVec2(1, 0));

				HandleMouseAndKeyboardEvents();
				DrawGizmos();
				MoveEditorCam();

				//Create new entity when dragging an image from assets to scene
				if (auto image = EditorGUI::StartWindowPayloadReceiver<const char>(PayloadTags::IMAGE))
				{
					auto scene = FlexECS::Scene::GetActiveScene();

					std::string image_key(image);
					std::filesystem::path path = image_key;
					Vector4 mouse_world_pos = GetWorldClickPosition();
					Vector2 position{ mouse_world_pos.x, mouse_world_pos.y };
					FlexECS::Entity new_entity = scene->CreateEntity(path.filename().string());
					new_entity.AddComponent<Position>({ position });
					new_entity.AddComponent<Rotation>({});
					new_entity.AddComponent<Scale>({ Vector2::One * 100.0f });
					new_entity.AddComponent<Transform>({});
					new_entity.AddComponent<Sprite>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(image_key) });
					EditorGUI::EndPayloadReceiver();
				}
			}
		ImGui::End();
	}

	void SceneView::MoveEditorCam()
	{
		if (ImGui::IsWindowHovered() || m_dragging_camera)
		{
			//Camera moving
			if (ImGui::IsMouseDown(2)) //Use middle mouse button to drag and move
			{
				m_dragging_camera = true;
				ImVec2 drag_delta = ImGui::GetMouseDragDelta(2);
				if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(2);
				Vector2 camera_pos_change{ drag_delta.x, -drag_delta.y };

				camera_pos_change.x *= CameraManager::GetEditorCamera()->m_data.m_OrthoWidth / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				camera_pos_change.y *= CameraManager::GetEditorCamera()->m_data.m_OrthoHeight / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
				CameraManager::GetEditorCamera()->m_data.position += Vector3(camera_pos_change.x, -camera_pos_change.y, 0.0f);
				//CameraManager::GetEditorCamera()->MoveCamera(Vector3(camera_pos_change.x, camera_pos_change.y));
			}
			else if (ImGui::IsMouseReleased(2))
			{
				m_dragging_camera = false;
			}

			if (ImGui::IsMouseDown(1) && ImGui::IsKeyDown(ImGuiKey_LeftAlt)) //This additional control is because my middle mouse button is broken T_T
			{
				m_dragging_camera = true;
				ImVec2 drag_delta = ImGui::GetMouseDragDelta(1);
				if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(1);
				Vector2 camera_pos_change{ drag_delta.x, -drag_delta.y };

				camera_pos_change.x *= CameraManager::GetEditorCamera()->m_data.m_OrthoWidth / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				camera_pos_change.y *= CameraManager::GetEditorCamera()->m_data.m_OrthoHeight / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
				CameraManager::GetEditorCamera()->m_data.position += Vector3(camera_pos_change.x, -camera_pos_change.y, 0.0f);
				//CameraManager::GetEditorCamera()->MoveCamera(Vector3(camera_pos_change.x, camera_pos_change.y));
			}
			else if (ImGui::IsMouseReleased(1))
			{
				m_dragging_camera = false;
			}


			//Camera zooming
			float baseAspectRatio = CameraManager::GetEditorCamera()->m_data.m_OrthoWidth / CameraManager::GetEditorCamera()->m_data.m_OrthoHeight;  // Base aspect ratio (can be easily adjusted)
			float zoomSpeed = 40.0f;      // Adjust this for faster/slower zoom
			float minZoom = 100.0f;       // Minimum orthographic width
			float maxZoom = 5000.0f;      // Maximum orthographic width

			ImGuiIO& io = ImGui::GetIO();
			if (io.MouseWheel != 0.0f)
			{
				float zoomDelta = io.MouseWheel * zoomSpeed;
				float new_right = std::clamp(CameraManager::GetEditorCamera()->m_data.m_OrthoWidth - zoomDelta, minZoom, maxZoom);
				CameraManager::GetEditorCamera()->SetOrthographic(0.0f, new_right, new_right / baseAspectRatio, 0.0f, -2.0f, 2.0f);
			}
		}
	}
}
