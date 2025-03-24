#version 330 core

out vec4 FragColor;
in vec2 tex_coord;

uniform sampler2D screenTex;
uniform sampler2D bloomVTex;
uniform sampler2D bloomHTex;
uniform float opacity;     // Controls bloom intensity
uniform float bloomRadius; // Bloom spread radius

void main()
{
    // Fetch base color from the screen texture
    vec3 fragment = texture(screenTex, tex_coord).rgb;

    // Sample bloom textures with radius factor
    vec2 tex_offset = bloomRadius * (1.0 / textureSize(screenTex, 0)); // Scale by bloomRadius

    // Sample slightly shifted bloom
    vec3 bloomV = texture(bloomVTex, tex_coord + tex_offset).rgb;
    vec3 bloomH = texture(bloomHTex, tex_coord - tex_offset).rgb;

    // Blend horizontal and vertical bloom
    vec3 bloom = mix(bloomH, bloomV, 0.5);

    // Enhance bloom by radius (adjust brightness)
    bloom *= bloomRadius;

    // Perform overlay blending with opacity control
    vec3 blended = mix(fragment, fragment + bloom, opacity);

    // Output the final fragment color
    FragColor = vec4(blended, 1.0);
}
