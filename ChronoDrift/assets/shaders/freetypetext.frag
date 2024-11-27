/*!************************************************************************
* FREETYPETEXT.SHADER [freetypetext.frag]
*
* This file implements the fragment shader for rendering 2D text using a 
* bitmap font texture. It samples the texture and applies a color uniform 
* to render text with customizable colors.
*
* Key functionalities include:
* - Sampling the font texture to retrieve alpha values for text rendering.
* - Applying a user-defined color to the sampled text.
* - Outputting the final color of each fragment.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*   - Developed text sampling and color blending logic.
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#version 460 core

out vec4 fragment_color;

in vec2 tex_coord;

// font
uniform sampler2D text;
uniform vec3 u_color;

void main()
{
   vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, tex_coord).r);
   fragment_color = vec4(u_color, 1.0) * sampled;
}