/*!************************************************************************
* FREETYPETEXT.SHADER [freetypetext.vert]
*
* This file implements the vertex shader for rendering 2D text with texture 
* coordinates. It transforms vertex positions from local space to clip space 
* and passes texture coordinates to the fragment shader.
*
* Key functionalities include:
* - Transforming vertex positions using model and projection matrices.
* - Passing texture coordinates to the fragment shader for texturing.
* - Preparing vertices for rendering in 2D space.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*   - Developed vertex transformation logic and texture coordinate handling.
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#version 460 core

// Input vertex data
layout(location = 0) in vec4 m_vertex; // (pos.x, pos.y, tex.s, tex.t)

// Uniforms
uniform mat4 u_model;       // converts local space to world space
//uniform mat4 u_view;        // converts world space to view space
//uniform mat4 u_projection;  // converts view space to clip space
//uniform mat4 u_projection_view;
uniform mat4 projection;

// Output data
out vec2 tex_coord;

void main()
{
    // multiplication is right to left
    gl_Position = projection * u_model * vec4(m_vertex.xy, 0.0, 1.0);
    tex_coord = m_vertex.zw;
}