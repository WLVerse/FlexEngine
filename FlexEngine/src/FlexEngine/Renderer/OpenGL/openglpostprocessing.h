#pragma once
#include <glad/glad.h>
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