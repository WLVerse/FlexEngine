/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// assetbrowser.h
//
// Asset browser for the Level Editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#pragma once

#include "FlexEngine.h"
#include "FlexEngine/assetdropmanager.h"
#include "editorpanel.h"
#include <unordered_map>
#include <string>

namespace Editor
{

	struct Folder
	{
		std::filesystem::path path;
		std::string name;
		std::vector<std::filesystem::path> files;
		std::unordered_map<std::filesystem::path, Folder*> subfolders;

		Folder() = default;

		Folder(const std::filesystem::path& folder_path)
			: path(folder_path), name(folder_path.string())
		{}
	};

	class AssetBrowser : public EditorPanel
	{
	public:
		void Init();
		void Update();
		void EditorUI();
		void Shutdown();

		const std::unordered_map<std::string, std::string>& GetLoadedFontsList() const;

	private:

		/*!***************************************************************************
		* @brief
		* This function renders each folder, and its contents
		* into the asset browser window
		******************************************************************************/
		void RenderFolder(Folder& folder);

		/*!***************************************************************************
		* @brief
		* Loads all files inside the assets folder, and populates m_directories with the filepath information.
		******************************************************************************/
		void LoadAllDirectories();

		/*!***************************************************************************
		* @brief
		* Loads all files inside the assets folder, and populates m_directories with the filepath information.
		******************************************************************************/
		void AddToDirectoryStructure(const std::filesystem::path& entry);

		/*!***************************************************************************
		* @brief
		* Used for the add file feature.
		* Renders the dropdown of where you use the folder.
		* Recursive function, so start it by RenderFolderDropdown(root_folder);
		******************************************************************************/
		void RenderFolderDropdown(Folder& folder);


		/*!***************************************************************************
		* @brief DeleteFilePopup, RenderDeleteConfirmationPopup
		* These two functions combined handle the displaying of 
		* delete confirmation popup
		******************************************************************************/
		void DeleteFilePopup(std::filesystem::path& file);
		void RenderDeleteConfirmationPopup();


		/*
		* Editor display 2
		*/
		void EditorUI2();
		void RenderFolderDropdown2(Folder& folder);
		void RenderThumbnails(Folder& folder);



		//Drag file from windows to editor
		void OnFileDropped(const std::vector<std::string>& file_list);

		char m_text_input[128] = "";	//create file name, search...
		std::filesystem::path m_selected_file = "";
		Folder* m_selected_folder;

		bool m_should_delete_file;
		std::filesystem::path m_file_to_delete;
		bool m_show_delete_confirmation = false; // Flag to show the confirmation popup

		std::unordered_map<std::filesystem::path, Folder> m_directories;
		Folder m_root_folder;	//Need to know what's inside the root folder is so we know where to start displaying

		float m_left_panel_width = 320.0f;
		
		struct PanelRect
		{
			ImVec2 min;
			ImVec2 max;
		};
		PanelRect m_right_panel_rect;
		
		//Additional tracking of certain file types
		//For things such as font dropdown menus.
		//first = font name, second = filepath
		std::unordered_map<std::string, std::string> m_font_paths;


		const std::filesystem::path m_root_directory = "../../assets";

		#pragma region -.-
		const std::filesystem::path m_resources_root = "../../Editor/resources";

		const std::filesystem::path m_audio_image = "../../Editor/resources/assetbrowser_audio.png";
		const std::filesystem::path m_audio_image_key = "/assetbrowser_audio.png";

		const std::filesystem::path m_folder_image = "../../Editor/resources/assetbrowser_folder.png";
		const std::filesystem::path m_folder_image_key = "/assetbrowser_folder.png";

		const std::filesystem::path m_flxprefab_image = "../../Editor/resources/assetbrowser_flxprefab.png";
		const std::filesystem::path m_flxprefab_image_key = "/assetbrowser_flxprefab.png";

		const std::filesystem::path m_shader_image = "../../Editor/resources/assetbrowser_shader.png";
		const std::filesystem::path m_shader_image_key = "/assetbrowser_shader.png";

		const std::filesystem::path m_font_image = "../../Editor/resources/assetbrowser_font.png";
		const std::filesystem::path m_font_image_key = "/assetbrowser_font.png";
		#pragma endregion
	};


}

//const std::unordered_map<std::string, std::set<std::string>> safe =
//{
//	// FlexEngine files
//	{ "flx", { ".flxscene", ".flxscript", ".flxprefab", ".flxmaterial", ".flxdata"}},
//	// FlexEngine binary files
//	{ "flb", { ".flbscene", ".flbscript", ".flbprefab", ".flbmaterial" } },
//	// Data files (txt, json, etc.)
//	{ "data", { ".txt", ".json", ".csv", ".yaml", ".ini", ".cfg", ".log", ".dat" } },
//	// Shader files (glsl, frag, etc.)
//	{ "shader", { ".glsl", ".hlsl", ".frag", ".vert" }},
//	// Image files (jpg, png, etc.)
//	{ "image", { ".jpg", ".jpeg", ".png" } },
//	// Font files (ttf)
//	{ "font", { ".ttf"} },
//	// Video files (mp4, avi, etc.)
//	{ "video", { ".mp4" } },
//	// Audio files (mp3, wav, etc.)
//	{ "audio", { ".mp3", ".wav", ".ogg", ".flac" } },
//	// Model files (obj, fbx, etc.)
//	{ "model", { ".obj", ".mtl", ".fbx", ".gltf", ".glb" } },
//};