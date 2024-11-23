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