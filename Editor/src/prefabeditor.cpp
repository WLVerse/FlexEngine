/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// prefabeditor.cpp
//
// Brief
//
// AUTHORS
// [100%] Ho Jin Jie Donovan (h.jinjiedonovan\@digipen.edu)
//   - Main Author
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "editor.h"
#include "prefabeditor.h"
#include "flxdata.h"

#include <filesystem> // For Renaming and Deleting FlxData files

using namespace FlexEngine;
using EntityName = FlexEngine::FlexECS::Scene::StringIndex;

namespace Editor
{
	void PrefabEditor::Init()
	{}
	void PrefabEditor::Update()
	{}
	void PrefabEditor::Shutdown()
	{}

	void PrefabEditor::CreatePrefab() {
		prefab_keys.clear();
		file_name.first.clear();
		file_name.second = "flxer";
		prefab_keys.push_back(std::make_tuple("test", "test", true));
		prefab_keys.push_back(std::make_tuple("tester", "tester", std::string("Hello2 World!")));
		prefab_keys.push_back(std::make_tuple("testint", "testint", 1243));
	}

	void PrefabEditor::LoadPrefab() {
		// Open the file browser
		file_name.first = FileList::Browse(
			"Open Prefab",
			Path::current("assets/data"), "default.flxdata",
			L"FlexData Files (*.flxdata)\0" L"*.flxdata\0",
			//L"All Files (*.*)\0" L"*.*\0",
			false,
			false
		);

		// cancel if no files were selected
		if (file_name.first.empty()) {
			Log::Error("No file was selected");
			return;
		}

		prefab_keys.clear();

		// open the prefab
		File& file = File::Open(file_name.first[0]);

		// read the file details
		std::istringstream input_stream(file.Read());

		// Store file name inside variable file_name
		file_name.second = file.path.filename().stem().string();

		// Access the prefab
		Asset::FlxData prefab(file.path.filename().string());

		std::string temp;
		while (std::getline(input_stream, temp)) {
			size_t start_pos = temp.find('\"');
			if (start_pos == std::string::npos) continue;

			size_t end_pos = temp.find('\"', start_pos + 1);
			std::string key_name = temp.substr(start_pos + 1, end_pos - start_pos - 1);

			// Skip unneeded line
			if (key_name == "flxdata") continue;

			// String Data Type
			if (temp.find('\"', end_pos + 1) != std::string::npos) {
				prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetString(key_name, "")));
				continue;
			}
			
			size_t colon_pos = temp.find(':', end_pos + 1);

