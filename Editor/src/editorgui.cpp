/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// editorgui.cpp
//
// This is the Encapsulation of ImGUI functions to be of ease for other programmers to use.
// Lemme know if you need anything, and I'll try to create something easy to use!
//
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "editorgui.h"

using namespace FlexEngine;
namespace Editor
{
	#if 1
	int EditorGUI::m_id = 0;


	/*!***************************************************************************
	* Component viewers ( Inspector )
	******************************************************************************/

	#pragma region Inspector

	void EditorGUI::DragFloat2(Vector2& data, std::string title,
		//std::string label1, std::string label2, 
		float width, float drag_speed)
	{
		if (title != "")
		{
			TextField(title);
		}
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::DragFloat("", &data.x, drag_speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		PopID();
		PushID();
		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::PushItemWidth(width);
		ImGui::DragFloat("", &data.y, drag_speed);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorGUI::DragFloat3(Vector3& data, std::string title,
		//std::string label1, std::string label2, std::string label3,
		float width, float drag_speed)
	{
		if (title != "")
		{
			TextField(title);
		}
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text("X"); ImGui::SameLine();
		ImGui::DragFloat("", &data.x, drag_speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		PopID();
		PushID();
		ImGui::Text("Y"); ImGui::SameLine();
		ImGui::PushItemWidth(width);
		ImGui::DragFloat("", &data.y, drag_speed);
		ImGui::PopItemWidth();
		ImGui::SameLine();
		PopID();
		PushID();
		ImGui::Text("Z"); ImGui::SameLine();
		ImGui::PushItemWidth(width);
		ImGui::DragFloat("", &data.z, drag_speed);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorGUI::Float1(float& data, std::string title, float width)
	{
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text(title.c_str()); ImGui::SameLine();
		ImGui::InputFloat("", &data, 0.0f, 0.0f, "%.3f", ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorGUI::DragFloat1(float& data, std::string title, float width, float drag_speed)
	{
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text(title.c_str()); ImGui::SameLine();
		ImGui::DragFloat("", &data, drag_speed);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorGUI::Int1(int& data, std::string title, float width)
	{
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text(title.c_str()); ImGui::SameLine();
		ImGui::InputInt("", &data, 0, 0, ImGuiInputTextFlags_ReadOnly);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorGUI::DragInt(int& data, std::string title, float width, float drag_speed)
	{
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text(title.c_str()); ImGui::SameLine();
		ImGui::DragInt("", &data, drag_speed);
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorGUI::DragGLuint(GLuint& data, std::string title, float width, float drag_speed)
	{
		int tempData = static_cast<int>(data);
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text(title.c_str()); ImGui::SameLine();
		if (ImGui::DragInt("", &tempData, drag_speed)) {
			data = static_cast<GLuint>(tempData);
		}
		ImGui::PopItemWidth();
		PopID();
	}

	void EditorGUI::CreateCheckbox(bool& value, std::string title)
	{
		PushID();
		ImGui::Text(title.c_str());  // Display the title label
		ImGui::SameLine();           // Place the checkbox next to the title
		ImGui::Checkbox("", &value); // Create the checkbox widget
		PopID();
	}

	void EditorGUI::EntityReference(FlexECS::Entity& entity, std::string title)
	{
		PushID();
		std::string entity_name{ "(no entity)" };
		if (entity != FlexECS::Entity::Null)
		{
			entity_name = FLX_STRING_GET(*entity.GetComponent<EntityName>());
		}

		ImGui::BeginGroup();
		ImGui::Text(title.c_str()); ImGui::SameLine();
		if (ImGui::Button(entity_name.c_str()) && entity != FlexECS::Entity::Null)
		{
			Editor::GetInstance().GetSystem<SelectionSystem>()->SelectEntity(entity);
		}

		if (auto payload = StartPayloadReceiver<FlexECS::EntityID>(PayloadTags::ENTITY))
		{
			//Warning: needs a way to check that the dropped entity is not itself.
			FlexECS::Entity dropped_entity = *payload;
			entity = dropped_entity;
			EndPayloadReceiver();
		}

		ImGui::EndGroup();
		PopID();
	}

	void EditorGUI::ShaderPath(std::string& path, std::string title)
	{
		PushID();

		std::filesystem::path current_texture = path;
		std::string filename = current_texture.filename().string();
		if (filename == "") filename = "(no shader)";

		ImGui::Text("Shader");
		ImGui::SameLine();
		ImGui::Button(filename.c_str());

		if (const char* data = StartPayloadReceiver<const char>(PayloadTags::SHADER))
		{
			std::string new_file_path(data);
			path = new_file_path;
			EndPayloadReceiver();
		}

		PopID();
	}

	void EditorGUI::TexturePath(std::string& path, std::string title)
	{
		PushID();
		
		std::filesystem::path current_texture = path;
		std::string filename = current_texture.filename().string();
		if (filename == "") filename = "(no sprite)";

		ImGui::Text("Sprite");
		ImGui::SameLine();
		ImGui::Button(filename.c_str());

		if (const char* data = StartPayloadReceiver<const char>(PayloadTags::IMAGE))
		{
			std::string new_file_path(data);
			path = new_file_path;
			current_texture = path;
			EndPayloadReceiver();
		}

		if (filename != "(no sprite)")
		{
			std::string asset_key = current_texture.string();
      
			//Should find a way to be able to display both sprite and spritesheet,
			//Since animator hijacks the sprite
			if (current_texture.extension() != ".flxspritesheet")
			{
				Asset::Texture& texture = AssetManager::Get<Asset::Texture>(asset_key);
				ImGui::Image(texture.GetTextureImGui(), ImVec2(60.0f, 60.0f));
			}
		}

		PopID();
	}

	void EditorGUI::SpritesheetPath(std::string& path, std::string title)
	{
		PushID();

		std::filesystem::path current_texture = path;
		std::string filename = current_texture.filename().string();
		if (filename == "") filename = "(no spritesheet)";

		ImGui::Text("Spritesheet");
		ImGui::SameLine();
		ImGui::Button(filename.c_str());
		
		//Drag drop a spritesheet into inspector
		if (const char* data = StartPayloadReceiver<const char>(PayloadTags::SPRITESHEET))
		{
			std::string new_file_path(data);
			path = new_file_path;
			current_texture = path;
			EndPayloadReceiver();
		}

		//TODO: display image of spritesheet
		//if (filename != "(no spritesheet)")
		//{
		//	std::string asset_key = current_texture.string();
		//	Asset::Spritesheet& texture = AssetManager::Get<Asset::Spritesheet>(asset_key);
		//}

		PopID();
	}

	void EditorGUI::StyleColorPush_Selected()
	{
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.15f, 0.5f, 0.75f, 1.0f));
	}

	void EditorGUI::StyleColorPop_Selected()
	{
		ImGui::PopStyleColor(3);
	}

	void EditorGUI::AudioPath(std::string& path, std::string title)
	{
		PushID();

		std::filesystem::path current_texture = path;
		std::string filename = current_texture.filename().string();
		if (filename == "") filename = "(no audio)";

		ImGui::Text(title.c_str());
		ImGui::SameLine();
		ImGui::Button(filename.c_str());

		if (const char* data = StartPayloadReceiver<const char>(PayloadTags::AUDIO))
		{
			std::string new_file_path(data);
			path = new_file_path;
			EndPayloadReceiver();
		}

		PopID();
	}

	void EditorGUI::FontPath(std::string& path, std::string title)
	{
		PushID();
		std::filesystem::path current_font = path;
		std::string filename = current_font.filename().string();
		if (filename == "") filename = "(no font)";
		ImGui::Text(title.c_str());
		ImGui::SameLine();


		const std::unordered_map<std::string, std::string>& saved_fonts = Editor::GetInstance().GetPanel<AssetBrowser>()->GetLoadedFontsList();
		std::vector<std::string> font_names;
		std::vector<const char*> font_ptrs;
		font_names.reserve(saved_fonts.size());
		font_ptrs.reserve(saved_fonts.size());

		int current_font_index = 0; // Index of selected item
		int i = 0;
		for (const auto& pair : saved_fonts)
		{
			font_names.push_back(pair.first);  // Store keys as std::string
			if (filename == pair.first)
			{
				current_font_index = i;
			}
			++i;
		}
		for (const auto& key : font_names)
		{
			font_ptrs.push_back(key.c_str());  // Store const char* pointers
		}


		if (ImGui::Combo("", &current_font_index, font_ptrs.data(), static_cast<int>(font_ptrs.size())))
		{
			// Action when selection changes
			path = saved_fonts.at(font_names[current_font_index]);
		}

		PopID();
	}

	void EditorGUI::VideoPath(std::string& path, std::string title)
	{
		PushID();

		std::filesystem::path current_texture = path;
		std::string filename = current_texture.filename().string();
		if (filename == "") filename = "(no video)";

		ImGui::Text(title.c_str());
		ImGui::SameLine();
		ImGui::Button(filename.c_str());

		if (const char* data = StartPayloadReceiver<const char>(PayloadTags::VIDEO))
		{
			std::string new_file_path(data);
			path = new_file_path;
			EndPayloadReceiver();
		}

		PopID();
	}

	void EditorGUI::Color3(Vector3& data, std::string title)
	{
		PushID();
		ImGui::ColorEdit3(title.c_str(), data.data);
		PopID();
	}

	void EditorGUI::EditableTextField(std::string& data, std::string title)
	{
		PushID();
		char text_buffer[1024];
		strncpy_s(text_buffer, data.c_str(), sizeof(text_buffer));
		text_buffer[sizeof(text_buffer) - 1] = '\0';
		ImGui::Text(title.c_str());
		ImGui::SameLine();
		PushID();
		if (ImGui::InputText("##", text_buffer, IM_ARRAYSIZE(text_buffer)))
		{
			data = std::string(text_buffer);
		}
		PopID();
		PopID();
	}

	void EditorGUI::TextField(const std::string& data)
	{
		PushID();
		ImGui::Text(data.c_str());
		PopID();
	}

	void EditorGUI::Checkbox(bool& data, std::string title)
	{
		PushID();
		ImGui::Checkbox(title.c_str(), &data);
		PopID();
	}

	void EditorGUI::Mat44(FlexEngine::Matrix4x4& data, std::string title)
	{
		PushID();
		
		// Create a label for the matrix
		ImGui::Text("%s", title.c_str());

		ImGui::PushItemWidth(60.0f);

		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				PushID();
				ImGui::DragFloat("##", &data(row, col), 0.1f);
				PopID();

				if (col < 3) ImGui::SameLine();
			}
		}
		ImGui::PopItemWidth();

		PopID();
	}

	void EditorGUI::TextAlignment(std::pair<int, int>& alignment)
	{
		PushID();

		const char* h_options[] = { "Center", "Left", "Right" };
		const char* v_options[] = { "Middle", "Top", "Bottom" };
		int h = alignment.first;
		int v = alignment.second;

		ImGui::Text("AlignmentX");
		ImGui::SameLine();
		if (ImGui::Combo("##FontAlignmentH", &h, h_options, IM_ARRAYSIZE(h_options)))
		{
			alignment.first = h;
		}		

		ImGui::Text("AlignmentY");
		ImGui::SameLine();
		if (ImGui::Combo("##FontAlignmentV", &v, v_options, IM_ARRAYSIZE(v_options)))
		{
			alignment.second = v;
		}
		PopID();
	}

	#pragma endregion

	/*!***************************************************************************
	* payloads
	******************************************************************************/
	bool EditorGUI::StartPayload(PayloadTags tag, const void* data, size_t data_size, std::string tooltip)
	{
		bool result = ImGui::SetDragDropPayload(GetPayloadTagString(tag), data, data_size);
		ImGui::Text("%s", tooltip.c_str());
		return result;
	}

	void EditorGUI::EndPayload()
	{
		ImGui::EndDragDropSource();
	}

	void EditorGUI::EndPayloadReceiver()
	{
		ImGui::EndDragDropTarget();
	}


	/*!***************************************************************************
	* Gizmos
	******************************************************************************/
	#pragma region Gizmos

	static constexpr float length = 40.0f;
	static constexpr float thickness = 12.0f;
	static constexpr float padding = 7.0f;
	static constexpr float half_thickness = thickness / 2;
	static constexpr float arrow_neck_size = 2.0f;
	static constexpr float length_scale = 1.25f;
	static constexpr int arrow_gizmo_point_count = 7;
	static constexpr int box_gizmo_point_count = 8;

	static constexpr float circle_size = 60.0f;
	static constexpr float circle_thickness = 3.0f;
	static constexpr int circle_segments = 64;

	static constexpr ImU32 red_gizmo_color						= IM_COL32(255, 0, 0, 225);
	static constexpr ImU32 red_gizmo_hovered_color		= IM_COL32(191, 0, 0, 225); 
	static constexpr ImU32 green_gizmo_color							= IM_COL32(0, 230, 0, 225);
	static constexpr ImU32 green_gizmo_hovered_color_gizmo_color			= IM_COL32(0, 170, 0, 225); 
	static constexpr ImU32 blue_gizmo_color					= IM_COL32(0, 55, 255, 225);
	static constexpr ImU32 blue_gizmo_hovered_color = IM_COL32(255, 165, 0, 225);
	//constexpr ImU32 blue_gizmo_hovered_color	= IM_COL32(0, 0, 191, 225);

	EditorGUI::GizmoStatus EditorGUI::GizmoTranslateRight(float* p_x_axis_change, const ImVec2& origin, bool* hovering)
	{
		static bool manipulated = false;
		GizmoStatus status = GizmoStatus::NO_INTERACTION;
		// Origin refers to the center of the entity.
		//We set imgui cursor pos to start from this place, then we adjust
		//The gizmos position starting from that position.
		ImGui::SetCursorPos(origin);
		ImVec2 pos = ImGui::GetCursorPos();
		pos.x += padding;
		pos.y -= half_thickness;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		//Coordinates of arrow polygon
		//Drawing Polygons with Imgui must have a clockwise winding order.
		ImVec2 arrow_gizmo[arrow_gizmo_point_count] =
		{
			ImVec2{ pos.x + length								, pos.y + half_thickness - arrow_neck_size },
			ImVec2{ pos.x + length								, pos.y },
			ImVec2{ pos.x + length * length_scale , pos.y + half_thickness},
			ImVec2{ pos.x + length								, pos.y + thickness},
			ImVec2{ pos.x + length								, pos.y + half_thickness + arrow_neck_size },
			ImVec2{ pos.x													, pos.y + half_thickness + arrow_neck_size },
			ImVec2{ pos.x													, pos.y + half_thickness - arrow_neck_size }
		};

		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems) return status;

		ImGuiID id = window->GetID("Gizmo_Translate_Right");
		ImRect bb({ pos.x, pos.y }, { pos.x + length * length_scale, pos.y + thickness });

		//Reserve space for an item
		ImGui::ItemSize(bb);
		ImGui::ItemAdd(bb, id);

		//Check button is clicked
		bool hovered, held;
		bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		*hovering = hovered;

		ImU32 gizmo_color = (hovered || held) ? red_gizmo_hovered_color : red_gizmo_color;

		if (held)
		{
			manipulated = true;
			status = (ImGui::IsMouseClicked(0)) ? GizmoStatus::START_DRAG : GizmoStatus::DRAGGING;
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			*p_x_axis_change += drag_delta.x;
		}
		else if (!held)
		{
			if (manipulated)
			{
				manipulated = false;
				status = GizmoStatus::END_DRAG;
			}
		}
		if (released)
		{
			//Add to undo list
		}

		draw_list->AddConvexPolyFilled(arrow_gizmo, arrow_gizmo_point_count, gizmo_color);
		return status;
	}
	//Note: ImGui dragdelta.y is +ve when you move mouse downwards
	EditorGUI::GizmoStatus EditorGUI::GizmoTranslateUp(float* p_y_axis_change, const ImVec2& origin, bool* hovering)
	{
		static bool manipulated = false;
		GizmoStatus status = GizmoStatus::NO_INTERACTION;
		ImGui::SetCursorPos(origin);
		ImVec2 pos = ImGui::GetCursorPos();
		pos.x -= half_thickness;
		pos.y -= padding;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 arrow_gizmo[arrow_gizmo_point_count] =
		{
			ImVec2{ pos.x + half_thickness - arrow_neck_size , pos.y - length},
			ImVec2{ pos.x																		 , pos.y - length},
			ImVec2{ pos.x + half_thickness									 , pos.y - length * length_scale},
			ImVec2{ pos.x + thickness												 , pos.y - length},
			ImVec2{ pos.x + half_thickness + arrow_neck_size , pos.y - length},
			ImVec2{ pos.x + half_thickness + arrow_neck_size , pos.y},
			ImVec2{ pos.x + half_thickness - arrow_neck_size , pos.y}
		};

		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems) return status;

		ImGuiID id = window->GetID("Gizmo_Translate_Up");
		ImRect bb({ pos.x, pos.y - length * length_scale }, { pos.x + thickness, pos.y });

		ImGui::ItemSize(bb);
		ImGui::ItemAdd(bb, id);

		bool hovered, held;
		bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		*hovering = hovered;

		ImU32 gizmo_color = (hovered || held) ? green_gizmo_hovered_color_gizmo_color : green_gizmo_color;

		if (held)
		{
			manipulated = true;
			status = (ImGui::IsMouseClicked(0)) ? GizmoStatus::START_DRAG : GizmoStatus::DRAGGING;
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			*p_y_axis_change += drag_delta.y;
		}
		else if (!held)
		{
			if (manipulated)
			{
				manipulated = false;
				status = GizmoStatus::END_DRAG;
			}
		}
		if (released)
		{
			//Unforunately not consistent
		}

		draw_list->AddConvexPolyFilled(arrow_gizmo, arrow_gizmo_point_count, gizmo_color);
		return status;
	}

	EditorGUI::GizmoStatus EditorGUI::GizmoTranslateXY(float* p_x_axis_change, float* p_y_axis_change, const ImVec2& origin, bool* hovering)
	{
		static bool manipulated = false;
		GizmoStatus status = GizmoStatus::NO_INTERACTION;
		ImGui::SetCursorPos(origin);
		ImVec2 pos = ImGui::GetCursorPos();
		pos.x += 12.0f;
		pos.y -= 12.0f;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems) return status;

		ImGuiID id = window->GetID("Gizmo_XY_Rect");
		ImRect bb({ pos.x, pos.y - 18.0f }, { pos.x + 18.0f, pos.y });

		ImGui::ItemSize(bb);
		ImGui::ItemAdd(bb, id);

		//ImGui::GetForegroundDrawList()->AddRect(bb.Min, bb.Max, IM_COL32(255, 255, 0, 255));

		bool hovered, held;
		bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		*hovering = hovered;

		ImU32 gizmo_color = (hovered || held) ? blue_gizmo_hovered_color : blue_gizmo_color;

		if (held)
		{
			manipulated = true;
			status = (ImGui::IsMouseClicked(0)) ? GizmoStatus::START_DRAG : GizmoStatus::DRAGGING;
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			*p_x_axis_change += drag_delta.x;
			*p_y_axis_change += drag_delta.y;
		}
		else if (!held)
		{
			if (manipulated)
			{
				manipulated = false;
				status = GizmoStatus::END_DRAG;
			}
		}
		if (released)
		{

		}

		draw_list->AddRectFilled(bb.Min, bb.Max, gizmo_color);
		return status;
	}


	EditorGUI::GizmoStatus EditorGUI::Gizmo_Scale_X(float* p_x_axis_change, const ImVec2& origin, bool* hovering)
	{
		static bool manipulated = false;
		GizmoStatus status = GizmoStatus::NO_INTERACTION;
		ImGui::SetCursorPos(origin);
		ImVec2 pos = ImGui::GetCursorPos();
		pos.x += padding;
		pos.y -= half_thickness;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 box_gizmo[box_gizmo_point_count] =
		{
			ImVec2{ pos.x + length								, pos.y + half_thickness - arrow_neck_size },
			ImVec2{ pos.x + length								, pos.y					  },
			ImVec2{ pos.x + length * length_scale , pos.y 				  },
			ImVec2{ pos.x + length * length_scale , pos.y + thickness	  },
			ImVec2{ pos.x + length								, pos.y + thickness	  },
			ImVec2{ pos.x + length								, pos.y + half_thickness + arrow_neck_size },
			ImVec2{ pos.x													, pos.y + half_thickness + arrow_neck_size },
			ImVec2{ pos.x													, pos.y + half_thickness - arrow_neck_size }
		};

		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems) return status;

		ImGuiID id = window->GetID("Gizmo_Scale_Right");
		ImRect bb({ pos.x, pos.y }, { pos.x + length * length_scale, pos.y + thickness });

		ImGui::ItemSize(bb);
		ImGui::ItemAdd(bb, id);

		bool hovered, held;
		bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		*hovering = hovered;

		ImU32 gizmo_color = (hovered || held) ? red_gizmo_hovered_color : red_gizmo_color;

		if (held)
		{
			manipulated = true;
			status = (ImGui::IsMouseClicked(0)) ? GizmoStatus::START_DRAG : GizmoStatus::DRAGGING;
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			*p_x_axis_change += drag_delta.x;
		}
		else if (!held)
		{
			if (manipulated)
			{
				manipulated = false;
				status = GizmoStatus::END_DRAG;
			}
		}
		if (released)
		{
			//Unforunately not consistent
		}

		draw_list->AddConvexPolyFilled(box_gizmo, box_gizmo_point_count, gizmo_color);
		return status;
	}

	EditorGUI::GizmoStatus EditorGUI::Gizmo_Scale_Y(float* p_y_axis_change, const ImVec2& origin, bool* hovering)
	{
		static bool manipulated = false;
		GizmoStatus status = GizmoStatus::NO_INTERACTION;
		ImGui::SetCursorPos(origin);
		ImVec2 pos = ImGui::GetCursorPos();
		pos.x -= half_thickness;
		pos.y -= padding;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImVec2 box_gizmo[box_gizmo_point_count] =
		{
			ImVec2{ pos.x + half_thickness - 1.f						 , pos.y - length},
			ImVec2{ pos.x																		 , pos.y - length},
			ImVec2{ pos.x 																	 , pos.y - length * length_scale},
			ImVec2{ pos.x + thickness												 , pos.y - length * length_scale},
			ImVec2{ pos.x + thickness 											 , pos.y - length},
			ImVec2{ pos.x + half_thickness + arrow_neck_size , pos.y - length},
			ImVec2{ pos.x + half_thickness + arrow_neck_size , pos.y},
			ImVec2{ pos.x + half_thickness - arrow_neck_size , pos.y}
		};

		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems) return status;

		ImGuiID id = window->GetID("Gizmo_Scale_Up");
		ImRect bb({ pos.x, pos.y - length * length_scale }, { pos.x + thickness, pos.y });

		ImGui::ItemSize(bb);
		ImGui::ItemAdd(bb, id);

		bool hovered, held;
		bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		*hovering = hovered;

		ImU32 gizmo_color = (hovered || held) ? green_gizmo_hovered_color_gizmo_color : green_gizmo_color;

		if (held)
		{
			manipulated = true;
			status = (ImGui::IsMouseClicked(0)) ? GizmoStatus::START_DRAG : GizmoStatus::DRAGGING;
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
			*p_y_axis_change += drag_delta.y;
		}
		else if (!held)
		{
			if (manipulated)
			{
				manipulated = false;
				status = GizmoStatus::END_DRAG;
			}
		}
		if (released)
		{
			//Unforunately not consistent
		}

		draw_list->AddConvexPolyFilled(box_gizmo, box_gizmo_point_count, gizmo_color);
		return status;
	}

