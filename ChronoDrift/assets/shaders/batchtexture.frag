/*!************************************************************************
* BATCHTEXTURE.SHADER [batchtexture.frag]
*
* This file implements the fragment shader for rendering 2D sprites in batch.
* It supports texture sampling, color transformation, and optional texture usage 
* for dynamic rendering of sprites.
*
* Key functionalities include:
* - Sampling textures to fetch color data.
* - Applying color addition and multiplication for custom sprite appearance.
* - Clamping color values to ensure valid color output.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*   - Developed texture sampling and color manipulation logic.
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#version 460 core

out vec4 fragment_color;

in vec2 tex_coord;
in vec3 u_color_to_add;
in vec3 u_color_to_multiply;

// texture
uniform sampler2D u_texture;
uniform bool u_use_texture;

void main()
{
  vec3 diffuse = vec3(1.0, 0.0, 1.0);
  double alpha = 1.0;
  if (u_use_texture)
  {
    vec4 tex = texture(u_texture, tex_coord);
    diffuse = tex.rgb;
    alpha = tex.a;
  }
  else
  {
    diffuse = vec3(0.0, 0.0, 0.0);
    alpha = 1.0;
  }

  //fragment_color = vec4(diffuse, alpha);

  vec3 result = diffuse * u_color_to_multiply + u_color_to_add;
  result = clamp(result, 0.0, 1.0);
  fragment_color = vec4(result, alpha);
}