			// Boolean Data Type
			if (temp.substr(colon_pos + 2, 4) == "true" || temp.substr(colon_pos + 2, 5) == "false") {
				prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetBool(key_name, false)));
			}
			else {
				// Integer Data Type
				prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetInt(key_name, 0)));
			}
		}
	}

	void PrefabEditor::SavePrefab() {
		if (file_name.second == "") Log::Error("No file selected or created");

		// Check whether new file name is valid or not
		if (!std::all_of(file_name.second.begin(), file_name.second.end(), [](char ch) {
			return std::isalnum(static_cast<unsigned char>(ch)); // Check if the character is alphanumeric
		})) {
			Log::Error("File name is not valid!");
			ImGui::End();
			return;
		}

		if (!file_name.first.empty()) {
			File& file = File::Open(file_name.first[0]);
			// Check whether file name is to be changed
			if (file.path.filename().stem().string() != file_name.second) {
				std::filesystem::rename(Path::current(file.path.filename().string()).c_str(),
					Path::current(file_name.second + file.path.extension().string()).c_str());
				Log::Info("Renaming file success.");
			}
		}

		Asset::FlxData prefab_to_save(file_name.second + ".flxdata");

		for (int i = 0; i < prefab_keys.size(); i++) {
			// If key name is changed, delete old key and create new key
			if (!file_name.first.empty() && std::get<1>(prefab_keys[i]) != std::get<0>(prefab_keys[i])) {
				prefab_to_save.DeleteKey(std::get<0>(prefab_keys[i]));
			}

			if (std::get_if<std::string>(&std::get<2>(prefab_keys[i]))) {
				prefab_to_save.SetString(std::get<1>(prefab_keys[i]), std::get<std::string>(std::get<2>(prefab_keys[i])));
			}
			else if (std::get_if<int>(&std::get<2>(prefab_keys[i]))) {
				prefab_to_save.SetInt(std::get<1>(prefab_keys[i]), std::get<int>(std::get<2>(prefab_keys[i])));
			}
			else if (std::get_if<bool>(&std::get<2>(prefab_keys[i]))) {
				prefab_to_save.SetBool(std::get<1>(prefab_keys[i]), std::get<bool>(std::get<2>(prefab_keys[i])));
			}
		}
		prefab_to_save.Save();

		// This does not work
		if (file_name.first.empty()) {
			file_name.first.push_back(Path::current(file_name.second + ".flxdata"));
			file_name.second = file_name.first[0].filename().stem().string();
		}

		for (auto& p : FlexECS::Scene::GetActiveScene()->CachedQuery<Prefab>()) {
			if (p.GetComponent<Prefab>()->prefab_name == file_name.second) {
				// Update the values of the entity based on the new prefab values
			}
		}
	}

	void PrefabEditor::DeletePrefab() {
		if (!prefab_keys.empty()) {
			if (file_name.first.empty()) {
				prefab_keys.clear();
				file_name.second.clear();
			}
			else {
				try {
					std::filesystem::remove(Path::source("assets/data/" + file_name.first[0].filename().string()));
					Log::Info("Prefab deleted successfully.");
					prefab_keys.clear();
					file_name.first.clear();
					file_name.second.clear();
				}
				catch (const std::filesystem::filesystem_error& e) {
					// Handle rename error (optional error logging)
					Log::Error("Unable to delete prefab");
				}
			}
		}
		else Log::Error("No prefab selected");
	}

	void PrefabEditor::PrefabDisplay() {
		// Prefab Editor Imgui editable fields
		if (!prefab_keys.empty()) {
			// Prefab file name
			ImGui::Text("Prefab File Name:");
			ImGui::SameLine(200);

			char prefab_file_name[128];
			strncpy_s(prefab_file_name, file_name.second.c_str(), sizeof(prefab_file_name) - 1);
			prefab_file_name[sizeof(prefab_file_name) - 1] = '\0';
			if (ImGui::InputText("##hidden_label", prefab_file_name, IM_ARRAYSIZE(prefab_file_name))) {
				file_name.second = std::string(prefab_file_name);
			}

			for (int i = 0; i < prefab_keys.size(); i++) {
				char field_name[128];
				strncpy_s(field_name, std::get<1>(prefab_keys[i]).c_str(), sizeof(field_name) - 1);
				field_name[sizeof(field_name) - 1] = '\0';

				ImGui::PushID(i);
				ImGui::PushItemWidth(175); // Set the width to 175 pixels
				if (ImGui::InputText("##hidden_label", field_name, sizeof(field_name))) {
					// The text has been changed by the user
					// You can handle the new text here
					std::get<1>(prefab_keys[i]) = std::string(field_name);
				}
				ImGui::PopItemWidth(); // Restore the default width
				ImGui::PopID();

				ImGui::SameLine(200);

				// Push a unique ID for each item to avoid conflicts
				ImGui::PushID(static_cast<int>(prefab_keys.size() + i));

				if (std::get_if<std::string>(&std::get<2>(prefab_keys[i]))) {
					char text[256];
					strncpy_s(text, std::get<std::string>(std::get<2>(prefab_keys[i])).c_str(), sizeof(text) - 1);
					text[sizeof(text) - 1] = '\0';

					if (ImGui::InputText("##hidden_label", text, sizeof(text))) {
						// The text has been changed by the user
						// You can handle the new text here
						std::get<2>(prefab_keys[i]) = std::string(text);
					}
				}
				else if (std::get_if<int>(&std::get<2>(prefab_keys[i]))) {
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x); // Set the width to 175 pixels
					if (ImGui::InputInt("##hidden_label", &std::get<int>(std::get<2>(prefab_keys[i])))) {
						// The number has been changed by the user
						// Handle the updated integer here
					}
					ImGui::PopItemWidth();
				}
				else if (std::get_if<bool>(&std::get<2>(prefab_keys[i]))) {
					if (ImGui::Checkbox("##hidden_label", &std::get<bool>(std::get<2>(prefab_keys[i])))) {
						// The checkbox value has been changed
						// Handle the updated state here
					}
				}

				// Create a right-click context menu for each field
				if (ImGui::BeginPopupContextItem("FieldContextMenu")) {
					if (ImGui::Selectable("Delete field")) {
						prefab_keys.erase(prefab_keys.begin() + i); // Remove the field
						--i; // Adjust index after removal
						ImGui::EndPopup();
						ImGui::PopID();
						continue; // Skip the rest of this loop iteration
					}
					ImGui::EndPopup();
				}
				ImGui::PopID();
			}
			// To add new fields for the prefab
			if (ImGui::Button("Add field")) {
				ImGui::OpenPopup("OptionsPopup");
			}

			if (ImGui::BeginPopup("OptionsPopup")) {
				if (ImGui::MenuItem("String")) {
					prefab_keys.push_back(std::make_tuple("tester", "tester", std::string("Hello2 World!")));
				}
				if (ImGui::MenuItem("Integer")) {
					prefab_keys.push_back(std::make_tuple("testint", "testint", 1243));
				}
				if (ImGui::MenuItem("Boolean")) {
					prefab_keys.push_back(std::make_tuple("test", "test", true));
				}
				ImGui::EndPopup();
			}
		}
	}

	void PrefabEditor::EditorUI()
	{
		ImGui::Begin("Prefab Editor", nullptr, ImGuiWindowFlags_MenuBar);
		if (ImGui::BeginMenuBar()) {
			if (ImGui::MenuItem("New")) {
				CreatePrefab();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Open")) {
				LoadPrefab();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Save")) {
				SavePrefab();
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Delete")) {
				DeletePrefab();
			}
			ImGui::EndMenuBar();
		}
		PrefabDisplay();
		ImGui::End();
		
		#if 0
		// Create New Prefab
		if (ImGui::Button("Create Prefab")) {
			prefab_keys.clear();
			//file_name.second = "assets/data/flxer.flxdata";
			file_name.second = "flxer";
			prefab_keys.push_back(std::make_tuple("test", "test", true));
			prefab_keys.push_back(std::make_tuple("tester", "tester", std::string("Hello2 World!")));
			prefab_keys.push_back(std::make_tuple("testint", "testint", 1243));
		}
		ImGui::SameLine(175);

		// Load Existing Prefab
		if (ImGui::Button("Load Prefab")) {
			prefab_keys.clear();
			file_name.first = FileList::Browse(
				"Open Prefab",
				Path::current("assets/data"), "default.flxdata",
				L"FlexData Files (*.flxdata)\0" L"*.flxdata\0",
				//L"All Files (*.*)\0" L"*.*\0",
				false,
				false
			);
			
			// cancel if no files were selected
			if (file_name.first.empty()) {
				Log::Error("No file was selected");
			}
			else {
				// open the prefab
				File& file = File::Open(file_name.first[0]);
				
				// read the file details
				std::istringstream input_stream(file.Read());
				std::string temp;
				file_name.second = file.path.filename().stem().string();

				// Access the prefab
				Asset::FlxData prefab(file.path.filename().string());

				while (std::getline(input_stream, temp)) {
					size_t start_pos = temp.find('\"');
					if (start_pos != std::string::npos) {
						size_t end_pos = temp.find('\"', start_pos + 1);
						std::string key_name = temp.substr(start_pos + 1, end_pos - start_pos - 1);
						// Skip unneeded line
						if (key_name == "flxdata") continue;

						if (temp.find('\"', end_pos + 1) != std::string::npos) {
							// String Data Type
							prefab_keys.push_back(std::make_tuple(key_name,key_name, prefab.GetString(key_name, "")));
						}
						else {
							size_t colon_pos = temp.find(':', end_pos + 1);
							if (temp.substr(colon_pos + 2, 4) == "true" || temp.substr(colon_pos + 2, 5) == "false") {
								// Boolean Data Type
								prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetBool(key_name, false)));
							}
							else {
								// Integer Data Type
								prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetInt(key_name, 0)));
							}
						}
					}
				}
			}
		}
		ImGui::SameLine(350);
		
		// Delete Prefab
		if (ImGui::Button("Delete Prefab")) {
			if (!prefab_keys.empty()) ImGui::OpenPopup("Delete Prefab");
			else Log::Error("No prefab selected");
		}

		if (ImGui::BeginPopupModal("Delete Prefab", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::Button("Confirm")) {
				if (file_name.first.empty()) {
					prefab_keys.clear();
					file_name.second.clear();
				}
				else {
					try {
						std::filesystem::remove(Path::current(file_name.first[0].filename().string()));
						Log::Info("Prefab deleted successfully.");
						prefab_keys.clear();
						file_name.first.clear();
						file_name.second.clear();
						ImGui::CloseCurrentPopup();
					}
					catch (const std::filesystem::filesystem_error& e) {
						// Handle rename error (optional error logging)
						Log::Error("Unable to delete prefab");
					}
				}
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		
		// Prefab Editor Imgui editable fields
		if (!prefab_keys.empty()) {
			// Prefab file name
			ImGui::Text("Prefab File Name:");
			ImGui::SameLine(200);

			char prefab_file_name[128];
			strncpy_s(prefab_file_name, file_name.second.c_str(), sizeof(prefab_file_name) - 1);
			prefab_file_name[sizeof(prefab_file_name) - 1] = '\0';
			if (ImGui::InputText("##hidden_label", prefab_file_name, sizeof(prefab_file_name))) {
				file_name.second = std::string(prefab_file_name);
			}

			for (int i = 0; i < prefab_keys.size(); i++) {
				char field_name[128];
				strncpy_s(field_name, std::get<1>(prefab_keys[i]).c_str(), sizeof(field_name) - 1);
				field_name[sizeof(field_name) - 1] = '\0';

				ImGui::PushID(i);
				ImGui::PushItemWidth(175); // Set the width to 175 pixels
				if (ImGui::InputText("##hidden_label", field_name, sizeof(field_name))) {
					// The text has been changed by the user
					// You can handle the new text here
					std::get<1>(prefab_keys[i]) = std::string(field_name);
				}
				ImGui::PopItemWidth(); // Restore the default width
				ImGui::PopID();

				ImGui::SameLine(200);

				// Push a unique ID for each item to avoid conflicts
				ImGui::PushID(static_cast<int>(prefab_keys.size() + i));

				if (std::get_if<std::string>(&std::get<2>(prefab_keys[i]))) {
					char text[256];
					strncpy_s(text, std::get<std::string>(std::get<2>(prefab_keys[i])).c_str(), sizeof(text) - 1);
					text[sizeof(text) - 1] = '\0';
					
					if (ImGui::InputText("##hidden_label", text, sizeof(text))) {
						// The text has been changed by the user
						// You can handle the new text here
						std::get<2>(prefab_keys[i]) = std::string(text);
					}
				}
				else if (std::get_if<int>(&std::get<2>(prefab_keys[i]))) {
					if (ImGui::InputInt("##hidden_label", &std::get<int>(std::get<2>(prefab_keys[i])))) {
						// The number has been changed by the user
						// Handle the updated integer here
					}
				}
				else if (std::get_if<bool>(&std::get<2>(prefab_keys[i]))) {
					if (ImGui::Checkbox("##hidden_label", &std::get<bool>(std::get<2>(prefab_keys[i])))) {
						// The checkbox value has been changed
						// Handle the updated state here
					}
				}

				// Create a right-click context menu for each field
				if (ImGui::BeginPopupContextItem("FieldContextMenu")) {
					if (ImGui::Selectable("Delete")) {
						prefab_keys.erase(prefab_keys.begin() + i); // Remove the field
						--i; // Adjust index after removal
						ImGui::EndPopup();
						ImGui::PopID();
						continue; // Skip the rest of this loop iteration
					}
					ImGui::EndPopup();
				}

				ImGui::PopID();
			}
			// To add new fields for the prefab
			if (ImGui::BeginMenu("Add field")) {
				if (ImGui::MenuItem("String")) {
					prefab_keys.push_back(std::make_tuple("tester","tester", std::string("Hello2 World!")));
				}
				if (ImGui::MenuItem("Integer")) {
					prefab_keys.push_back(std::make_tuple("testint","testint", 1243));
				}
				if (ImGui::MenuItem("Boolean")) {
					prefab_keys.push_back(std::make_tuple("test","test", true));
				}
				ImGui::EndMenu();
			}
		}

		// Save Prefab
		if (ImGui::Button("Save Prefab")) {
			bool is_renamed_file_valid = true;

			if (file_name.second == "") {
				Log::Error("No file selected or created");
			}
			else {
				if (!file_name.first.empty()) {
					File& file = File::Open(file_name.first[0]);
					// Check whether file name is to be changed
					if (file.path.filename().stem().string() != file_name.second) {
						// Check whether new file name is valid or not
						if (std::all_of(file_name.second.begin(), file_name.second.end(), [](char ch) {
							return std::isalnum(static_cast<unsigned char>(ch)); // Check if the character is alphanumeric
						})) {
							std::filesystem::rename(Path::current(file.path.filename().string()).c_str(),
							Path::current(file_name.second + file.path.extension().string()).c_str());
							Log::Info("Renaming file success.");
						}
						else {
							Log::Error("File name is not valid!");
							is_renamed_file_valid = false;
						}
					}
				}
				else file_name.second = "assets/data/" + file_name.second;

				if (!is_renamed_file_valid) {
					ImGui::End();
					return;
				}
					
				Asset::FlxData prefab_to_save(file_name.second + ".flxdata");
				for (int i = 0; i < prefab_keys.size(); i++) {
					// If key name is changed, delete old key and create new key
					if (std::get<1>(prefab_keys[i]) != std::get<0>(prefab_keys[i])) {
						prefab_to_save.DeleteKey(std::get<0>(prefab_keys[i]));
					}
					if (std::get_if<std::string>(&std::get<2>(prefab_keys[i]))) {
						prefab_to_save.SetString(std::get<1>(prefab_keys[i]), std::get<std::string>(std::get<2>(prefab_keys[i])));
					}
					else if (std::get_if<int>(&std::get<2>(prefab_keys[i]))) {
						prefab_to_save.SetInt(std::get<1>(prefab_keys[i]), std::get<int>(std::get<2>(prefab_keys[i])));
					}
					else if (std::get_if<bool>(&std::get<2>(prefab_keys[i]))) {
						prefab_to_save.SetBool(std::get<1>(prefab_keys[i]), std::get<bool>(std::get<2>(prefab_keys[i])));
					}
				}
				prefab_to_save.Save();

				if (file_name.first.empty()) {
					file_name.first.push_back(Path::current(file_name.second + ".flxdata"));
					file_name.second = file_name.first[0].filename().stem().string();
				}

				for (auto& p : FlexECS::Scene::GetActiveScene()->CachedQuery<Prefab>()) {
					if (p.GetComponent<Prefab>()->prefab_name == file_name.second) {
						// Update the values of the entity based on the new prefab values
					}
				}
			}
		}
		#endif
	}
}