/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// assetbrowser.cpp
//
// Asset browser for the Level Editor.
//
// AUTHORS
// [100%] Rocky Sutarius (rocky.sutarius@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "editor.h"
#include "assetbrowser.h"
#include "editorgui.h"
#include <filesystem>
#include <functional>
#include <filesystem>


namespace Editor
{
	void AssetBrowser::Init()
	{
		LoadAllDirectories();
		
		m_root_folder.name = "Assets";
		//m_root_folder.path = m_root_directory;
		
		Application::GetCurrentWindow()->m_dropmanager.RegisterFileDropCallback(std::bind(&AssetBrowser::OnFileDropped, this, std::placeholders::_1));

		AssetManager::LoadFileFromPath(m_audio_image, m_resources_root);
		AssetManager::LoadFileFromPath(m_folder_image, m_resources_root);
		AssetManager::LoadFileFromPath(m_flxprefab_image, m_resources_root);
		AssetManager::LoadFileFromPath(m_shader_image, m_resources_root);
		AssetManager::LoadFileFromPath(m_font_image, m_resources_root);
	}

	void AssetBrowser::Update()
	{
		RenderDeleteConfirmationPopup();

		if (m_should_delete_file)
		{
			if (std::filesystem::exists(m_file_to_delete))
			{
				std::filesystem::remove(m_file_to_delete);
				m_should_delete_file = false;
				m_file_to_delete.clear();


				//save the path, so we can navigate back to the folder after refreshing directory structure
				std::filesystem::path old_selected_folder_path = m_selected_folder->path;
				LoadAllDirectories();
				m_selected_folder = &m_directories[old_selected_folder_path];
			}
		}
	}
	void AssetBrowser::Shutdown()
	{
		m_root_folder.subfolders.clear();
		m_directories.clear();
	}

	const std::unordered_map<std::string, std::string>& AssetBrowser::GetLoadedFontsList() const
	{
		return m_font_paths;
	}


	void AssetBrowser::LoadAllDirectories()
	{
		m_root_folder.files.clear(); 
		m_root_folder.subfolders.clear();
		m_font_paths.clear();
		m_directories.clear();
		m_selected_folder = nullptr;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(m_root_directory))
		{
			AddToDirectoryStructure(entry);
		}

