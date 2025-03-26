#version 330 core

in vec2 tex_coord;
out vec4 FragColor;

uniform sampler2D screenTexture;  
uniform float warpStrength; // e.g. 0.3 for moderate warping

void main()
{
    vec2 center = vec2(0.5, 0.5);
    vec2 offset = tex_coord - center;
    float dist = length(offset);

    // Define the distance (in UV space) at which the warp effect fades out.
    // At distances >= maxDist, the UVs remain unchanged.
    float maxDist = 0.5;

    // Compute a scaling factor that is minimum at the center and 1.0 at the edges.
    // At the center (dist = 0), factor = (1.0 - warpStrength), and it linearly (via smoothstep)
    // approaches 1.0 at dist >= maxDist.
    float factor = mix(1.0 - warpStrength, 1.0, smoothstep(0.0, maxDist, dist));

    vec2 warpedCoords = center + offset * factor;

    FragColor = texture(screenTexture, warpedCoords);
}
