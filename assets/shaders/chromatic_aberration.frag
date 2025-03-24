#version 330 core

in vec2 tex_coord;
out vec4 FragColor;

uniform sampler2D u_InputTex;       // Input texture
uniform float u_ChromaIntensity;    // Global intensity multiplier
uniform vec2 u_RedOffset;           // Base red offset (in pixels)
uniform vec2 u_GreenOffset;         // Base green offset (in pixels)
uniform vec2 u_BlueOffset;          // Base blue offset (in pixels)

void main()
{
    ivec2 texSize = textureSize(u_InputTex, 0);
    vec2 texDim = vec2(texSize);

    // Convert pixel-based offsets into normalized texture space.
    vec2 normalizedRed = (u_RedOffset / texDim) * u_ChromaIntensity;
    vec2 normalizedGreen = (u_GreenOffset / texDim) * u_ChromaIntensity;
    vec2 normalizedBlue = (u_BlueOffset / texDim) * u_ChromaIntensity;

    // Sample each channel with its offset.
    float red   = texture(u_InputTex, tex_coord + normalizedRed).r;
    float green = texture(u_InputTex, tex_coord + normalizedGreen).g;
    float blue  = texture(u_InputTex, tex_coord + normalizedBlue).b;

    FragColor = vec4(red, green, blue, 1.0);
}
