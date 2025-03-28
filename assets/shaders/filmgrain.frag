#version 330 core

in vec2 tex_coord;
out vec4 FragColor;

uniform sampler2D u_InputTex;         // Input scene texture.
uniform float u_FilmGrainIntensity;   // How strong the grain effect is.
uniform float u_FilmGrainSize;        // Controls the size/scale of the noise.
uniform int u_FilmGrainAnimate;       // 0 = static, 1 = animated.
uniform float u_Time;                 // Time in seconds (used if animation is enabled).

// Simple pseudo-random noise function based on texture coordinates.
float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    // Sample the base color.
    vec3 color = texture(u_InputTex, tex_coord).rgb;
    
    // Compute grain coordinates by scaling texture coordinates by the grain size.
    vec2 grainUV = tex_coord * u_FilmGrainSize;
    if(u_FilmGrainAnimate == 1)
    {
        // Offset coordinates by time for animated noise.
        grainUV += vec2(u_Time);
    }
    
    // Generate a noise value in [0,1]
    float noise = rand(grainUV);
    // Remap to [-0.5, 0.5] so that noise is centered.
    noise = noise - 0.5;
    
    // Add the noise to the original color scaled by the grain intensity.
    color += noise * u_FilmGrainIntensity;
    
    // Clamp the result to ensure valid color range.
    FragColor = vec4(clamp(color, 0.0, 1.0), 1.0);
}
