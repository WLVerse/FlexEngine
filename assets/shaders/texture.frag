#version 460 core

out vec4 fragment_color;

in vec2 tex_coord;

// texture
uniform sampler2D u_texture;
//uniform vec3 u_color;
uniform vec3 u_color_to_add;
uniform vec3 u_color_to_multiply;
uniform float u_alpha;
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

  if (u_alpha != 1.0)
  {
      alpha = u_alpha;
  }

  vec3 result = diffuse * u_color_to_multiply + u_color_to_add;
  result = clamp(result, 0.0, 1.0);
  fragment_color = vec4(result, alpha);
}