	EditorGUI::GizmoStatus EditorGUI::Gizmo_Scale_XY(float* value, const ImVec2& origin, bool* hovering)
	{
		static bool manipulated = false;
		GizmoStatus status = GizmoStatus::NO_INTERACTION;
		ImGui::SetCursorPos(origin);
		ImVec2 pos = ImGui::GetCursorPos();
		pos.x -= half_thickness;
		pos.y -= half_thickness;
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGuiWindow* window = ImGui::GetCurrentWindow();

		if (window->SkipItems) return status;

		ImGuiID id = window->GetID("Gizmo_XY_Rect");
		ImRect bb({ pos.x, pos.y }, { pos.x + thickness, pos.y + thickness });

		ImGui::ItemSize(bb);
		ImGui::ItemAdd(bb, id);

		bool hovered, held;
		bool released = ImGui::ButtonBehavior(bb, id, &hovered, &held);

		*hovering = hovered;

		ImU32 gizmo_color = (hovered || held) ? blue_gizmo_hovered_color : blue_gizmo_color;

		if (held)
		{
			manipulated = true;
			status = (ImGui::IsMouseClicked(0)) ? GizmoStatus::START_DRAG : GizmoStatus::DRAGGING;
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
			if (drag_delta.x != 0 || drag_delta.y != 0) ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);

			*value = drag_delta.x;	//personally it feels better this way, who even scales by moving the mouse upwards instead of right?
		}
		else if (!held)
		{
			if (manipulated)
			{
				manipulated = false;
				status = GizmoStatus::END_DRAG;
			}
		}
		if (released)
		{
			//Unforunately not consistent
		}

