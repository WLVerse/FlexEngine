#include "OpenGLPostProcessing.h"
#include "OpenGLFrameBuffer.h"
#include "FlexEngine/DataStructures/freequeue.h"

namespace FlexEngine 
{
    Asset::Shader OpenGLPostProcessing::m_bloom_brightness_shader;
    Asset::Shader OpenGLPostProcessing::m_bloom_gaussianblur_shader;
    Asset::Shader OpenGLPostProcessing::m_bloom_finalcomp_shader;

    GLuint OpenGLPostProcessing::m_VAOid = 0;

    #pragma region File locations
    std::filesystem::path curr_file_path = __FILE__;
    std::filesystem::path shared_vert_path(curr_file_path.parent_path() / "../../../../assets/shader/Shared.vert");
    std::filesystem::path bloom_brightness_frag_path(curr_file_path.parent_path() / "../../../../assets/shader/bloom/bloom_bright_extraction.frag");
    std::filesystem::path bloom_blur_frag_path(curr_file_path.parent_path() / "../../../../assets/shader/bloom/bloom_gaussian_blurN.frag");
    std::filesystem::path bloom_final_frag_path(curr_file_path.parent_path() / "../../../../assets/shader/bloom/bloom_final_composite.frag");
    #pragma endregion

    #pragma region Init
    void OpenGLPostProcessing::Init(GLuint VAOIDtoSet)
    {
        m_VAOid = VAOIDtoSet;
        m_bloom_brightness_shader.Create(shared_vert_path, bloom_brightness_frag_path);
        m_bloom_gaussianblur_shader.Create(shared_vert_path, bloom_blur_frag_path);
        m_bloom_finalcomp_shader.Create(shared_vert_path, bloom_final_frag_path);
        FreeQueue::Push(
          [=]()
        {
            m_bloom_brightness_shader.Destroy();
            m_bloom_gaussianblur_shader.Destroy();
            m_bloom_finalcomp_shader.Destroy();
        }
        );
        Log::Info("All post-processing shaders are created.");
    }
    #pragma endregion

    #pragma region Features
    /*!***************************************************************************
    * \brief
    * Applies a brightness threshold pass for the bloom effect.
    *
    * \param threshold The brightness threshold to apply.
    *****************************************************************************/
    void OpenGLPostProcessing::ApplyBrightnessPass(float threshold) 
    {
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, drawBuffers);

        m_bloom_brightness_shader.Use();
        m_bloom_brightness_shader.SetUniform_float("u_Threshold", threshold);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OpenGLFrameBuffer::m_finalRenderTex); // Input: scene texture
        m_bloom_brightness_shader.SetUniform_int("scene", 0);

        glBindVertexArray(m_VAOid);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //m_draw_calls++;
    }

    ///*!***************************************************************************
    //* \brief
    //* Applies a Gaussian blur effect with specified passes, blur distance, and intensity.
    //*
    //* \param blurDrawPasses The number of passes to apply for the blur.
    //* \param blurDistance The distance factor for the blur effect.
    //* \param intensity The intensity of the blur.
    //*****************************************************************************/
    void OpenGLPostProcessing::ApplyGaussianBlur(int blurPasses, float blurDistance, int intensity) 
    {
        m_bloom_gaussianblur_shader.Use();
        bool horizontal = true;

        for (int i = 0; i < blurPasses; ++i)
        {
            GLenum drawBuffer = horizontal ? GL_COLOR_ATTACHMENT0 : GL_COLOR_ATTACHMENT1;
            glDrawBuffers(1, &drawBuffer);

            m_bloom_gaussianblur_shader.SetUniform_int("horizontal", horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, OpenGLFrameBuffer::m_pingpongTex[horizontal]);
            m_bloom_gaussianblur_shader.SetUniform_int("scene", 0);
            m_bloom_gaussianblur_shader.SetUniform_float("blurDistance", blurDistance);
            m_bloom_gaussianblur_shader.SetUniform_int("intensity", intensity);

            glBindVertexArray(m_VAOid);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            //m_draw_calls++;

            horizontal = !horizontal;
        }
    }

    ///*!***************************************************************************
    //* \brief
    //* Applies the final bloom composition with a specified opacity level.
    //*
    //* \param opacity The opacity level for the bloom composition.
    //*****************************************************************************/
    void OpenGLPostProcessing::ApplyBloomFinalComposition(float opacity) 
    {
        OpenGLFrameBuffer::SetEditorFrameBuffer();
        //GLenum drawBuffer = GL_COLOR_ATTACHMENT1;
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(1, drawBuffers);

        m_bloom_finalcomp_shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OpenGLFrameBuffer::m_editorTex); // Original scene texture
        m_bloom_finalcomp_shader.SetUniform_int("screenTex", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, OpenGLFrameBuffer::m_pingpongTex[0]); // Blur Vertical
        m_bloom_finalcomp_shader.SetUniform_int("bloomVTex", 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, OpenGLFrameBuffer::m_pingpongTex[1]); // Blur Horizontal
        m_bloom_finalcomp_shader.SetUniform_int("bloomHTex", 2);
        m_bloom_finalcomp_shader.SetUniform_float("opacity", opacity);

        glBindVertexArray(m_VAOid);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //m_draw_calls++;
    }
    #pragma endregion
}