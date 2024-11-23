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
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "editorgui.h"

using namespace FlexEngine;
namespace ChronoDrift
{
	int EditorGUI::m_id = 0;


	/*!***************************************************************************
	* Component viewers
	******************************************************************************/

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

	void EditorGUI::DragFloat1(float& data, std::string title, float width, float drag_speed)
	{
		PushID();
		ImGui::PushItemWidth(width);
		ImGui::Text(title.c_str()); ImGui::SameLine();
		ImGui::DragFloat("", &data, drag_speed);
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
			entity_name = FlexECS::Manager::GetInstance().GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<FlexEngine::FlexECS::Scene::StringIndex>());
		}

		ImGui::BeginGroup();
		ImGui::Text(title.c_str()); ImGui::SameLine();
		if (ImGui::Button(entity_name.c_str()) && entity != FlexECS::Entity::Null)
		{
			Editor::GetInstance().SelectEntity(entity);
		}

		if (auto payload = StartPayloadReceiver<FlexECS::EntityID>(PayloadTags::ENTITY))
		{
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
			EndPayloadReceiver();
		}

		if (filename != "(no sprite)")
		{
			std::string asset_key = current_texture.string();
			AssetVariant* asset_ptr = AssetManager::Get(asset_key);
			if (asset_ptr)
			{
				Asset::Texture& texture = std::get<Asset::Texture>(*asset_ptr);
				ImGui::Image(texture.GetTextureImGui(), ImVec2(60.0f, 60.0f));
			}
		}

		PopID();
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

	void EditorGUI::Color3(Vector3& data, std::string title)
	{
		PushID();
		ImGui::ColorEdit3(title.c_str(), data.data);
		PopID();
	}

	void EditorGUI::EditableTextField(std::string& data, std::string title)
	{
		PushID();
		char text_buffer[128];
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