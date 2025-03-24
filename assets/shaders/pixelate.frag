#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D u_InputTex;   // The input scene texture
uniform float u_PixelWidth;     // Pixel block width (in screen pixels)
uniform float u_PixelHeight;    // Pixel block height (in screen pixels)

void main()
{
    // Get the resolution of the input texture
    ivec2 texSize = textureSize(u_InputTex, 0);
    
    // Compute the normalized block size.
    vec2 blockSize = vec2(u_PixelWidth / float(texSize.x),
                          u_PixelHeight / float(texSize.y));
    
    // Snap the texture coordinates to the center of the block.
    vec2 uv = floor(vTexCoord / blockSize) * blockSize + blockSize * 0.5;
    
    // Sample the input texture using the pixelated coordinate.
    vec3 color = texture(u_InputTex, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}
