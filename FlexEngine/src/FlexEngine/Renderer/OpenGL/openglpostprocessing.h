#pragma once
/*!************************************************************************
// WLVERSE [https://wlverse.web.app]
// openglpostprocessing.h
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

#include "OpenGLFrameBuffer.h"
#include "FlexMath/Matrix4x4.h"
#include "FlexMath/Vector3.h"
#include "opengltexture.h"

namespace FlexEngine 
{
    class OpenGLPostProcessing 
    {
        static GLuint m_VAOid;

        static Asset::Shader m_bloom_brightness_shader;
        static Asset::Shader m_bloom_gaussianblur_shader;
        static Asset::Shader m_bloom_finalcomp_shader;
    public:
        /*!***************************************************************************
        * \brief
        * Refer to .cpp
        *****************************************************************************/
        static void Init(GLuint VAOIDtoSet = 0);
        /*!***************************************************************************
        * \brief
        * Applies a brightness threshold pass for the bloom effect.
        *
        * \param threshold The brightness threshold to apply.
        *****************************************************************************/
        static void ApplyBrightnessPass(float threshold = 1.0f);
        /*!***************************************************************************
        * \brief
        * Applies a Gaussian blur effect with specified passes, blur distance, and intensity.
        *
        * \param blurDrawPasses The number of passes to apply for the blur.
        * \param blurDistance The distance factor for the blur effect.
        * \param intensity The intensity of the blur.
        *****************************************************************************/
        static void ApplyGaussianBlur(int blurPasses = 4, float blurDistance = 10.0f, int intensity = 12);
        /*!***************************************************************************
        * \brief
        * Applies the final bloom composition with a specified opacity level.
        *
        * \param opacity The opacity level for the bloom composition.
        *****************************************************************************/
        static void ApplyBloomFinalComposition(float opacity = 1.0f);
    };
}