/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// inspector.cpp
//
// Inspector Panel (properties viewer) for the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#include "editor.h"
#include "hierarchyview.h"
#include "inspector.h"
#include "componentviewer.h"

using namespace FlexEngine;
using EntityName = FlexECS::Scene::StringIndex;

namespace Editor
{
	void Inspector::Init()
	{}
	void Inspector::Update()
	{}
	void Inspector::Shutdown()
	{}

	void Inspector::EditorUI()
	{
		ImGui::Begin("Inspector");

		auto scene = FlexECS::Scene::GetActiveScene();
		auto selection_system = Editor::GetInstance().GetSystem<SelectionSystem>();
		auto selected_entities = selection_system->GetSelectedEntities();

		//auto entity = Editor::GetInstance().GetSelectedEntity();

		//if (entity != FlexECS::Entity::Null)
		if (selected_entities.size() == 1)
		{
			FlexECS::Entity entity = *selected_entities.begin();
			

			std::string& name = FLX_STRING_GET(*entity.GetComponent<EntityName>());
			EditorGUI::EditableTextField(name);

			auto entity_record = ENTITY_INDEX[entity];
			auto archetype = entity_record.archetype;
			auto component_list = archetype->type;

			for (auto component_id : component_list)
			{
				auto type_descriptor = TYPE_DESCRIPTOR_LOOKUP[component_id];
				std::string component_name = type_descriptor->ToString();
				//if (component_name == "Position" || component_name == "Rotation" || component_name == "Scale" || component_name == "IsActive")
				//	continue;

				if (ComponentViewRegistry::ViewerExists(component_name))
				{
					if (ImGui::CollapsingHeader(component_name.c_str()))
					{
						ComponentViewRegistry::ViewComponent(component_name, entity);
					}
					
					//Remove component functionality
					if (ImGui::BeginPopupContextItem(("ComponentPopup" + component_name).c_str())) // Create a unique ID for the popup
					{
						bool can_remove = ComponentViewRegistry::RemoverExists(component_name);	//Disable remove button if not allowed to
						if (!can_remove) ImGui::BeginDisabled();

						if (ImGui::MenuItem("Remove Component"))
						{
							Log::Debug("Removing the component: " + component_name);
							ComponentViewRegistry::RemoveComponent(component_name, entity);
						}
						if (!can_remove) ImGui::EndDisabled();

						ImGui::EndPopup();
					}
				}
			}


			//Menu to add component
			if (ImGui::Button("Add Component"))
			{
				ImGui::OpenPopup("AddComponentPopup");
				
				memset(m_search_query, 0, sizeof(m_search_query));
				m_focus_search_bar = true; // Indicate to focus search bar
			}

			// Create the popup for adding components
			if (ImGui::BeginPopup("AddComponentPopup"))
			{
				if (m_focus_search_bar)
				{
					ImGui::SetKeyboardFocusHere(); // Focus on the next widget
					m_focus_search_bar = false; // Reset focus flag
				}
				// Display the search bar inside the popup
				ImGui::InputTextWithHint("##ComponentSearch", "Search Component...", m_search_query, IM_ARRAYSIZE(m_search_query));

				std::string query = m_search_query;
				for (auto& c : query)
				{
					c = static_cast<char>(tolower(static_cast<int>(c)));
				}

				for (const std::string& component_name : ComponentViewRegistry::GetComponentList())
				{
					// Convert the component name to lowercase for comparison
					std::string lower_component_name = component_name;
					for (auto& c : lower_component_name)
					{
						c = static_cast<char>(tolower(static_cast<int>(c)));
					}

					if (query.empty() || lower_component_name.find(query) != std::string::npos)
					{
						if (ImGui::Selectable(component_name.c_str()))
						{
							ComponentViewRegistry::AddComponent(component_name, entity);

							//if (component_name == "Camera")
							//	Editor::GetInstance().GetCamManager().AddCameraEntity(entity.Get(), entity.GetComponent<Camera>()->camera);
							//else if (component_name == "Button")
							//{
							//	ComponentViewRegistry::AddComponent("Sprite", entity);
							//	ComponentViewRegistry::AddComponent("OnHover", entity);
							//	ComponentViewRegistry::AddComponent("OnClick", entity);
							//	ComponentViewRegistry::AddComponent("BoundingBox2D", entity);
							//	ComponentViewRegistry::AddComponent("Rigidbody", entity);
							//}
							ImGui::CloseCurrentPopup();
						}
					}
				}
				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}

/*
			float button_width = ImGui::CalcTextSize("...").x + ImGui::GetStyle().FramePadding.x * 2.0f;
			ImGui::SameLine(ImGui::GetContentRegionAvail().x - button_width);
			if (ImGui::Button("..."))
			{
				ImGui::OpenPopup("ComponentOptions_Position");  // Open the popup
			}
			if (ImGui::BeginPopup("ComponentOptions_Position"))
			{
				if (ImGui::MenuItem("Remove Component"))
				{
					// Logic to remove the component goes here
					entity.RemoveComponent<Position>();
				}
				ImGui::EndPopup();
			}
*/

/*
//Do mandatory components
//IsActive and EntityName
if (entity.HasComponent<IsActive>())
{
	auto& is_active = entity.GetComponent<IsActive>()->is_active;
	ImGui::Checkbox("##IsActive", &is_active);
	ImGui::SameLine();
}

std::string& name = entity.GetComponent<EntityName>();
EditorGUI::EditableTextField(name);

//Hardcode these 3 components first so they appear
//at the top for clarity (also non removable)
auto transform = entity.GetComponent<Transform>();
if (entity.HasComponent<Position>())
{
	auto& position = entity.GetComponent<Position>()->position;
	if (ImGui::CollapsingHeader("Position"))
	{
		EditorGUI::DragFloat2(position, "Position");
		transform->is_dirty = true;
	}
	//Fake popup for clarity
	if (ImGui::BeginPopupContextItem("ComponentPopup Position"))
	{
		ImGui::BeginDisabled();
		ImGui::MenuItem("Remove Component");
		ImGui::EndDisabled();
		ImGui::EndPopup();
	}
}
if (entity.HasComponent<Rotation>())
{
	auto& rotation = entity.GetComponent<Rotation>()->rotation;
	if (ImGui::CollapsingHeader("Rotation"))
	{
		EditorGUI::DragFloat3(rotation, "Rotation");
		transform->is_dirty = true;
	}
	if (ImGui::BeginPopupContextItem("ComponentPopup Rotation"))
	{
		ImGui::BeginDisabled();
		ImGui::MenuItem("Remove Component");
		ImGui::EndDisabled();
		ImGui::EndPopup();
	}
}
if (entity.HasComponent<Scale>())
{
	auto& scale = entity.GetComponent<Scale>()->scale;
	if (ImGui::CollapsingHeader("Scale"))
	{
		EditorGUI::DragFloat2(scale, "Scale");
		transform->is_dirty = true;
	}
	if (ImGui::BeginPopupContextItem("ComponentPopup Scale"))
	{
		ImGui::BeginDisabled();
		ImGui::MenuItem("Remove Component");
		ImGui::EndDisabled();
		ImGui::EndPopup();
	}
}

// For cam only
if (entity.HasComponent<Camera>())
{
	transform->is_dirty = true;
	auto& cam = entity.GetComponent<Camera>()->camera;
	if (entity.HasComponent<IsActive>())
	{
		auto& is_active = entity.GetComponent<IsActive>()->is_active;
		if (!is_active && cam.cam_is_active)
		{
			// Disable the camera instead of removing it
			Editor::GetInstance().GetCamManager().DisableCameraEntity(entity.Get());
			cam.cam_is_active = is_active;
		}
		else if (is_active && !cam.cam_is_active)
		{
			// Enable the camera and validate it as the main camera if necessary
			Editor::GetInstance().GetCamManager().EnableCameraEntity(entity.Get());
			cam.cam_is_active = is_active;
		}
	}
}
*/

