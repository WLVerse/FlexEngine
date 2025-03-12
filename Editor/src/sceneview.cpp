#include "sceneview.h"
#include "editorgui.h"
#include "imguipayloads.h"
#include <set>

#include <glm/glm.hpp> // :3
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Note to self - thoughts:
// About global_pos / position
// >global_pos is calculated with Transform component.
//  this is required when an object is parented, so the values inside the pos/rot/scale components don't match.
//
// >right now, we don't got parents, so its irrelevant right now, but just hope you remember, future me!
//  obb is calculated with local scale, but like, the scale within the transform mat4x4 is NOT the actual scale
//	since it's affected by rotation as well.
//	this is the main reason why i switched back to just calculating with local pos. but eventually,
//	if we ever have mothers and fathers again, you gotta think of something
//

std::array<Vector2, 4> ComputeOBBCorners(Matrix4x4& transform)
{
	Vector4 corners[4] =
	{
		{-0.5f, -0.5f, 0.5f, 1.0f},
		{0.5f, -0.5f, 0.5f, 1.0f},
		{0.5f, 0.5f, 0.5f, 1.0f},
		{-0.5f, 0.5f, 0.5f, 1.0f}
	};

	for (auto& corner : corners)
	{
		corner = transform * corner;
	}
	return
	{
		Vector2(corners[0].x, corners[0].y),
		Vector2(corners[1].x, corners[1].y),
		Vector2(corners[2].x, corners[2].y),
		Vector2(corners[3].x, corners[3].y)
	};
}


namespace Editor
{
	constexpr float TOP_PADDING = 10.0f;

	void SceneView::Init()
	{

	}

	void SceneView::Update()
	{
		Editor::GetInstance().m_editorCamera.Update();
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
		float aspect_ratio = Editor::GetInstance().m_editorCamera.GetOrthoWidth() / Editor::GetInstance().m_editorCamera.GetOrthoHeight();

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
		float app_width = Editor::GetInstance().m_editorCamera.GetOrthoWidth();
		float app_height = Editor::GetInstance().m_editorCamera.GetOrthoHeight();

		// Get Mouse position relative to the viewport
		ImVec2 relative_pos = ImVec2(mouse_pos_ss.x - window_top_left.x - m_viewport_position.x,
																 mouse_pos_ss.y - window_top_left.y - m_viewport_position.y); // IMGUI space is screen space - top left of imgui window

		//normalize 0, 1 coords relative to viewport, then scale by app height
		//This is mouse relative and scaled to "game" screen 
		Vector2 screen_pos = { (relative_pos.x / m_viewport_size.x) * app_width,
													 (relative_pos.y / m_viewport_size.y) * app_height };


		Vector2 ndc_click_pos = { (2 * screen_pos.x / app_width) - 1, 1 - 2 * screen_pos.y / app_height };
		Matrix4x4 inverse = (Editor::GetInstance().m_editorCamera.GetProjViewMatrix()).Inverse();
		Vector4 clip = { ndc_click_pos.x,
										 ndc_click_pos.y,
										 1.0f,
										 1 };
		Vector4 world_pos = inverse * clip;

		return world_pos;
	}

