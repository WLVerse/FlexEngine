/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// openglpostprocessing.cpp
//
// This source file defines the `OpenGLPostProcessing` class, which manages
// post-processing effects in the rendering pipeline using OpenGL. It includes
// functionality for bloom effects, Gaussian blur, and final composition to
// enhance the visual output.
//
// Key Responsibilities:
// - Load and manage shaders for various stages of post-processing:
//   - Brightness threshold extraction for bloom.
//   - Gaussian blur application for softening.
//   - Final composition of bloom with the main scene.
// - Manage rendering state for post-processing via vertex array objects (VAOs).
// - Utilize `OpenGLFrameBuffer` for managing intermediate rendering textures.
// - Integrate with `FreeQueue` for deferred shader cleanup.
//
// Main Features:
// - Flexible brightness threshold control for bloom extraction.
// - Multi-pass Gaussian blur implementation with adjustable intensity and distance.
// - Seamless final composition of bloom with configurable opacity.
// - Shader and resource initialization, cleanup, and error logging.
//
// AUTHORS
// [100%] Soh Wei Jie (weijie.soh@digipen.edu)
//   - Main Author
//   - Implemented bloom brightness pass, Gaussian blur, and final composition.
//
// Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#include "OpenGLPostProcessing.h"

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
    /*!***************************************************************************
     * \brief
     * Initializes the OpenGLPostProcessing system by setting up shaders and VAO.
     *
     * This function prepares the post-processing pipeline by creating and compiling
     * the necessary shaders for bloom brightness extraction, Gaussian blur, and final
     * composition. It also sets the Vertex Array Object (VAO) to be used during rendering
     * and registers cleanup tasks to ensure proper resource management.
     *
     * \param VAOIDtoSet
     * The ID of the Vertex Array Object (VAO) to be used for rendering post-processing
     * effects. This VAO should be bound during the initialization process.
     *
     * \details
     * - Loads shader programs from pre-defined file paths for shared vertex operations
     *   and fragment processing specific to each stage of post-processing.
     * - Registers deferred cleanup operations for all initialized shaders using
     *   the `FreeQueue` system.
     * - Logs a message upon successful initialization.
     *
     * \note
     * The function assumes that the VAO provided by `VAOIDtoSet` is valid and properly
     * configured to handle post-processing geometries, such as screen-space quads.
     *
     * \warning
     * Ensure that the shader files exist at the specified paths. Missing or invalid files
     * will cause the initialization process to fail.
     *****************************************************************************/
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
        OpenGLFrameBuffer::SetGameFrameBuffer();
        GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(1, drawBuffers);

        m_bloom_finalcomp_shader.Use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, OpenGLFrameBuffer::m_gameRenderTex); // Original scene texture
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
    }
    #pragma endregion
}