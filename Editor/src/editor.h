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
// Copyright (c) 2025 DigiPen, All rights reserved.
**************************************************************************/

#pragma once
#include "FlexEngine.h"

#include <FlexEngine/Renderer/Camera/camera.h>

#include "editorpanel.h"
#include "editorsystem.h"
#include "assetbrowser.h"
#include "hierarchyview.h"
#include "inspector.h"
#include "prefabeditor.h"
#include "sceneview.h"
#include "gameview.h"
#include "settingspanel.h"
#include "FlexEngine/Renderer/Camera/camera.h" // For an editor owned camera

#include "selectionsystem.h"

#include <memory>
#include <typeindex>
#include <typeinfo>

using namespace FlexEngine;

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


		/*!***************************************************************************
		* @brief
		* Access specific panels if u need to
		******************************************************************************/
		template <typename T> T* GetPanel();
		template <typename T> T* GetSystem();

    static Camera m_editorCamera; // Publicly assessed editorcamera for now
			
	private:
		bool m_initialized = false;
		
		AssetBrowser m_assetbrowser;
		HierarchyView m_hierarchy;
		Inspector m_inspector;
		PrefabEditor m_prefabeditor;
		SceneView m_sceneview;
		GameView m_gameview;
		SettingsPanel m_settingspanel;

		SelectionSystem m_selection;

		std::unordered_map<std::type_index, EditorPanel*> m_panels;
		std::unordered_map<std::type_index, EditorSystem*> m_systems;

	};


	template <typename T>
	T* Editor::GetPanel()
	{
		auto type = std::type_index(typeid(T));
		auto it = m_panels.find(type);
		if (it == m_panels.end())
		{
			FlexEngine::Log::Fatal("Panel not found!");
		}

		return static_cast<T*>(it->second);
	}

	template <typename T>
	T* Editor::GetSystem()
	{
		auto type = std::type_index(typeid(T));
		auto it = m_systems.find(type);
		if (it == m_systems.end())
		{
			FlexEngine::Log::Fatal("Editor System not found!");
		}

		return static_cast<T*>(it->second);
	}
}
