#pragma once

#include "FlexEngine.h"
#include "editorpanel.h"

namespace ChronoDrift
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


		std::filesystem::path m_current_directory = "assets";
		const std::filesystem::path m_root_directory = "assets";

		std::filesystem::path m_selected_file = "";

		//bool m_is_renaming = false;	//idw support renaming files inside editor

		std::unordered_map<std::filesystem::path, Folder> m_directories;
		Folder m_root_folder;	//Need to know what's inside the root folder is so we know where to start displaying

		//Additional tracking of certain file types
		//For things such as font dropdown menus.
		std::vector<std::string> m_font_paths;
	};


}