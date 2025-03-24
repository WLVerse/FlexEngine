#version 460 core

in vec2 tex_coord;    
out vec4 FragColor;

uniform sampler2D u_InputTex;       // Input texture (scene)
uniform float u_VignetteIntensity;    // How strong the vignette darkening is (0 = none, 1 = full effect)
uniform float u_VignetteRadius;       // The radius (in normalized coordinates) where the vignette starts (e.g., 0.5 means center area is unaffected)
uniform float u_VignetteSoftness;     // Controls how gradually the effect transitions toward the edges

void main()
{
    // Sample the base color.
    vec3 color = texture(u_InputTex, tex_coord).rgb;
    
    // Compute the distance from the center (0.5, 0.5) in normalized texture coordinates.
    float dist = distance(tex_coord, vec2(0.5, 0.5));
    
    // Compute the vignette factor using smoothstep.
    // Pixels with a distance less than u_VignetteRadius are unaffected;
    // Beyond that, the effect transitions over u_VignetteSoftness.
    float vig = smoothstep(u_VignetteRadius, u_VignetteRadius - u_VignetteSoftness, dist);
    
    // Apply the vignette: darken the color based on the vignette factor and intensity.
    color *= (1.0 - u_VignetteIntensity * vig);
    
    FragColor = vec4(color, 1.0);
}
