#version 460 core

// Input vertex data
layout (location = 0) in vec3 m_position;
layout (location = 1) in vec2 m_tex_coord;

//layout(std140, binding = 0) buffer m_InstanceTransforms 
//{
//    mat4 m_instanceTransforms[]; // Array of transformations for each instance
//};
layout(std140) uniform Matrices 
{
    mat4 transformations[100];
};

// Uniforms
//uniform mat4 u_model;       // converts local space to world space
//uniform mat4 u_view;        // converts world space to view space
//uniform mat4 u_projection;  // converts view space to clip space
uniform mat4 u_projection_view;

// Output data
out vec2 tex_coord;

void main()
{
  // Use gl_InstanceID to fetch the specific transformation matrix for this instance
  mat4 instanceTransform = transformations[gl_InstanceID];

  // multiplication is right to left
  gl_Position = u_projection_view * instanceTransform * vec4(m_position, 1.0);

  // data passthrough
  tex_coord = m_tex_coord;
}