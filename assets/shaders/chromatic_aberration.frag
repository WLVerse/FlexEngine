#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D u_InputTex;      // Input texture
uniform float u_ChromaIntensity;   // Global intensity multiplier
uniform vec2 u_RedOffset;          // Base red offset
uniform vec2 u_GreenOffset;        // Base green offset
uniform vec2 u_BlueOffset;         // Base blue offset
uniform vec2 u_MaxOffset;          // Maximum allowed offset for each channel

void main()
{
    // Calculate per-channel offsets (scaled by intensity) and clamp them.
    vec2 redOff   = clamp(u_RedOffset * u_ChromaIntensity, -u_MaxOffset, u_MaxOffset);
    vec2 greenOff = clamp(u_GreenOffset * u_ChromaIntensity, -u_MaxOffset, u_MaxOffset);
    vec2 blueOff  = clamp(u_BlueOffset * u_ChromaIntensity, -u_MaxOffset, u_MaxOffset);

    // Sample the input texture for each color channel with its offset.
    float red   = texture(u_InputTex, vTexCoord + redOff).r;
    float green = texture(u_InputTex, vTexCoord + greenOff).g;
    float blue  = texture(u_InputTex, vTexCoord + blueOff).b;
    
    FragColor = vec4(red, green, blue, 1.0);
}
