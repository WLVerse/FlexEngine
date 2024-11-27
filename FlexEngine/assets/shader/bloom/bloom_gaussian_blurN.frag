/*!************************************************************************
* WLVERSE [https://wlverse.web.app]
*
* \file bloom_gaussian_blurN.frag
* \brief
* Fragment shader for Gaussian blur, optimized for horizontal and vertical
* blurring in post-processing pipelines.
*
* Key functionalities:
* - Implements Gaussian blur with dynamic sample intensity and spread.
* - Blurs textures in horizontal or vertical directions based on a flag.
* - Computes and normalizes Gaussian weights for accurate results.
*
* Uniforms:
* - sampler2D scene: The input texture to be blurred.
* - bool horizontal: Flag for horizontal or vertical blur.
* - float blurDistance: Defines the spread of the blur effect.
* - int intensity: Specifies the number of blur samples.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D scene;
uniform bool horizontal;
uniform float blurDistance; // Distance for blurring (spread)
uniform int intensity; // Number of samples (blur strength)

// Maximum sample size for safety
const int maxSamples = 64;
float weights[maxSamples];

void main()
{
    // Compute Gaussian weights
    float x = 0.0f;
    float weightSum = 0.0f;
    for (int i = 0; i < intensity; i++)
    {
        x = float(i) * blurDistance / float(intensity);
        weights[i] = exp(-0.5f * pow(x / blurDistance, 2.0f)) / (blurDistance * sqrt(2.0f * 3.14159265f));
        weightSum += (i == 0) ? weights[i] : 2.0f * weights[i]; // Symmetry, double weight for non-center samples
    }

    // Normalize the weights
    for (int i = 0; i < intensity; i++) {
        weights[i] /= weightSum;
    }

    vec2 tex_offset = 1.0f / textureSize(scene, 0);
    vec3 result = texture(scene, tex_coord).rgb * weights[0];

    // Apply blur
    for (int i = 1; i < intensity; i++)
    {
        vec2 offset = horizontal ? vec2(tex_offset.x * float(i), 0.0) : vec2(0.0, tex_offset.y * float(i));

        result += texture(scene, tex_coord + offset).rgb * weights[i];
        result += texture(scene, tex_coord - offset).rgb * weights[i];
    }

    FragColor = vec4(result, 1.0f);
}
