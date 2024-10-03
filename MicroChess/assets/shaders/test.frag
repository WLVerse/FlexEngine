#version 330 core

out vec4 FragColor;
in vec2 tex_coord;

uniform sampler2D screenTexture;
uniform sampler2D bloomTexture;
uniform float gamma;

void main()
{
    vec3 fragment = texture(screenTexture, tex_coord).rgb;
    vec3 bloom = texture(bloomTexture, tex_coord).rgb;

    FragColor = vec4(1.0,0.0,0.0, 1.0);  // Additive blending
    //vec3 color = fragment + bloom;

    //float exposure = 0.8f;
    //vec3 toneMapped = vec3(1.0f) - exp(-color * exposure);

    //FragColor.rgb = pow(toneMapped, vec3(1.0f / gamma));
}