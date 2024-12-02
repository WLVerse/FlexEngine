/*!***************************************************************************
* WLVERSE [https://wlverse.web.app]
*
* \file bloom_final_composite.frag
* \brief
* Fragment shader responsible for combining the original scene with bloom
* effects, creating the final composite image. Uses horizontal and vertical
* bloom passes for enhanced visuals.
*
* Key functionalities:
* - Fetches base scene color and bloom contributions from textures.
* - Blends the bloom effect with the scene using an opacity parameter.
* - Outputs the final color to the framebuffer.
*
* Uniforms:
* - sampler2D screenTex: The original scene texture.
* - sampler2D bloomVTex: The vertically blurred bloom texture.
* - sampler2D bloomHTex: The horizontally blurred bloom texture.
* - float opacity: Controls the intensity of the bloom effect.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*
* Copyright (c) 2024 DigiPen, All rights reserved.
****************************************************************************/

#version 330 core

out vec4 FragColor;
in vec2 tex_coord;

uniform sampler2D screenTex;
uniform sampler2D bloomVTex;
uniform sampler2D bloomHTex;
uniform float opacity;  // Controls bloom intensity

void main()
{
    // Fetch base color from the screen texture
    vec3 fragment = texture(screenTex, tex_coord).rgb;

    // Fetch bloom color
    vec3 bloomV = texture(bloomVTex, tex_coord).rgb;
    vec3 bloomH = texture(bloomHTex, tex_coord).rgb;
    vec3 bloom = mix(bloomH, bloomV, 0.5f);

    // Perform overlay blending with opacity control
    vec3 blended = mix(fragment, fragment + bloom, opacity); //might need to change

    // Output the final fragment color
    FragColor = vec4(blended, 1.0);  // Full alpha
}