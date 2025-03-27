#version 460 core

layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_tex_coord;

out vec2 tex_coord;

void main()
{
    // Directly use vertex positions (assumed to be in clip space).
    gl_Position = vec4(m_position, 1.0);
    tex_coord = m_tex_coord;
}