/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// hierarchyview.cpp
//
// Hierarchy Panel for the editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#include "editor.h"
#include "hierarchyview.h"
#include "editorgui.h"

using namespace FlexEngine;

using EntityName = FlexECS::Scene::StringIndex;

namespace Editor
{
	void HierarchyView::Init()
	{
	}
	void HierarchyView::Update()
	{
	}

	void HierarchyView::EditorUI()
	{
		auto scene = FlexECS::Scene::GetActiveScene();

		ImGui::Begin("Scene Hierarchy");

		auto selection_system = Editor::GetInstance().GetSystem<SelectionSystem>();

		//Drag a sprite from assets to window to create entity with the sprite.
		if (auto image = EditorGUI::StartWindowPayloadReceiver<const char>(PayloadTags::IMAGE))
		{
			//calculate position to place, at center
			Vector3 position = Editor::GetInstance().m_editorCamera.GetPosition();
			//float width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth());
			//float height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());
			//position.x += width / 2;
			//position.y += height / 2;

			std::string image_key(image);
			std::filesystem::path path = image_key;

			FlexECS::Entity new_entity = scene->CreateEntity(path.filename().string());
			new_entity.AddComponent<Position>({ {position.x, position.y} });
			new_entity.AddComponent<Rotation>({});
			new_entity.AddComponent<Scale>({ Vector2::One * 100.0f });
			new_entity.AddComponent<Transform>({});
			new_entity.AddComponent<Sprite>({ FLX_STRING_NEW(image_key) });
			EditorGUI::EndPayloadReceiver();
		}

		//Right click menu (create entity)
		if (ImGui::IsWindowHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right))
		{
			ImGui::OpenPopup("RightClickMenu");
		}
		if (ImGui::BeginPopup("RightClickMenu"))
		{
			if (ImGui::MenuItem("Create Entity"))
			{
				//Add default components
				FlexECS::Entity new_entity = FlexECS::Scene::CreateEntity();
				new_entity.AddComponent<Position>({});
				new_entity.AddComponent<Rotation>({});
				new_entity.AddComponent<Scale>({});
				new_entity.AddComponent<Transform>({});
			}
			ImGui::EndPopup();
		}

		//Display all entities
		auto selected_entities = Editor::GetInstance().GetSystem<SelectionSystem>()->GetSelectedEntities();
		bool multiselect = selected_entities.size() > 1;
		bool item_hovered = false;
		for (auto& [id, record] : scene->entity_index)
		{
			EditorGUI::PushID();
			FlexECS::Entity entity(id);

      std::string name = FLX_STRING_GET(*entity.GetComponent<EntityName>());
			ImGuiTreeNodeFlags node_flags = entity.HasComponent<Parent>() ? 
				ImGuiTreeNodeFlags_DefaultOpen |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_OpenOnArrow |
				ImGuiTreeNodeFlags_SpanAvailWidth
				:
				ImGuiTreeNodeFlags_Leaf |
				ImGuiTreeNodeFlags_FramePadding |
				ImGuiTreeNodeFlags_SpanAvailWidth;

			bool is_selected = false;
			if (selected_entities.find(entity) != selected_entities.end()) is_selected = true;

			if (is_selected)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			if (ImGui::TreeNodeEx(name.c_str(), node_flags, name.c_str()))
			{
				if (!multiselect && ImGui::BeginDragDropSource())
				{
					EditorGUI::StartPayload(PayloadTags::ENTITY, &entity.Get(), sizeof(FlexECS::EntityID), name.c_str());
					EditorGUI::EndPayload();
				}
				ImGui::TreePop();
			}

			if (ImGui::IsItemHovered())
			{
				item_hovered = true;
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Left) && !ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			{
				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
				{
					selection_system->AddSelectedEntity(entity);
				}
				else 
				{
					selection_system->SelectEntity(entity);
				}
			}


			if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			{
				ImGui::OpenPopup("EntityOptions");
				selection_system->SelectEntity(entity);
			}
			if (ImGui::BeginPopup("EntityOptions"))
			{
				if (ImGui::MenuItem("Duplicate Entity"))
				{
					scene->CloneEntity(entity);
				}
				if (ImGui::MenuItem("Destroy Entity"))
				{
					selection_system->DeleteEntity(entity);
				}
				ImGui::EndPopup();
			}
			EditorGUI::PopID();
		}

		//Delete entity with del key
		if (ImGui::IsKeyPressed(ImGuiKey_Delete) && ImGui::IsWindowFocused())
		{
			selection_system->DeleteSelectedEntities();
		}

		//Deselect focused entity when clicking on empty space
		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !item_hovered)
		{
			selection_system->ClearSelection();
		}
		ImGui::End();
	}

	void HierarchyView::Shutdown()
	{
	}
}

