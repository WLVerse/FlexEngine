#version 330 core

in vec2 tex_coord;
out vec4 FragColor;

uniform sampler2D backgroundTex; // Current framebuffer texture
uniform sampler2D objTex;        // Object texture with black background

void main()
{
    // Sample both textures at the current coordinates
    vec4 bgColor = texture(backgroundTex, tex_coord);
    vec4 objColor = texture(objTex, tex_coord);

    // Compute brightness from the object texture.
    // Non-black pixels (above threshold) are considered part of the object.
    float threshold = 0.03;  // Adjust this value to account for noise or anti-aliasing
    float brightness = length(objColor.rgb);
    float mask = smoothstep(threshold, threshold + 0.05, brightness);
    
    // Blend the object on top of the background based on the mask.
    // Where mask == 0, background remains unchanged;
    // where mask == 1, object completely overwrites background.
    vec4 finalColor = mix(bgColor, objColor, mask);
    
    FragColor = finalColor;
}
