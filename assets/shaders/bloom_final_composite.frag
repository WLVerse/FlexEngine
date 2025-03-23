#version 330 core

out vec4 FragColor;
in vec2 tex_coord;

uniform sampler2D screenTex;
uniform sampler2D bloomVTex;
uniform sampler2D bloomHTex;
uniform float opacity;  // Controls bloom intensity

void main()
{
    // Fetch base color from the screen texture
    vec3 fragment = texture(screenTex, tex_coord).rgb;

    // Fetch bloom color
    vec3 bloomV = texture(bloomVTex, tex_coord).rgb;
    vec3 bloomH = texture(bloomHTex, tex_coord).rgb;
    vec3 bloom = mix(bloomH, bloomV, 0.5f);

    // Perform overlay blending with opacity control
    vec3 blended = mix(fragment, fragment + bloom, opacity); //might need to change

    // Output the final fragment color
    FragColor = vec4(blended, 1.0);  // Full alpha
}