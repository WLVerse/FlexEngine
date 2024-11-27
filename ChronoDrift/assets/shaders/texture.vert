/*!************************************************************************
* TEXTURE.SHADER [texture.vert]
*
* This file implements the vertex shader for rendering 2D sprites. It handles 
* vertex transformations from local space to clip space and passes texture 
* coordinates to the fragment shader.
*
* Key functionalities include:
* - Transforming vertex positions using model and projection-view matrices.
* - Passing texture coordinates directly to the fragment shader.
* - Preparing vertices for accurate 2D sprite rendering.
*
* AUTHORS
* [100%] Chan Wen Loong (wenloong.c\@digipen.edu)
*   - Main Author
*   - Developed vertex transformation and texture coordinate handling logic.
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/

#version 460 core

// Input vertex data
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_tex_coord;

// Uniforms
uniform mat4 u_model;       // converts local space to world space
//uniform mat4 u_view;        // converts world space to view space
//uniform mat4 u_projection;  // converts view space to clip space
uniform mat4 u_projection_view;

// Output data
out vec2 tex_coord;

void main()
{
  // multiplication is right to left
  gl_Position = u_projection_view * u_model * vec4(m_position, 1.0);

  // data passthrough
  tex_coord = m_tex_coord;
}
