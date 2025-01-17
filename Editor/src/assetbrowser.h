#pragma once

#include "FlexEngine.h"
#include "editorpanel.h"

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

		const std::vector<std::string>& GetLoadedFontsList() const { return m_font_paths; }

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

		void RenderFolderDropdown(Folder& folder);

		void DeleteFilePopup(std::filesystem::path& file);

		void RenderDeleteConfirmationPopup();

		const std::filesystem::path m_root_directory = "../../assets";

		char m_text_input[128] = "";	//create file name, search...
		std::filesystem::path m_selected_file = "";
		Folder* m_selected_folder;

		bool m_should_delete_file;
		std::filesystem::path m_file_to_delete;
		bool m_show_delete_confirmation = false; // Flag to show the confirmation popup

		std::unordered_map<std::filesystem::path, Folder> m_directories;
		Folder m_root_folder;	//Need to know what's inside the root folder is so we know where to start displaying

		//Additional tracking of certain file types
		//For things such as font dropdown menus.
		std::vector<std::string> m_font_paths;

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