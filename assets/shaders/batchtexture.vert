#version 460 core

// Input vertex data
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_tex_coord;

// Input SSBO data
layout(std430, binding = 0) buffer InstanceDataBuffer 
{
    mat4 transformation[];
};
layout(std430, binding = 1) buffer ColorBuffer 
{
    vec3 u_Color_to_add[];
};
layout(std430, binding = 2) buffer ColorMultiplyBuffer 
{
    vec3 u_Color_to_multiply[];
};
layout(std430, binding = 3) buffer AnimationUVBuffer
{
    vec4 u_Uv[];
};

// Uniforms
uniform mat4 u_projection_view;

// Output data
out vec2 tex_coord;
out vec3 u_color_to_add;
out vec3 u_color_to_multiply;

void main()
{
  // multiplication is right to left
  gl_Position = u_projection_view * transformation[gl_InstanceID] * vec4(m_position, 1.0);

  // data passthrough
  tex_coord = mix(u_Uv[gl_InstanceID].xy, u_Uv[gl_InstanceID].zw, m_tex_coord);
  u_color_to_add = u_Color_to_add[gl_InstanceID];
  u_color_to_multiply = u_Color_to_multiply[gl_InstanceID];

}
