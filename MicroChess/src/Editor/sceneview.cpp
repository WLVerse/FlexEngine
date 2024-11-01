#include "sceneview.h"
#include "editorgui.h"
#include "imguipayloads.h"
#include <FlexEngine/Renderer/OpenGL/openglspriterenderer.h>
#include "ImGuizmo/ImGuizmo.h"


namespace ChronoShift
{



	void SceneView::Init()
	{

	}

	void SceneView::Update()
	{
		WindowProps window_props = Application::GetCurrentWindow()->GetProps();
		Renderer2DProps props;
		props.window_size = { static_cast<float>(window_props.width), static_cast<float>(window_props.height) };

		FunctionQueue finalized_render_queue;



		for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, ZIndex, Transform, Shader, Sprite>())
		{
			auto entity_name_component = entity.GetComponent<EntityName>();

			if (!entity.GetComponent<IsActive>()->is_active) continue;
			auto& z_index = entity.GetComponent<ZIndex>()->z;
			Matrix4x4 transform = entity.GetComponent<Transform>()->transform;
			auto& shader = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(entity.GetComponent<Shader>()->shader);
			auto sprite = entity.GetComponent<Sprite>();

			props.shader = shader;
			props.transform = transform;
			props.texture = FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(sprite->texture);
			props.color = sprite->color;
			props.color_to_add = sprite->color_to_add;
			props.color_to_multiply = sprite->color_to_multiply;
			props.alignment = static_cast<Renderer2DProps::Alignment>(sprite->alignment);
			props.vbo_id = sprite->vbo_id;

			finalized_render_queue.Insert({ [props]() { OpenGLSpriteRenderer::DrawTexture2D(OpenGLSpriteRenderer::GetCreatedTexture(OpenGLSpriteRenderer::CID_editor),props); }, "", z_index });

		}
	}
	void SceneView::Shutdown()
	{
	}


	void SceneView::EditorUI()
	{
		WindowProps window_props = Application::GetCurrentWindow()->GetProps();
		Renderer2DProps props;
		props.window_size = { static_cast<float>(window_props.width), static_cast<float>(window_props.height) };

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollWithMouse;

		ImGui::Begin("Scene", nullptr, window_flags);
		//drag object into scene to create it
		if (auto image = EditorGUI::StartWindowPayloadReceiver<const char>(PayloadTags::IMAGE))
		{
			auto scene = FlexECS::Scene::GetActiveScene();

			std::string image_key(image);
			std::filesystem::path path = image_key;

			FlexECS::Entity new_entity = scene->CreateEntity(path.filename().string());
			new_entity.AddComponent<IsActive>({ true });
			new_entity.AddComponent<Position>({ Vector2::One * 500.0f });
			new_entity.AddComponent<Rotation>({});
			new_entity.AddComponent<Scale>({ Vector2::One * 100.0f });
			new_entity.AddComponent<Transform>({});
			new_entity.AddComponent<ZIndex>({});
			new_entity.AddComponent<Sprite>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(image_key) });
			new_entity.AddComponent<Shader>({ FlexECS::Scene::GetActiveScene()->Internal_StringStorage_New(R"(\shaders\texture)") });
			EditorGUI::EndPayloadReceiver();
		}

		//Calculating size of display image
		ImVec2 panel_size = ImGui::GetWindowSize();
		float width = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth());
		float height = static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight());

		if (height > panel_size.y)
		{
			float aspect_ratio = width / height;
			height = panel_size.y;
			width = height * aspect_ratio;
		}
		ImVec2 viewport_size(width, height);
		ImVec2 viewport_position((panel_size.x - viewport_size.x) / 2.0f, (panel_size.y - viewport_size.y) / 2.0f);
		//static ImVec2 viewport_position((panel_size.x - viewport_size.x) / 2.0f, (panel_size.y - viewport_size.y) / 2.0f);

		ImGui::SetCursorPos(viewport_position);
		ImGui::Image((ImTextureID)OpenGLSpriteRenderer::GetCreatedTexture(OpenGLSpriteRenderer::CreatedTextureID::CID_editor),
			viewport_size, ImVec2(0, 1), ImVec2(1, 0)); //Note: need to invert UVs vertically.



		//Object Picking
		if (ImGui::IsWindowFocused() && ImGui::IsMouseClicked(0))
		{
			ImVec2 global_mouse_pos = ImGui::GetMousePos();
			ImVec2 window_pos = ImGui::GetWindowPos();
			//ImVec2 window_pos = viewport_position;
			float local_mouse_x = global_mouse_pos.x - window_pos.x - viewport_position.x;
			float local_mouse_y = global_mouse_pos.y - window_pos.y - viewport_position.y;

			//convert to world pos coordinates
			//we dont have a scene camera yet, just take window size
			float x_scale = Application::GetCurrentWindow()->GetWidth() / viewport_size.x;
			float y_scale = Application::GetCurrentWindow()->GetHeight() / viewport_size.y;

			float world_x = local_mouse_x * x_scale;
			float world_y = local_mouse_y * y_scale;

			//I want to get only things that are rendered
			for (auto& entity : FlexECS::Scene::GetActiveScene()->View<IsActive, Position, Scale, Transform, Shader, Sprite>())
			{
				if (!entity.GetComponent<IsActive>()->is_active) continue;

				Vector2& position = entity.GetComponent<Position>()->position;
				Vector2& scale = entity.GetComponent<Scale>()->scale;

				//AABB
				if (world_x < (position.x - scale.x / 2) || world_x >(position.x + scale.x / 2)) continue;
				if (world_y < (position.y - scale.y / 2) || world_y >(position.y + scale.y / 2)) continue;

				std::cout << "Found entity: " << FlexECS::Scene::GetActiveScene()->Internal_StringStorage_Get(*entity.GetComponent<EntityName>()) << "\n";
				Editor::GetInstance().SelectEntity(entity);
				break;
			}
		}



		//Display Gizmos
		FlexECS::Entity selected_entity = Editor::GetInstance().GetSelectedEntity();
		if (selected_entity != FlexECS::Entity::Null)
		{
			//std::cout << "there exists a selected entity\n";
			if (selected_entity.HasComponent<Transform>())
			{
				ImGuiIO& io = ImGui::GetIO();
				ImVec2 imgui_window_pos = ImGui::GetWindowPos();
				static Matrix4x4 camera_view =
				{
					1.0f, 0.0f, 0.0f, 0.f,
					0.f, 1.0f, 0.f, 0.f,
					0.f, 0.f, 1.0f, 0.f,
					0.f, 0.f, 0.f, 1.f
				};
				static Matrix4x4 camera_projection =
				{
					-0.0015625f, 0.f, -0.f, 0.f,
					0.f, -0.0027777777f, 0.0f, 0.f,
					0.f, -0.f, 1, 0.0f,
					-1.0f, 1.f, -1.f, 1.f
				};
				//Matrix4x4 camera_projection = Matrix4x4::Orthographic(0, 1280, 0, 720, -2.0f, 2.0f);

				//static Matrix4x4 objectMatrix =
				//{
				//		1.f, 0.f, 0.f, 0.f,
				//		0.f, 1.f, 0.f, 0.f,
				//		0.f, 0.f, 1.f, 0.f,
				//		selected_entity.GetComponent<Transform>()->transform.data[12], selected_entity.GetComponent<Transform>()->transform.data[13], 0.f, 1.f
				//};
				camera_projection.Dump();

				ImGuizmo::SetOrthographic(true);
				ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());
				ImGuizmo::SetRect(imgui_window_pos.x + viewport_position.x, imgui_window_pos.y + viewport_position.y, viewport_size.x, viewport_size.y);
				ImGuizmo::BeginFrame();
				ImGuizmo::Manipulate(camera_view.data, camera_projection.data, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, selected_entity.GetComponent<Transform>()->transform.data);
			}
		}
		ImGui::End();
	}
}




