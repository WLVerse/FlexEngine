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