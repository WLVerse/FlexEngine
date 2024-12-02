/*!************************************************************************
* TEXTURE.SHADER [texture.frag]
*
* This file implements the fragment shader for rendering 2D sprites with
* dynamic color transformation and optional texture sampling. It provides
* flexibility for both textured and non-textured sprite rendering with 
* customizable additive and multiplicative color adjustments.
*
* Key functionalities include:
* - Sampling texture data to retrieve color and alpha values.
* - Applying multiplicative and additive color transformations.
* - Supporting rendering with or without texture based on a boolean uniform.
* - Clamping resulting color values to ensure valid output.
*
* AUTHORS
* [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
*   - Main Author
*   - Developed texture sampling, color blending logic, and texture-less rendering support.
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#version 460 core

out vec4 fragment_color;

in vec2 tex_coord;

// texture
uniform sampler2D u_texture;
//uniform vec3 u_color;
uniform vec3 u_color_to_add;
uniform vec3 u_color_to_multiply;
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
