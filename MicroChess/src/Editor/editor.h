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

#include "hierarchyview.h"
#include "inspector.h"
#include "editorgui.h"
#include "componentviewer.h"
#include "Components/rendering.h"

#include "FlexEngine/Core/imguiwrapper.h"
#include "FlexEngine/Renderer/DebugRenderer/debugrenderer.h"
#include "FlexEngine.h"

namespace ChronoShift
{
	class Editor
	{
	public:

		/*!***************************************************************************
		* @brief
		* Getter for our singleton class.
		******************************************************************************/
		static Editor* GetInstance();

		/*!***************************************************************************
		* @brief
		* Currently nothing inside.
		******************************************************************************/
		void Init();

		/*!***************************************************************************
		* @brief
		* Update function. Just call once every frame!
		******************************************************************************/
		void Update();

		/*!***************************************************************************
		* @brief
		* Currently nothing inside.
		******************************************************************************/
		void Shutdown();

		/*!***************************************************************************
		* @brief
		* Sets the selected entity to param entity.
		* @param Entity
		* the entity to focus onto.
		******************************************************************************/
		void SelectEntity(FlexEngine::FlexECS::Entity);
		FlexEngine::FlexECS::Entity GetSelectedEntity();

	private:
		FlexEngine::FlexECS::Entity m_selected_entity = FlexEngine::FlexECS::Entity::Null;	//Which entity the inspector panel should focus on.
		//ComponentViewRegistry m_component_registry;
	};

}
