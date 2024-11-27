/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// editor.h
//
// This file contains the declaration of the Editor class.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
//panels
#include "Editor/editorpanel.h"
#include "Editor/hierarchyview.h"
#include "Editor/inspector.h"
#include "Editor/assetbrowser.h"
#include "Editor/sceneview.h"
#include "Editor/gameview.h"
#include "Editor/componentviewer.h"
#include "Editor/editorgui.h"
#include "Editor/imguipayloads.h"
//systems
#include "Editor/editorsystem.h"
#include "Editor/editorcommands.h"

#include "Components/rendering.h"

#include "FlexEngine/Core/imguiwrapper.h"
#include "FlexEngine/Renderer/DebugRenderer/debugrenderer.h"
#include "FlexEngine.h"

#include <memory>

namespace ChronoDrift
{
	class Editor
	{
	public:
		/*!***************************************************************************
		* @brief
		* Getter for our singleton class.
		******************************************************************************/
		static Editor& GetInstance();

		/*!***************************************************************************
		* @brief
		* init
		******************************************************************************/
		void Init();

		/*!***************************************************************************
		* @brief
		* Update function. Just call once every frame!
		******************************************************************************/
		void Update();

		/*!***************************************************************************
		* @brief
		* shutdun
		******************************************************************************/
		void Shutdown();

		//consider shared_ptr instead?
		/*template <typename T>
		T& GetPanel();*/

		EditorPanel& GetPanel(const std::string& panel_name);

		EditorSystem* GetSystem(const std::string& system_name);


		/*!***************************************************************************
		* @brief
		* Sets the selected entity to param entity.
		* @param Entity
		* the entity to focus onto.
		******************************************************************************/
		void SelectEntity(FlexEngine::FlexECS::Entity);
		FlexEngine::FlexECS::Entity GetSelectedEntity();

		void DeleteSelectedEntity();

		/*!***************************************************************************
		* @brief Camera Get/Set functions.
		******************************************************************************/
		FlexEngine::CameraManager& GetCamManager() const;
		void SetCamManager(FlexEngine::CameraManager& camManager);

	private:
		bool m_initialized = false;

		HierarchyView m_hierarchy;
		Inspector m_inspector;
		AssetBrowser m_assetbrowser;
		SceneView m_sceneview;
		GameView m_gameview;

		EditorCommands m_editorcommands;

		//In a future update, id like to have systems alongside the panels
		//that handle these kinds of behaviour like deletion and selected entity tracking 
		std::unordered_set<FlexEngine::FlexECS::EntityID> m_entities_to_delete;
		FlexEngine::FlexECS::Entity m_selected_entity = FlexEngine::FlexECS::Entity::Null;	//Which entity the inspector panel should focus on.

		std::unordered_map<std::string, EditorPanel*> m_panels;
		std::unordered_map<std::string, EditorSystem*> m_systems;

		FlexEngine::CameraManager* m_CamM_Instance = nullptr;
	};



}
