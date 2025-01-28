#pragma once
#include "editorsystem.h"
#include "unordered_set"

namespace Editor
{
	class SelectionSystem : public EditorSystem
	{
	public:
		void Init() override;
		void Update() override;
		void Shutdown() override;

		const std::unordered_set<FlexEngine::FlexECS::EntityID>& GetSelectedEntities();

		void SelectEntity(FlexEngine::FlexECS::EntityID);	//Singular
		void AddSelectedEntity(FlexEngine::FlexECS::EntityID); //For Multiselecting
		void DeselectEntity(FlexEngine::FlexECS::EntityID);
		void ClearSelection();

		void DeleteEntity(FlexEngine::FlexECS::EntityID);
		void DeleteSelectedEntities();

	private:
		void HandleEntityDelete();
		void FindChildrenToDelete(FlexEngine::FlexECS::EntityID);

		FlexEngine::FlexECS::Entity m_selected_entity = FlexEngine::FlexECS::Entity::Null;	//Which entity is focused on.
		
		std::unordered_set<FlexEngine::FlexECS::EntityID> m_selected_entities;
		std::unordered_set<FlexEngine::FlexECS::EntityID> m_entities_to_delete;

	};
}