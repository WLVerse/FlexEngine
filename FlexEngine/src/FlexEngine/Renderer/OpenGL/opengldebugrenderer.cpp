#include "opengldebugrenderer.h"

#include "FlexEngine/AssetManager/assetmanager.h" // FLX_ASSET_GET
#include "FlexEngine/DataStructures/freequeue.h"
#include "FlexEngine/Core/application.h"
#include "openglspriterenderer.h"

#include <filesystem>

namespace FlexEngine
{
    //static declarations
    Asset::Shader OpenGLDebugRenderer::m_line_shader;
    std::filesystem::path current_file_path = __FILE__;
    std::filesystem::path vertex_shader_path(current_file_path.parent_path() / "../../../../assets/shader/debugshaders/debugrenderer.vert");
    std::filesystem::path fragment_shader_path(current_file_path.parent_path() / "../../../../assets/shader/debugshaders/debugrenderer.frag");

    void FlexEngine::OpenGLDebugRenderer::DrawLine2D(Vector2 start, Vector2 end, Vector3 color, float line_width)
    {
        static float vertices[12];
        static GLuint vao = 0, vbo = 0;

        if (vao == 0)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

            glBindVertexArray(0);
            std::filesystem::path current_dir = std::filesystem::current_path();
            std::cout << current_dir;

            m_line_shader.Create(vertex_shader_path,
                                 fragment_shader_path);
            // free in freequeue
            FreeQueue::Push(
              [=]()
            {
                glDeleteBuffers(1, &vbo);
                glDeleteVertexArrays(1, &vao);
                m_line_shader.Destroy();
            }
            );
        }

        glNamedBufferSubData(vbo, 0, sizeof(vertices), (void*)vertices);

        //Update vertex info
        vertices[0] = start.x; vertices[1] = start.y; vertices[2] = 0.0f;
        vertices[3] = color.x; vertices[4] = color.y; vertices[5] = color.z;
        vertices[6] = end.x; vertices[7] = end.y; vertices[8] = 0.0f;
        vertices[9] = color.x; vertices[10] = color.y; vertices[11] = color.z;

        //////////////////////////////////////////////////////////////////////////////////
        //if want to edit line vbo -> goto spriterender init() and change there
        glBindVertexArray(vao/*OpenGLSpriteRenderer::GetVAO_ID(Renderer2DProps::VBO_Type::VBO_Line)*/);
        //glEnableVertexAttribArray(0); //not necessary as of yet
        //glEnableVertexAttribArray(1);

        float window_width = static_cast<float>(Application::GetCurrentWindow()->GetWidth());
        float window_height = static_cast<float>(Application::GetCurrentWindow()->GetHeight());

        m_line_shader.Use();
        //static const Matrix4x4 view_matrix = Matrix4x4::LookAt(Vector3::Zero, Vector3::Forward, Vector3::Up);
        Matrix4x4 view_matrix = Matrix4x4::Identity;

        Matrix4x4 projection_matrix = Matrix4x4::Orthographic(
          0.0f, window_width,
          window_height, 0.0f,
          -2.0f, 2.0f
        );

        m_line_shader.SetUniform_mat4("u_view", view_matrix);
        m_line_shader.SetUniform_mat4("u_projection", projection_matrix);

        glLineWidth(line_width);

        OpenGLFrameBuffer::SetEditorFrameBuffer();
        glDrawArrays(GL_LINES, 0, 2);
        OpenGLFrameBuffer::SetDefaultFrameBuffer();

        glBindVertexArray(0);
    }

    void OpenGLDebugRenderer::DrawAABB(Vector3 position, float width, float height, Vector3 color, float line_width)
    {
        UNREFERENCED_PARAMETER(color);
        UNREFERENCED_PARAMETER(line_width);
        Vector3 max = position;
        max.x += width / 2; max.y += height / 2;
        Vector3 min = position;
        min.x -= width / 2; min.y -= height / 2;

        //construct lines for AABB
        Vector3 topleft = min;
        Vector3 topright = { max.x, min.y };
        Vector3 botleft = { min.x, max.y };
        Vector3 botright = max;
        OpenGLDebugRenderer::DrawLine2D(topleft, topright);
        OpenGLDebugRenderer::DrawLine2D(topright, botright);
        OpenGLDebugRenderer::DrawLine2D(botright, botleft);
        OpenGLDebugRenderer::DrawLine2D(botleft, topleft);
    }

}