		draw_list->AddRectFilled(bb.Min, bb.Max, gizmo_color);
		return status;
	}


	struct RotationInfo
	{
		ImVec2 initial_click_pos{};
		float initial_angle{};	//at the start of click pos
		float manipulated_angle{}; //track current angle as we are spinning around
		bool is_manipulating{ false };
	};
	EditorGUI::GizmoStatus EditorGUI::Gizmo_Rotate_Z(float* value, const ImVec2& origin, bool* hovering)
	{
		static RotationInfo rotation_info;
		GizmoStatus status = GizmoStatus::NO_INTERACTION;
		ImGui::SetCursorPos(origin);
		ImVec2 pos = ImGui::GetCursorPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems) return status;

		//ImGuiID id = window->GetID("Gizmo_Rotate_Z");

		bool hovered;
		hovered = CheckCircleMouseCollision(pos, circle_size, circle_thickness);
		*hovering = hovered || rotation_info.is_manipulating;

		if (hovered)
		{
			if (ImGui::IsMouseClicked(0) && !rotation_info.is_manipulating)
			{
				//start dragging
				status = GizmoStatus::START_DRAG;
				rotation_info.is_manipulating = true;
				rotation_info.initial_click_pos = ImGui::GetMousePos();
				rotation_info.initial_angle = atan2f(rotation_info.initial_click_pos.y - origin.y,
					rotation_info.initial_click_pos.x - origin.x);
				rotation_info.manipulated_angle = rotation_info.initial_angle;
			}
		}

		if (ImGui::IsMouseReleased(0) && rotation_info.is_manipulating)
		{
			status = GizmoStatus::END_DRAG;
			//Mouse was released
			rotation_info.initial_angle = {};
			rotation_info.initial_click_pos = {};
			rotation_info.manipulated_angle = {};
			rotation_info.is_manipulating = false;
		}

		//Now compute the amount to rotate
		if (rotation_info.is_manipulating)
		{
			if (!ImGui::IsMouseClicked(0)) status = GizmoStatus::DRAGGING;
			ImVec2 current_mouse_pos = ImGui::GetMousePos();
			float current_angle = atan2f(current_mouse_pos.y - origin.y,
				current_mouse_pos.x - origin.x);
			float angle_delta = rotation_info.manipulated_angle - current_angle;
			rotation_info.manipulated_angle = current_angle;

			//normalize to between pi and -pi
			if (angle_delta > IM_PI)
				angle_delta -= 2.0f * IM_PI;
			else if (angle_delta < -IM_PI)
				angle_delta += 2.0f * IM_PI;

			*value = angle_delta;
		}

		ImU32 gizmo_color = (hovered || rotation_info.is_manipulating) ? blue_gizmo_hovered_color : blue_gizmo_color;
		draw_list->AddCircle(origin, circle_size, gizmo_color, circle_segments, circle_thickness);
		if (rotation_info.is_manipulating)
		{
			draw_list->AddCircleFilled(rotation_info.initial_click_pos, 10.0f, green_gizmo_color, circle_segments);
		}
		return status;
	}

	#pragma endregion


	/*!***************************************************************************
	* System
	******************************************************************************/

	void EditorGUI::StartFrame()
	{
		m_id = 0;
	}

	void EditorGUI::EndFrame()
	{
		m_id = 0;
	}

	int EditorGUI::PushID()
	{
		ImGui::PushID(m_id);
		return m_id++;
	}

	void EditorGUI::PopID()
	{
		ImGui::PopID();
	}

	bool EditorGUI::CheckCircleMouseCollision(ImVec2 _circle_origin, float _circle_size, float _circle_thickness)
	{
		// Detect clicks on the circle
		ImVec2 mouse_pos = ImGui::GetMousePos();
		float mouse_distance = std::sqrtf((mouse_pos.x - _circle_origin.x) * (mouse_pos.x - _circle_origin.x) +
																 (mouse_pos.y - _circle_origin.y) * (mouse_pos.y - _circle_origin.y));
		float inner_radius = _circle_size - _circle_thickness * 0.5f;
		float outer_radius = _circle_size + _circle_thickness * 0.5f;

		inner_radius -= 3.0f; //tiny tiny modifier because it just feels better like this
		outer_radius += 1.0f; 

		if (mouse_distance >= inner_radius && mouse_distance <= outer_radius)
		{
			return true;
		}
		return false;
	}

	#endif
}



//if (ImGui::BeginDragDropTarget()) 
//{
//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_PATH")) {
//		const char* dropped_path = (const char*)payload->Data;
//		std::string new_file_path(dropped_path);
//		path = new_file_path; // Store the file path in the component
//	}
//	ImGui::EndDragDropTarget();
//}