	FlexEngine::Vector4 SceneView::ScreenToWorld(ImVec2 point)
	{
		ImVec2 window_top_left = ImGui::GetWindowPos();
		float app_width = Editor::GetInstance().m_editorCamera.GetOrthoWidth();
		float app_height = Editor::GetInstance().m_editorCamera.GetOrthoHeight();

		// Get Mouse position relative to the viewport
		ImVec2 relative_pos = ImVec2(point.x - window_top_left.x - m_viewport_position.x,
																 point.y - window_top_left.y - m_viewport_position.y); // IMGUI space is screen space - top left of imgui window

		Vector2 screen_pos = { (relative_pos.x / m_viewport_size.x) * app_width,
													 (relative_pos.y / m_viewport_size.y) * app_height };


		Vector2 ndc_click_pos = { (2 * screen_pos.x / app_width) - 1, 1 - 2 * screen_pos.y / app_height };
		Matrix4x4 inverse = (Editor::GetInstance().m_editorCamera.GetProjViewMatrix()).Inverse();
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
		Vector4 clip = Editor::GetInstance().m_editorCamera.GetProjViewMatrix() * world_pos;
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
		//Sorts them based on position.z
		auto scene = FlexECS::Scene::GetActiveScene();
		for (auto entity : scene->CachedQuery<Position, Rotation, Scale, Transform, Sprite>()) //you probably only wanna click on things that are rendered
		{
			auto& active = entity.GetComponent<Transform>()->is_active;
			if (!active) continue;

			//auto& pos = entity.GetComponent<Position>()->position;
			//auto& scale = entity.GetComponent<Scale>()->scale;
			//float rotation = entity.GetComponent<Rotation>()->rotation.z;
			auto& transform = entity.GetComponent<Transform>()->transform;

			auto corners = ComputeOBBCorners(transform);

			Vector2 max_point = corners[0];
			Vector2 min_point = corners[0];
			for (int i = 1; i < 4; i++)
			{
				min_point.x = std::min(min_point.x, corners[i].x);
				min_point.y = std::min(min_point.y, corners[i].y);
				max_point.x = std::max(max_point.x, corners[i].x);
				max_point.y = std::max(max_point.y, corners[i].y);
			}

			if (mouse_world_pos.x >= min_point.x &&
					mouse_world_pos.x <= max_point.x &&
					mouse_world_pos.y >= min_point.y &&
					mouse_world_pos.y <= max_point.y)
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

			if (current_selected != FlexECS::Entity::Null)
			{
				if (!current_selected.HasComponent<Position>())
				{
					clicked_entity = clicked_entities.begin()->first;
					return clicked_entity;
				}

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

	void SceneView::DraggingEntitySelect()
	{
		if (!m_dragging_selection)
		{
			m_drag_selection_start_point = ImGui::GetMousePos();
			m_dragging_selection = true;
		}

		ImVec2 current_mouse_pos = ImGui::GetMousePos();

		//Display an imgui rect around the sprite so we know where we are clicking, at least
		ImRect bounding_box(m_drag_selection_start_point, current_mouse_pos);
		ImGui::GetWindowDrawList()->AddRectFilled(bounding_box.Min, bounding_box.Max, IM_COL32(108, 121, 142, 100));
		ImGui::GetWindowDrawList()->AddRect(bounding_box.Min, bounding_box.Max, IM_COL32(175, 179, 184, 220), 0.0f, 0, 2.0f);

	}

	void SceneView::FindEntitiesInSelection()
	{
		std::vector<FlexECS::Entity> selected_entities;

		ImVec2 current_mouse_pos = ImGui::GetMousePos();
		Vector2 pt1 = ScreenToWorld(current_mouse_pos);
		Vector2 pt2 = ScreenToWorld(m_drag_selection_start_point);

		Vector2 max_point = { std::max(pt1.x, pt2.x), std::max(pt1.y, pt2.y) };
		Vector2 min_point = { std::min(pt1.x, pt2.x), std::min(pt1.y, pt2.y) };

		auto scene = FlexECS::Scene::GetActiveScene();
		for (auto entity : scene->CachedQuery<Position, Rotation, Scale, Transform, Sprite>()) //you probably only wanna click on things that are rendered
		{
			auto& active = entity.GetComponent<Transform>()->is_active;
			if (!active) continue;

			//auto& pos = entity.GetComponent<Position>()->position;
			//auto& scale = entity.GetComponent<Scale>()->scale;
			//float rotation = entity.GetComponent<Rotation>()->rotation.z;
			auto& transform = entity.GetComponent<Transform>()->transform;

			Vector2 entity_center = { transform.m30, transform.m31 };

			if (entity_center.x >= min_point.x &&
					entity_center.x <= max_point.x &&
					entity_center.y >= min_point.y &&
					entity_center.y <= max_point.y)
			{
				selected_entities.push_back(entity);
			}
		}
		if (selected_entities.size() > 0)
		{
			Editor::GetInstance().GetSystem<SelectionSystem>()->ClearSelection();
			for (auto e : selected_entities)
			{
				Editor::GetInstance().GetSystem<SelectionSystem>()->AddSelectedEntity(e);
			}
		}
	}

	void SceneView::HandleMouseAndKeyboardEvents()
	{
		if (ImGui::IsWindowFocused())
		{
			if (ImGui::IsMouseReleased(0) && ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
			{
				if (!m_gizmo_hovered &&
						std::abs(ImGui::GetMousePos().x - m_drag_selection_start_point.x) < 10 &&
						std::abs(ImGui::GetMousePos().y - m_drag_selection_start_point.y) < 10)
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
			else if (ImGui::IsMouseDown(0) && !m_gizmo_hovered && !m_dragging_gizmo)
			{
				DraggingEntitySelect();
			}
			else if (!ImGui::IsMouseDown(0) && m_dragging_selection && !m_gizmo_hovered && !m_dragging_gizmo)
			{
				FindEntitiesInSelection();
				m_dragging_selection = false;
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

			if (ImGui::IsKeyDown(ImGuiKey_LeftShift) && ImGui::IsKeyPressed(ImGuiKey_T))
			{
				m_display_cam_bounds = !m_display_cam_bounds;
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
		m_dragging_gizmo = false;

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

			//auto& entity_transform = selected_entity.GetComponent<Transform>()->transform;
			auto& entity_position = selected_entity.GetComponent<Position>()->position;
			auto& entity_scale = selected_entity.GetComponent<Scale>()->scale;
			auto& entity_rotation = selected_entity.GetComponent<Rotation>()->rotation;
			auto entity_transform = selected_entity.GetComponent<Transform>()->transform;


			//Find out where on the screen to draw the gizmos
			//Take entity position and convert it back to screen position
			ImVec2 gizmo_origin_pos = WorldToScreen(entity_position);

			auto corners = ComputeOBBCorners(entity_transform);

			Vector2 max_point = corners[0];
			Vector2 min_point = corners[0];
			for (int i = 1; i < 4; i++)
			{
				min_point.x = std::min(min_point.x, corners[i].x);
				min_point.y = std::min(min_point.y, corners[i].y);
				max_point.x = std::max(max_point.x, corners[i].x);
				max_point.y = std::max(max_point.y, corners[i].y);
			}

			//Display an imgui rect around the sprite so we know where we are clicking, at least
			ImRect bounding_box(WorldToScreen(min_point), WorldToScreen(max_point));
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
					m_dragging_gizmo = true;
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
					m_dragging_gizmo = true;
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
					m_dragging_gizmo = true;
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
				pos_change.x *= Editor::GetInstance().m_editorCamera.GetOrthoWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				pos_change.y *= Editor::GetInstance().m_editorCamera.GetOrthoHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
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
					m_dragging_gizmo = true;
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
					m_dragging_gizmo = true;
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
					m_dragging_gizmo = true;
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
					float modifier = entity_scale.x / 50.0f;
					modifier = std::max(modifier, 0.01f);
					scale_change.x = value * modifier;
					scale_change.y = value * modifier;
				}
				else
				{
					scale_change.y = -scale_change.y;
				}

				scale_change.x *= Editor::GetInstance().m_editorCamera.GetOrthoWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				scale_change.y *= Editor::GetInstance().m_editorCamera.GetOrthoHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
				entity_scale += Vector3(scale_change.x, scale_change.y, 0.0f);

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
				float value{};
				bool hovered;
				bool recording_ended = false;
				switch (EditorGUI::Gizmo_Rotate_Z(&value, { gizmo_origin_pos.x, gizmo_origin_pos.y }, &hovered))
				{
				case EditorGUI::GizmoStatus::START_DRAG:
					m_recorded_rotation.rotation = entity_rotation;
					break;
				case EditorGUI::GizmoStatus::DRAGGING:
					m_dragging_gizmo = true;
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					recording_ended = true;
					break;
				default:
					break;
				}
				m_gizmo_hovered = hovered;
				entity_rotation.z += value * (180 / IM_PI);

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
				//auto& entity_position = entity.GetComponent<Position>()->position;
				//auto& entity_scale = entity.GetComponent<Scale>()->scale;
				//auto& entity_rotation = entity.GetComponent<Rotation>()->rotation;
				auto& entity_transform = entity.GetComponent<Transform>()->transform;

				average_pos += { entity_transform.m30, entity_transform.m31, entity_transform.m32};

				auto corners = ComputeOBBCorners(entity_transform);

				Vector2 max_point = corners[0];
				Vector2 min_point = corners[0];
				for (int i = 1; i < 4; i++)
				{
					min_point.x = std::min(min_point.x, corners[i].x);
					min_point.y = std::min(min_point.y, corners[i].y);
					max_point.x = std::max(max_point.x, corners[i].x);
					max_point.y = std::max(max_point.y, corners[i].y);
				}

				//Display an imgui rect around the sprite so we know where we are clicking, at least
				ImRect bounding_box(WorldToScreen(min_point), WorldToScreen(max_point));
				ImGui::GetWindowDrawList()->AddRect(bounding_box.Min, bounding_box.Max, IM_COL32(255, 255, 0, 150));
			}

			average_pos /= static_cast<float>(selected_list.size());
			ImVec2 gizmo_origin_pos = WorldToScreen({ average_pos.x, average_pos.y });

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
					m_dragging_gizmo = true;
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
					m_dragging_gizmo = true;
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
					m_dragging_gizmo = true;
					break;
				case EditorGUI::GizmoStatus::END_DRAG:
					//End recording, new position
					recording_ended = true;
					break;
				default:
					break;
				}

				m_gizmo_hovered = right || up || xy;

				pos_change.x *= Editor::GetInstance().m_editorCamera.GetOrthoWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				pos_change.y *= Editor::GetInstance().m_editorCamera.GetOrthoHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);

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
					m_dragging_gizmo = true;
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
					m_dragging_gizmo = true;
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
					m_dragging_gizmo = true;
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
					float modifier = 0.5f;
					scale_change.x = value * modifier;
					scale_change.y = value * modifier;
				}
				else
				{
					scale_change.y = -scale_change.y;
				}

				scale_change.x *= Editor::GetInstance().m_editorCamera.GetOrthoWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				scale_change.y *= Editor::GetInstance().m_editorCamera.GetOrthoHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);

				for (FlexECS::Entity entity : selected_list)
				{
					entity.GetComponent<Scale>()->scale += Vector3(scale_change.x, scale_change.y, 0.0f);
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
					m_dragging_gizmo = true;
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

		Window::FrameBufferManager.SetCurrentFrameBuffer("Scene");
		GLuint texture = Window::FrameBufferManager.GetCurrentFrameBuffer()->GetColorAttachment();

		ImGui::Image((ImTextureID)static_cast<uintptr_t>(texture),
			m_viewport_size, ImVec2(0, 1), ImVec2(1, 0));

		HandleMouseAndKeyboardEvents();
		DrawGizmos();
		MoveEditorCam();
		DisplayMainCamBounds();

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
			new_entity.AddComponent<Scale>({ Vector2::One });
			new_entity.AddComponent<Transform>({});
			new_entity.AddComponent<Sprite>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(image_key) });
			EditorGUI::EndPayloadReceiver();
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

				camera_pos_change.x *= Editor::GetInstance().m_editorCamera.GetOrthoWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				camera_pos_change.y *= Editor::GetInstance().m_editorCamera.GetOrthoHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
				m_camera_position -= Vector3(camera_pos_change.x, camera_pos_change.y, 0.0f);
				Editor::GetInstance().m_editorCamera.SetViewMatrix(m_camera_position);
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

				camera_pos_change.x *= Editor::GetInstance().m_editorCamera.GetOrthoWidth() / ((m_viewport_size.x == 0.0f) ? 1.0f : m_viewport_size.x);
				camera_pos_change.y *= Editor::GetInstance().m_editorCamera.GetOrthoHeight() / ((m_viewport_size.y == 0.0f) ? 1.0f : m_viewport_size.y);
				m_camera_position -= Vector3(camera_pos_change.x, camera_pos_change.y, 0.0f);
				Editor::GetInstance().m_editorCamera.SetViewMatrix(m_camera_position);
			}
			else if (ImGui::IsMouseReleased(1))
			{
				m_dragging_camera = false;
			}


			//Camera zooming
			float baseAspectRatio = Editor::GetInstance().m_editorCamera.GetOrthoWidth() / Editor::GetInstance().m_editorCamera.GetOrthoHeight();  // Base aspect ratio (can be easily adjusted)
			float zoomSpeed = 40.0f;      // Adjust this for faster/slower zoom
			float minZoom = 50.0f;       // Minimum orthographic width
			//float maxZoom = 5000.0f;      // Maximum orthographic width

			ImGuiIO& io = ImGui::GetIO();
			if (io.MouseWheel != 0.0f)
			{
				float zoomDelta = io.MouseWheel * zoomSpeed;
				//float new_right = std::clamp(Editor::GetInstance().m_editorCamera.GetOrthoWidth() - zoomDelta, minZoom, maxZoom);
				float new_right = std::max(Editor::GetInstance().m_editorCamera.GetOrthoWidth() - zoomDelta, minZoom);
				float new_top = new_right / baseAspectRatio;
				Editor::GetInstance().m_editorCamera.SetOrthographic(-new_right / 2, new_right / 2, -new_top / 2, new_top / 2);
				Editor::GetInstance().m_editorCamera.Update();
			}
		}
	}

	void SceneView::DisplayMainCamBounds()
	{
		if (!m_display_cam_bounds) return;

		FlexECS::Entity main_camera = CameraManager::GetMainGameCameraID();
		if (!CameraManager::has_main_camera)
		{
			return;
		}

		const auto& cam_data = main_camera.GetComponent<Camera>();
		float cam_width = cam_data->GetOrthoWidth();
		float cam_height = cam_data->GetOrthoHeight();

		const auto& pos = main_camera.GetComponent<Position>()->position;

		//construct bounding box for camera
		ImVec2 gizmo_origin_pos = WorldToScreen({ pos.x, pos.y });

		Vector2 max_point = { pos.x + cam_width / 2, pos.y + cam_height / 2 };
		Vector2 min_point = { pos.x - cam_width / 2, pos.y - cam_height / 2 };

		ImRect bounding_box(WorldToScreen(min_point), WorldToScreen(max_point));
		ImGui::GetWindowDrawList()->AddRect(bounding_box.Min, bounding_box.Max, IM_COL32(100, 160, 100, 160), 0.0f, 0, 1.5f);

	}
}
