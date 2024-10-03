#version 460 core

// Input line data
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

// Uniforms
uniform mat4 u_view;        // converts world space to view space
uniform mat4 u_projection;  // converts view space to clip space

out vec3 f_color;

void main()
{
  f_color = a_color;

  gl_Position = u_projection * u_view * vec4(a_position, 1.0);
}
