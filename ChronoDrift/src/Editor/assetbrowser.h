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


	private:
		std::filesystem::path m_current_directory = "assets";
		const std::filesystem::path m_root_directory = "assets";

		std::filesystem::path m_selected_file = "";

		//bool m_is_renaming = false;

		Folder m_root_folder;
		std::unordered_map<std::filesystem::path, Folder> m_directories;

		void LoadAllDirectories();
		void AddToDirectoryStructure(const std::filesystem::path& entry);
		void RenderFolder(Folder& folder);

	};


}