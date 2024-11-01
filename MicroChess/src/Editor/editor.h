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

#include "Editor/hierarchyview.h"
#include "Editor/inspector.h"
#include "Editor/assetbrowser.h"
#include "Editor/sceneview.h"
#include "Editor/gameview.h"
#include "Editor/componentviewer.h"
#include "Editor/editorgui.h"
#include "Editor/imguipayloads.h"

#include "Components/rendering.h"

#include "FlexEngine/Core/imguiwrapper.h"
#include "FlexEngine/Renderer/DebugRenderer/debugrenderer.h"
#include "FlexEngine.h"
#include "ImGuizmo/ImGuizmo.h"

#include <memory>

namespace ChronoShift
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
		template <typename T>
		T& GetPanel();

		/*!***************************************************************************
		* @brief
		* Sets the selected entity to param entity.
		* @param Entity
		* the entity to focus onto.
		******************************************************************************/
		void SelectEntity(FlexEngine::FlexECS::Entity);
		FlexEngine::FlexECS::Entity GetSelectedEntity();



	private:
		bool m_initialized = false;
		std::vector<EditorPanel*> m_panels;

		HierarchyView m_hierarchy;
		Inspector m_inspector;
		AssetBrowser m_assetbrowser;
		SceneView m_sceneview;
		GameView m_gameview;

		//wtf move this out to inspector or something else
		FlexEngine::FlexECS::Entity m_selected_entity = FlexEngine::FlexECS::Entity::Null;	//Which entity the inspector panel should focus on.
		
		//ComponentViewRegistry m_component_registry;
		//std::unordered_map<std::type_index, std::string> type_names;
		//std::unordered_map<std::type_index, EditorPanel*> m_panels;
	};



}
