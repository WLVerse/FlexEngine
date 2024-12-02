/*!************************************************************************
* WLVERSE [https://wlverse.web.app]
*
* \file bloom_gaussian_blurD.frag
* \brief
* Fragment shader for directional Gaussian blur with diagonal support.
* Designed for applying advanced bloom effects in post-processing pipelines.
*
* Key functionalities:
* - Implements Gaussian blur with user-defined sample intensity and spread.
* - Supports horizontal, vertical, and diagonal blurring directions.
* - Dynamically computes Gaussian weights for each sample.
*
* Uniforms:
* - sampler2D scene: The input texture to be blurred.
* - bool horizontal: Controls the primary blur direction.
* - float blurDistance: Determines the spread of the blur.
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

    // Apply blur: horizontal, vertical, and diagonal
    for (int i = 1; i < intensity; i++)
    {
        vec2 horiz_offset = vec2(tex_offset.x * float(i), 0.0); // Horizontal
        vec2 vert_offset = vec2(0.0, tex_offset.y * float(i));  // Vertical
        vec2 diag1_offset = vec2(tex_offset.x * float(i), tex_offset.y * float(i));  // Diagonal: top-left to bottom-right
        vec2 diag2_offset = vec2(-tex_offset.x * float(i), tex_offset.y * float(i)); // Diagonal: top-right to bottom-left

        // Horizontal blur (if horizontal is true, apply it horizontally, otherwise, apply vertically)
        if (horizontal) {
            result += texture(scene, tex_coord + horiz_offset).rgb * weights[i];
            result += texture(scene, tex_coord - horiz_offset).rgb * weights[i];
        } else {
            result += texture(scene, tex_coord + vert_offset).rgb * weights[i];
            result += texture(scene, tex_coord - vert_offset).rgb * weights[i];
        }

        // Apply diagonal blur in both directions
        result += texture(scene, tex_coord + diag1_offset).rgb * weights[i];
        result += texture(scene, tex_coord - diag1_offset).rgb * weights[i];
        result += texture(scene, tex_coord + diag2_offset).rgb * weights[i];
        result += texture(scene, tex_coord - diag2_offset).rgb * weights[i];
    }

    FragColor = vec4(result, 1.0f);
}
