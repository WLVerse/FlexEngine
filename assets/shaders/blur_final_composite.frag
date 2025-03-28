#version 330 core

out vec4 FragColor;
in vec2 tex_coord;

uniform sampler2D blurHTex;  // Horizontal blur texture
uniform sampler2D blurVTex;  // Vertical blur texture

void main()
{
    // Sample both blur textures
    vec3 blurH = texture(blurHTex, tex_coord).rgb;
    vec3 blurV = texture(blurVTex, tex_coord).rgb;
    
    // Blend them together
    vec3 finalColor = mix(blurH, blurV, 0.5);
    
    FragColor = vec4(finalColor, 1.0);
}
