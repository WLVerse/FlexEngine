#version 460 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D u_texture; // The atlas texture
uniform vec3 u_color;

void main()
{
    // Sample the atlas texture (we assume the glyph bitmap is stored in the red channel)
    float sampled = texture(u_texture, TexCoord).r;
    // Multiply the glyph’s alpha by u_color
    FragColor = vec4(u_color, sampled);
}