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


#include "FlexEngine.h"
using namespace FlexEngine;

#include "editorpanel.h"
#include "assetbrowser.h"
#include <memory>

namespace Editor
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

		EditorPanel* GetPanel(const std::string& panel_name);



		void SelectEntity(FlexEngine::FlexECS::Entity);
		FlexEngine::FlexECS::Entity GetSelectedEntity();


	private:

		
		AssetBrowser m_assetbrowser;
		//HierarchyView m_hierarchy;
		//Inspector m_inspector;
		//SceneView m_sceneview;
		//GameView m_gameview;

		bool m_initialized = false;

		//Selection system to be done
		FlexEngine::FlexECS::Entity m_selected_entity = FlexEngine::FlexECS::Entity::Null;	//Which entity the inspector panel should focus on.

		std::unordered_map<std::string, EditorPanel*> m_panels;

	};
}
