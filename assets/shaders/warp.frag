#version 330 core

in vec2 tex_coord;
out vec4 FragColor;

uniform sampler2D screenTexture;  
uniform float warpStrength;   // Positive for pincushion, negative for barrel distortion
uniform float maxRadius;      // Effective radius for distortion (e.g., 0.5)

void main()
{
    // Calculate the offset from the specified center
    vec2 offset = tex_coord - vec2(0.5,0.5);
    float r = length(offset);
    
    // Outside the effective radius, no distortion is applied.
    if(r > maxRadius)
    {
        FragColor = texture(screenTexture, tex_coord);
        return;
    }
    
    // Convert the offset to polar coordinates
    float theta = atan(offset.y, offset.x);
    
    // Normalize the radius relative to the maximum distortion radius.
    float rn = r / maxRadius;
    
    // Apply a radial distortion function.
    // Here, the radius is scaled quadratically. This simulates focal length stretch.
    float rDistorted = r * (1.0 + warpStrength * rn * rn);
    
    // Reconstruct the texture coordinates using the distorted radius.
    vec2 newUV = vec2(0.5,0.5) + vec2(cos(theta), sin(theta)) * rDistorted;
    
    // Clamp coordinates to ensure they remain within [0, 1]
    newUV = clamp(newUV, 0.0, 1.0);
    
    FragColor = texture(screenTexture, newUV);
}
