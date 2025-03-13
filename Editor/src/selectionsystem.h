/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// selectionsystem.h
//
// Centralized location where selecting/deleting entities is managed in
// the level editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

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

		/*!***************************************************************************
		* @brief
		* Returns the selected entities as an unordered set.
		******************************************************************************/
		const std::unordered_set<FlexEngine::FlexECS::EntityID>& GetSelectedEntities();

		/*!***************************************************************************
		* @brief Select/Deselect
		* Functions to select and deselect entities to be focused on.
		* 
		* > SelectEntity() clears the selection, then selects the entity passed in
		* > AddSelectedEntity() adds the entity passed in to list of selected entits (multiselect supported)
		* > DeselectEntity() deselects a specific entity, if selected
		* > ClearSelection() just clears
		******************************************************************************/
		void SelectEntity(FlexEngine::FlexECS::EntityID);	//Singular
		void AddSelectedEntity(FlexEngine::FlexECS::EntityID); //For Multiselecting
		void DeselectEntity(FlexEngine::FlexECS::EntityID);
		void ClearSelection();

		/*!***************************************************************************
		* @brief Deleting Entities
		* > DeleteEntity() deletes specific entity
		* > DeleteSelectedEntities() deletes all selected entities
		******************************************************************************/
		void DeleteEntity(FlexEngine::FlexECS::EntityID);
		void DeleteSelectedEntities();

	private:
		//Internal functions that ensure all children of a parent entity are also deleted.
		void HandleEntityDelete();
		void FindChildrenToDelete(FlexEngine::FlexECS::EntityID);

		FlexEngine::FlexECS::Entity m_selected_entity = FlexEngine::FlexECS::Entity::Null;	//Which entity is focused on.
		
		std::unordered_set<FlexEngine::FlexECS::EntityID> m_selected_entities;
		std::unordered_set<FlexEngine::FlexECS::EntityID> m_entities_to_delete;

	};
}