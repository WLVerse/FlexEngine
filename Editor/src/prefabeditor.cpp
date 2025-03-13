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
// Copyright (c) 2025 DigiPen, All rights reserved.
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
		prefab_keys.push_back(std::make_tuple("test", "test", true, true));
		prefab_keys.push_back(std::make_tuple("tester", "tester", std::string("Hello2 World!"), true));
		prefab_keys.push_back(std::make_tuple("testint", "testint", 1243, true));
	}

	void PrefabEditor::LoadPrefab() {
		// Open the file browser
		FileList fl = FileList::Browse(
			"Open Prefab",
			Path::current("assets/data"), "default.flxdata",
			L"FlexData Files (*.flxdata)\0" L"*.flxdata\0",
			//L"All Files (*.*)\0" L"*.*\0",
			false,
			false
		);

		// cancel if no files were selected
		if (fl.empty()) {
			Log::Error("No file was selected");
			return;
		}
		file_name.first = fl;
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
				prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetString(key_name, ""), true));
				continue;
			}
			
			size_t colon_pos = temp.find(':', end_pos + 1);

			// Boolean Data Type
			if (temp.substr(colon_pos + 2, 4) == "true" || temp.substr(colon_pos + 2, 5) == "false") {
				prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetBool(key_name, false), true));
			}
			else {
				// Integer Data Type
				prefab_keys.push_back(std::make_tuple(key_name, key_name, prefab.GetInt(key_name, 0), true));
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
				std::filesystem::rename(Path::source("assets/data/" + file.path.filename().string()).c_str(),
					Path::source("assets/data/" + file_name.second + file.path.extension().string()).c_str());
				Log::Info("Renaming file success.");
			}
		}

		Asset::FlxData prefab_to_save(file_name.second + ".flxdata");

		for (int i = 0; i < prefab_keys.size(); i++) {
			// If field is to be deleted
			if (std::get<3>(prefab_keys[i]) == false) {
				prefab_to_save.DeleteKey(std::get<0>(prefab_keys[i]));
				continue;
			}

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
			file_name.first.push_back(Path::source("assets/data/" + file_name.second + ".flxdata"));
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
				std::filesystem::remove(Path::source("assets/data/" + file_name.first[0].filename().string()));
				Log::Info("Prefab deleted successfully.");
				prefab_keys.clear();
				file_name.first.clear();
				file_name.second.clear();
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
				// Do not display fields to delete
				if (std::get<3>(prefab_keys[i]) == false) continue;
				
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
						std::get<3>(prefab_keys[i]) = false; // To not save this field
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
					prefab_keys.push_back(std::make_tuple("tester", "tester", std::string("Hello2 World!"), true));
				}
				if (ImGui::MenuItem("Integer")) {
					prefab_keys.push_back(std::make_tuple("testint", "testint", 1243, true));
				}
				if (ImGui::MenuItem("Boolean")) {
					prefab_keys.push_back(std::make_tuple("test", "test", true, true));
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
	}
}