		//Populate root folder with directories
		for (auto& [folder_path, folder] : m_directories)
		{
			if (folder_path.parent_path() == "")
			{
				m_root_folder.subfolders[folder_path] = &folder;
			}
		}
	}

	void AssetBrowser::AddToDirectoryStructure(const std::filesystem::path& entry)
	{
		auto relative_path = std::filesystem::relative(entry, m_root_directory);
		relative_path = relative_path;
		auto parent_path = relative_path.parent_path();

		if (relative_path.extension().string() == ".ttf")
		{
			m_font_paths.insert({ relative_path.filename().string(), "/" + relative_path.generic_string()});
		}

		Folder* current_folder = nullptr;
		//check which folder this entry is within
		for (auto& [folder_name, folder] : m_directories)
		{
			if (parent_path == folder.path)
			{
				current_folder = &folder;
				break;
			}
		}

		if (std::filesystem::is_directory(entry))
		{
			//Add to directory list if havent yet
			if (m_directories.find(relative_path) == m_directories.end())
			{
				m_directories[relative_path] = Folder(relative_path);
				m_directories[relative_path].name = relative_path.filename().string();

			}

			//Add to the parent.subfolder if its a is directory
			if (m_directories.find(parent_path) != m_directories.end())
			{
				m_directories[parent_path].subfolders[relative_path] = &m_directories[relative_path];
			}
		}
		else
		{
			//Check if top level file
			if (parent_path.string() == "")
			{
				m_root_folder.files.push_back(relative_path);
			}
			else
			{
				// Add file to folder normally
				m_directories[parent_path].files.push_back(relative_path);
			}
		}
	}

	void AssetBrowser::RenderFolderDropdown(Folder& folder)
	{
		//ImGuiTreeNodeFlags node_flags = (folder.subfolders.size() < 1) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		
		ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		if (folder.subfolders.empty())
			node_flags |= ImGuiTreeNodeFlags_Leaf;
		else
		{
			node_flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		}


		// Highlight the selected folder
		if (m_selected_folder == &folder)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		bool node_open = ImGui::TreeNodeEx((folder.name + "##" + folder.path.string()).c_str(), node_flags);
		
		// Check for clicks on the tree node itself because imgui default behaviour bad
		if (ImGui::IsItemClicked())
		{
			// Get the rectangle of the tree node
			ImVec2 node_min = ImGui::GetItemRectMin();
			// Approximate arrow region width by the frame height
			float arrow_width = ImGui::GetFrameHeight();

			// If the mouse is clicked beyond the arrow region...
			if (ImGui::GetIO().MousePos.x > node_min.x + arrow_width)
			{
				m_selected_folder = &folder; // Register as a body click
			}
			// Else, the click was likely(ESTIMATED) on the arrow;
		}

		// Render folder node
		if (node_open)
		{
			// Render subfolders recursively
			for (auto& [subfolder_path, subfolder] : folder.subfolders)
			{
				RenderFolderDropdown(*subfolder);
			}
			ImGui::TreePop();
		}
	}

	void AssetBrowser::DeleteFilePopup(std::filesystem::path& file)
	{
		//Deleting files
		if (ImGui::BeginPopupContextItem(file.filename().string().c_str()))
		{
			if (ImGui::MenuItem("Delete"))
			{
				if (std::filesystem::exists(m_root_directory / file))
				{
					m_file_to_delete = m_root_directory / file;
					m_show_delete_confirmation = true;
				}
			}
			ImGui::EndPopup();
		}
	}

	void AssetBrowser::RenderDeleteConfirmationPopup()
	{
		if (m_show_delete_confirmation)
		{
			ImGui::OpenPopup("Confirm Delete");
		}

		if (ImGui::BeginPopupModal("Confirm Delete", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Are you sure you want to delete this file?\nThis action cannot be undone.");
			ImGui::Separator();

			ImGui::Text("File: %s", m_file_to_delete.filename().string().c_str());

			if (ImGui::Button("Yes"))
			{
				if (std::filesystem::exists(m_file_to_delete))
				{
					m_should_delete_file = true;
				}
				m_show_delete_confirmation = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("No"))
			{
				m_show_delete_confirmation = false; // Cancel deletion
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void AssetBrowser::RenderFolder(Folder& folder)
	{
		if (ImGui::TreeNodeEx(folder.name.c_str()))
		{
			// Render directories first
			for (auto& [subfolder_path, subfolder] : folder.subfolders)
			{
				RenderFolder(*subfolder);
			}

			// Render the rest of the files in the folder
			for (auto& file : folder.files)
			{
				//Render the file name
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
				bool is_selected = (m_selected_file == file);
				if (is_selected) node_flags |= ImGuiTreeNodeFlags_Selected;

				ImGui::TreeNodeEx(file.filename().string().c_str(), node_flags);
				if (ImGui::IsItemClicked())
				{
					m_selected_file = file;
				}

				//Drag and drop assets
				if (ImGui::BeginDragDropSource())
				{
					std::string payload(file.generic_string());
					payload.insert(0, "/");	//to fit the AssetKey format
					std::string extension = file.extension().string();
					//hardcode for now
					if (extension == ".jpg" || extension == ".jpeg" || extension == ".png")
					{
						EditorGUI::StartPayload(PayloadTags::IMAGE, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".flxspritesheet")
					{
						EditorGUI::StartPayload(PayloadTags::SPRITESHEET, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".glsl" || extension == ".hlsl" || extension == ".frag" || extension == ".vert")
					{
						payload = payload.substr(0, payload.find_last_of('.')); //to fit the AssetKey::Shader format
						EditorGUI::StartPayload(PayloadTags::SHADER, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg" || extension == ".flac")
					{
						EditorGUI::StartPayload(PayloadTags::AUDIO, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".flxprefab")
					{
						EditorGUI::StartPayload(PayloadTags::PREFAB, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else if (extension == ".ttf")
					{
						EditorGUI::StartPayload(PayloadTags::FONT, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
						EditorGUI::EndPayload();
					}
					else
					{
						//Find rocky if you want your filetype supported
						ImGui::Text("Unsupported file type: %s", extension.c_str());
						EditorGUI::EndPayload();
					}
				}

				DeleteFilePopup(file);
			}
			ImGui::TreePop();
		}
	}

	void AssetBrowser::EditorUI()
	{
		EditorUI2();

		//My intention is to create settings/options that allow you to swap "styles/layout"
		#if 0
		ImGui::Begin("Asset Browser");
		if (ImGui::Button("Refresh"))
		{
			AssetManager::Unload();
			AssetManager::Load();
			LoadAllDirectories();
		}

		ImGui::SameLine();

		if (ImGui::Button("Add File"))
		{
			ImGui::OpenPopup("AddFilePopup");
			memset(m_text_input, 0, sizeof(m_text_input));
			m_selected_folder = &m_root_folder;
		}

		if (ImGui::BeginPopup("AddFilePopup"))
		{
			ImGui::Text(("Selecting Folder: " + m_selected_folder->name).c_str());

			for (auto& [subfolder_path, subfolder] : m_root_folder.subfolders)
			{
				ImVec2 dropdown_size(470.0f, 250.0f);
				if (ImGui::BeginChild("FolderDropdown", dropdown_size, true, ImGuiWindowFlags_HorizontalScrollbar))
				{
					RenderFolderDropdown(*subfolder);
					ImGui::EndChild();
				}
			}

			ImGui::Separator();
			ImGui::InputText("File Name", m_text_input, IM_ARRAYSIZE(m_text_input));

			if (ImGui::Button("Create"))
			{
				if (std::strlen(m_text_input) > 0)
				{
					if (m_selected_folder == nullptr) m_selected_folder = &m_root_folder;
					std::filesystem::path new_file_path = m_root_directory / m_selected_folder->path /m_text_input;
					std::ofstream new_file(new_file_path);
					new_file.close();

					//AssetManager::Unload();
					//AssetManager::Load();
					LoadAllDirectories();
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		for (auto& [path, folder] : m_root_folder.subfolders)
		{
			RenderFolder(*folder);
		}

		for (auto& file : m_root_folder.files)
		{
			ImGui::Selectable(file.filename().string().c_str());
			DeleteFilePopup(file);
		}
		ImGui::End();

		#endif
	}


	/*************************
	***Below lies version 2***
	*************************/
	void AssetBrowser::EditorUI2()
	{
		ImGui::Begin("Asset Browser");

		// Left Panel: Fixed width child window for the folder dropdown.
		ImGui::BeginChild("LeftPanel", ImVec2(m_left_panel_width, 0), true);
		

		RenderFolderDropdown(m_root_folder);

		ImGui::EndChild();


		ImGui::SameLine();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine();


		// Right Panel: Child window that takes up the remaining space (width=0 means auto-fill)
		ImGui::BeginChild("RightPanel", ImVec2(0, 0), true);
		m_right_panel_rect.min = ImGui::GetWindowPos();
		m_right_panel_rect.max = { m_right_panel_rect.min.x + ImGui::GetWindowSize().x,
															m_right_panel_rect.min.y + ImGui::GetWindowSize().y };


		std::string current_path_to_folder;
		if (m_selected_folder)
		{
			current_path_to_folder = m_selected_folder->path.string();
		}
		else
		{
			current_path_to_folder = "No folder selected.";
		}

		ImGui::Text(("Current Folder:   " + current_path_to_folder).c_str());

		//File Creation
		static bool s_openAddFilePopup = false;
		if (ImGui::BeginPopupContextWindow("AssetBrowserContextMenu", ImGuiMouseButton_Right | ImGuiPopupFlags_NoOpenOverItems))
		{
			if (ImGui::MenuItem("Create File"))
			{
				s_openAddFilePopup = true;
				memset(m_text_input, 0, sizeof(m_text_input));
			}
			ImGui::EndPopup();
		}
		if (s_openAddFilePopup)
		{
			ImGui::OpenPopup("AddFilePopup");
			s_openAddFilePopup = false;
		}
		if (ImGui::BeginPopup("AddFilePopup"))
		{
			if (m_selected_folder)
			{
				ImGui::Text("Create new file in folder: %s", m_selected_folder->name.c_str());
			}
			else
			{
				ImGui::Text("No folder selected. Using root folder.");
				m_selected_folder = &m_root_folder;
			}

			ImGui::Separator();
			ImGui::InputText("File Name", m_text_input, IM_ARRAYSIZE(m_text_input));

			if (ImGui::Button("Create"))
			{
				if (std::strlen(m_text_input) > 0)
				{
					std::filesystem::path new_file_path = m_root_directory / m_selected_folder->path / m_text_input;
					std::ofstream new_file(new_file_path);
					new_file.close();

					//save the path, so we can navigate back to the folder after refreshing directory structure
					std::filesystem::path old_selected_folder_path = m_selected_folder->path;
					LoadAllDirectories();
					m_selected_folder = &m_directories[old_selected_folder_path];
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}


		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

		if (m_selected_folder)
		{
			RenderThumbnails(*m_selected_folder);
		}

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered())
		{
			m_selected_file.clear();
		}
		

		ImGui::EndChild();
		
		
		ImGui::End();
	}

	std::string TruncateTextToWidth(const std::string& text, float maxWidth) {
		// First, check if the text already fits.
		if (ImGui::CalcTextSize(text.c_str()).x <= maxWidth)
			return text;

		std::string truncated = text;
		const std::string ellipsis = "...";
		// Remove characters until the text with ellipsis fits.
		while (!truncated.empty() && ImGui::CalcTextSize((truncated + ellipsis).c_str()).x > maxWidth)
			truncated.pop_back();
		return truncated + ellipsis;
	}

	void AssetBrowser::RenderThumbnails(Folder& folder)
	{
		// Define thumbnail size and padding
		const float thumbnail_size = 64.0f;
		const float padding = 15.0f;
		float cell_size = thumbnail_size + padding;

		// Determine how many thumbnails can fit in one row
		float available_width = ImGui::GetContentRegionAvail().x - m_left_panel_width;
		int items_per_row = static_cast<int>(available_width / (cell_size + padding/2));
		if (items_per_row < 1)
			items_per_row = 1;

		int item_index = 0;
		
		//Folders before files
		for (auto& [directory_path, p_folder] : folder.subfolders)
		{
			ImGui::PushID(directory_path.string().c_str());

			// Group the thumbnail and its label together
			ImGui::BeginGroup();

			Asset::Texture& texture = AssetManager::Get<Asset::Texture>(m_folder_image_key.string());
			
			bool isSelected = (m_selected_file == directory_path);
			if (isSelected)
			{
				EditorGUI::StyleColorPush_Selected();
			}

			if (ImGui::ImageButton(texture.GetTextureImGui(), ImVec2(thumbnail_size, thumbnail_size)))
			{
				m_selected_file = directory_path;
			}


			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			{
				m_selected_folder = p_folder; // set the selected folder
			}

			if (isSelected)
			{
				EditorGUI::StyleColorPop_Selected();
			}

			// Get the filename and truncate it if necessary to fit thumbnail width.
			std::string directory_name = directory_path.filename().string();
			std::string displayname = TruncateTextToWidth(directory_name, cell_size + padding);

			// Render the text on a single line (without wrapping).
			ImGui::Text("%s", displayname.c_str());
			ImGui::EndGroup();

			ImGui::PopID();

			// Place the item on the same row if it's not the last in the row.
			if ((item_index + 1) % items_per_row != 0)
			{
				ImGui::SameLine(0, padding);
			}
			item_index++;
		}
		
		for (auto& file : folder.files)
		{
			ImGui::PushID(file.string().c_str());

			// Group the thumbnail and its label together
			ImGui::BeginGroup();

			bool found_texture = false;
			std::string extension = file.extension().string();
			std::string assetkey = file.generic_string();
			assetkey.insert(0, "/");

			if (extension == ".jpg" || extension == ".jpeg" || extension == ".png")
			{
				found_texture = true;
			}
			else if (extension == ".glsl" || extension == ".hlsl" || extension == ".frag" || extension == ".vert")
			{
				assetkey = m_shader_image_key.string();
				found_texture = true;
			}
			else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg" || extension == ".flac")
			{
				assetkey = m_audio_image_key.string();
				found_texture = true;
			}
			else if (extension == ".flxprefab")
			{
				assetkey = m_flxprefab_image_key.string();
				found_texture = true;
			}
			else if (extension == ".ttf")
			{
				assetkey = m_font_image_key.string();
				//Asset::Texture& texture = AssetManager::Get<Asset::Texture>(m_font_image_key.string());
				//if (ImGui::ImageButton(texture.GetTextureImGui(), ImVec2(thumbnail_size, thumbnail_size)))
				//{
				//	m_selected_file = file;
				//}
				found_texture = true;
			}


			bool isSelected = (m_selected_file == file);
			if (isSelected)
			{
				EditorGUI::StyleColorPush_Selected();
			}

			if (found_texture)
			{
				Asset::Texture& texture = AssetManager::Get<Asset::Texture>(assetkey);
				if (ImGui::ImageButton(texture.GetTextureImGui(), ImVec2(thumbnail_size, thumbnail_size)))
				{
					m_selected_file = file;
				}
				ImGui::SetItemTooltip(file.filename().string().c_str());
			}
			else
			{
				if (ImGui::ImageButton((ImTextureID)0, ImVec2(thumbnail_size, thumbnail_size)))
				{
					m_selected_file = file;
				}
				ImGui::SetItemTooltip(file.filename().string().c_str());
			}

			if (isSelected)
			{
				EditorGUI::StyleColorPop_Selected();
			}


			//Drag and drop assets
			if (ImGui::BeginDragDropSource())
			{
				std::string payload(file.generic_string());
				payload.insert(0, "/");	//to fit the AssetKey format

				//hardcode for now
				if (extension == ".jpg" || extension == ".jpeg" || extension == ".png")
				{
					EditorGUI::StartPayload(PayloadTags::IMAGE, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
					EditorGUI::EndPayload();
				}
				else if (extension == ".flxspritesheet")
				{
					EditorGUI::StartPayload(PayloadTags::SPRITESHEET, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
					EditorGUI::EndPayload();
				}
				else if (extension == ".glsl" || extension == ".hlsl" || extension == ".frag" || extension == ".vert")
				{
					payload = payload.substr(0, payload.find_last_of('.')); //to fit the AssetKey::Shader format
					EditorGUI::StartPayload(PayloadTags::SHADER, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
					EditorGUI::EndPayload();
				}
				else if (extension == ".mp3" || extension == ".wav" || extension == ".ogg" || extension == ".flac")
				{
					EditorGUI::StartPayload(PayloadTags::AUDIO, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
					EditorGUI::EndPayload();
				}
				else if (extension == ".flxprefab")
				{
					EditorGUI::StartPayload(PayloadTags::PREFAB, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
					EditorGUI::EndPayload();
				}
				else if (extension == ".ttf")
				{
					EditorGUI::StartPayload(PayloadTags::FONT, payload.c_str(), payload.size() + 1, file.filename().string().c_str());
					EditorGUI::EndPayload();
				}
				else
				{
					//Find rocky if you want your filetype supported
					ImGui::Text("Unsupported file type: %s", extension.c_str());
					EditorGUI::EndPayload();
				}
			}


			// Get the filename and truncate it if necessary to fit thumbnail width.
			std::string filename = file.filename().string();
			std::string displayname = TruncateTextToWidth(filename, cell_size + padding);

			// Render the text on a single line (without wrapping).
			ImGui::Text("%s", displayname.c_str());
			ImGui::EndGroup();


			DeleteFilePopup(file);


			ImGui::PopID();

			// Place the item on the same row if it's not the last in the row.
			if ((item_index + 1) % items_per_row != 0)
			{
				ImGui::SameLine(0, padding);
			}
			item_index++;
		}
	}

	void AssetBrowser::RenderFolderDropdown2(Folder& folder)
	{
		ImGuiTreeNodeFlags node_flags = (folder.subfolders.size() < 1) ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

		// Highlight the selected folder
		if (m_selected_folder == &folder)
			node_flags |= ImGuiTreeNodeFlags_Selected;

		// Render folder node
		if (ImGui::TreeNodeEx((folder.name + "##" + folder.path.string()).c_str(), node_flags))
		{
			// Check if the folder is clicked
			if (ImGui::IsItemClicked())
			{
				m_selected_folder = &folder; // Set the selected folder
			}

			// Render subfolders recursively
			for (auto& [subfolder_path, subfolder] : folder.subfolders)
			{
				RenderFolderDropdown(*subfolder);
			}

			ImGui::TreePop();
		}
	}


	void AssetBrowser::OnFileDropped(const std::vector<std::string>& file_list)
	{

		ImVec2 mouse_pos = ImGui::GetIO().MousePos;
		// Check if the mouse is over the right panel.
		if (mouse_pos.x < m_right_panel_rect.min.x || mouse_pos.x > m_right_panel_rect.max.x ||
				mouse_pos.y < m_right_panel_rect.min.y || mouse_pos.y > m_right_panel_rect.max.y)
		{
			//Dropped file outside of panel
			return;
		}

		//Check folder to put file in
		std::filesystem::path destination = m_root_directory;
		if (m_selected_folder)
		{
			destination /= std::filesystem::path(m_selected_folder->path);
		}
		else
		{
			return;
		}

		//save the path, so we can navigate back to the folder after refreshing directory structure
		std::filesystem::path old_selected_folder_path = m_selected_folder->path;

		for (auto file : file_list)
		{
			std::filesystem::path src(file);

			destination /= src.filename();

			if (!CopyFileA(src.string().c_str(), destination.string().c_str(), FALSE))
			{
				Log::Error("Failed to copy file.");
			}
			else
			{
				Log::Info("Successfully copied file.");
				//Create assetkey
				std::filesystem::path assetkey_dest = "../../assets";
				assetkey_dest /= std::filesystem::path(m_selected_folder->path);
				assetkey_dest /= src.filename();

				AssetManager::LoadFileFromPath(assetkey_dest, m_root_directory);
			}
		}
		LoadAllDirectories();
		m_selected_folder = &m_directories[old_selected_folder_path];
	}
}