/*
*
* my failure documented here
*

void OrthoGraphic(const float l, float r, float b, const float t, float zn, const float zf, float* m16)
	{
		m16[0] = 2 / (r - l);
		m16[1] = 0.0f;
		m16[2] = 0.0f;
		m16[3] = 0.0f;
		m16[4] = 0.0f;
		m16[5] = 2 / (t - b);
		m16[6] = 0.0f;
		m16[7] = 0.0f;
		m16[8] = 0.0f;
		m16[9] = 0.0f;
		m16[10] = 1.0f / (zf - zn);
		m16[11] = 0.0f;
		m16[12] = (l + r) / (l - r);
		m16[13] = (t + b) / (b - t);
		m16[14] = zn / (zn - zf);
		m16[15] = 1.0f;
	}

	void EditTransform(float* cameraView, float* cameraProjection, float* matrix)
	{
		ImGuiIO& io = ImGui::GetIO();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		if (true)
		{
			ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		}
		else
		{
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		}
		if (ImGuizmo::Manipulate(cameraView, cameraProjection, ImGuizmo::TRANSLATE, ImGuizmo::WORLD, matrix))
		{
			std::cout << "touching me...\n";
		}
		#if 0
		std::string camstring = "Camera Matrix4x4\n";
		for (int i = 0; i < 4; ++i)
		{
			camstring += "(";
			for (int j = 0; j < 4; ++j)
			{
				camstring += std::to_string(cameraView[i * 4 + j]);
				camstring += ", ";
			}
			camstring.pop_back();
			camstring.pop_back();
			camstring += ")\n";
		}
		std::string projstring = "Projection Matrix4x4\n";
		for (int i = 0; i < 4; ++i)
		{
			projstring += "(";
			for (int j = 0; j < 4; ++j)
			{
				projstring += std::to_string(cameraProjection[i * 4 + j]);
				projstring += ", ";
			}
			projstring.pop_back();
			projstring.pop_back();
			projstring += ")\n";
		}
		std::cout << camstring << "\n" << projstring << "\n";
		#endif
	}

	void MousePicking();

		GLuint m_picking_fbo;
		GLuint m_picking_texture;
		GLuint m_picking_texture_high;
		GLuint m_picking_texture_low;
		GLuint m_picking_depth_texture;

		std::string m_picking_shader = "\\shaders\\Editor\\editorpicking";
* 		WindowProps window_props = Application::GetCurrentWindow()->GetProps();

		#if 0
		glGenFramebuffers(1, &m_picking_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_picking_fbo);

		// High 32 bits
		glGenTextures(1, &m_picking_texture_high);
		glBindTexture(GL_TEXTURE_2D, m_picking_texture_high);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, window_props.width, window_props.height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_picking_texture_high, 0);

		// Low 32 bits
		glGenTextures(1, &m_picking_texture_low);
		glBindTexture(GL_TEXTURE_2D, m_picking_texture_low);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, window_props.width, window_props.height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_picking_texture_low, 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "Framebuffer not complete!" << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		#endif

		#if 1
		glGenFramebuffers(1, &m_picking_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, m_picking_fbo);

		//Use Texture object as information buffer (entity id)
		glGenTextures(1, &m_picking_texture);
		glBindTexture(GL_TEXTURE_2D, m_picking_texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, window_props.width, window_props.height, 0, GL_RGB_INTEGER, GL_UNSIGNED_INT, NULL);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32UI, window_props.width, window_props.height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, window_props.width, window_props.height, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_picking_texture, 0);

		//Depth buffer texture object
		//glGenTextures(1, &m_picking_depth_texture);
		//glBindTexture(GL_TEXTURE_2D, m_picking_depth_texture);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_props.width, window_props.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		#endif
//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_picking_fbo);
				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_picking_fbo);
				GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, drawBuffers); // Set both attachments for drawing

				//unsigned int clear_color = 0;
				//glClearBufferuiv(GL_COLOR, 0, &clear_color);
				//glClearBufferuiv(GL_COLOR, 1, &clear_color);

				auto& picking_shader = FLX_ASSET_GET(Asset::Shader, m_picking_shader);
				picking_shader.Use();

				picking_shader.SetUniform_mat4("u_model", props.transform);
				static const Matrix4x4 view_matrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);
				Matrix4x4 projection_view = Matrix4x4::Orthographic(
					0.0f, props.window_size.x,
					props.window_size.y, 0.0f,
					-2.0f, 2.0f
				) * view_matrix;
				picking_shader.SetUniform_mat4("u_projection_view", projection_view);
				std::cout << "The data I want to send: " << static_cast<unsigned int>(entity.Get()) << "\n";
				picking_shader.SetUniform_uint("u_entity_id", 33333);
				//picking_shader.SetUniform_uint("u_entity_id", static_cast<unsigned int>(entity.Get()));

				//uint64_t entity_id = static_cast<uint64_t>(entity.Get());
				//unsigned int high_part = static_cast<unsigned int>(entity_id >> 32);
				//unsigned int low_part = static_cast<unsigned int>(entity_id & 0xFFFFFFFF);

				//picking_shader.SetUniform_uint("u_entity_id_high", high_part);
				//picking_shader.SetUniform_uint("u_entity_id_low", low_part);

				glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);



						#if 0
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_picking_fbo);
		glReadBuffer(GL_COLOR_ATTACHMENT0);

		unsigned int data;
		glReadPixels(
			//static_cast<unsigned int>(io.MousePos.x), static_cast<unsigned int>(io.MousePos.y),
			100, 100,
			1, 1,
			GL_R32UI, GL_UNSIGNED_INT,
			&data);
		std::cout << "THE DATA READ: " << data << "\n";
		#endif



				std::cout << "Scene click!\n";
		ImGuiIO& io = ImGui::GetIO();
		std::cout << "MouseX: " << io.MousePos.x << "  MouseY: " << io.MousePos.y << "\n";

		WindowProps window_props = Application::GetCurrentWindow()->GetProps();

		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_picking_fbo);

		for (size_t x = 0; x < window_props.width; x += 10)
		{
			for (size_t y = 0; y < window_props.height; y += 10)
			{
				unsigned int data, low_part;

				//glReadBuffer(GL_COLOR_ATTACHMENT0);
				glNamedFramebufferReadBuffer(m_picking_fbo, GL_COLOR_ATTACHMENT0);
				glReadPixels(static_cast<int>(x), static_cast<int>(y), 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &data);


				// Combine high and low parts into uint64_t
				uint64_t entity_id = data;
				std::cout << "Picked Entity ID: " << data << "\n";
				if (data != 0)
				{
				}
			}
			std::cout << "end of my read :(\n";

				*/