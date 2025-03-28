#version 330 core

in vec2 tex_coord;
out vec4 FragColor;

uniform sampler2D u_InputTex;       // Input texture
uniform float u_ChromaIntensity;    // Global intensity multiplier
uniform vec2 u_RedOffset;           // Base red offset (in pixels)
uniform vec2 u_GreenOffset;         // Base green offset (in pixels)
uniform vec2 u_BlueOffset;          // Base blue offset (in pixels)

// New uniforms for rectangular edge-based effect (normalized values)
uniform vec2 u_EdgeRadius;          // Distance from each edge (0 to 0.5) below which no effect is applied.
uniform vec2 u_EdgeSoftness;        // Transition range over which the effect ramps up.

void main()
{
    // Get texture dimensions (in pixels) and convert to a vec2.
    ivec2 texSize = textureSize(u_InputTex, 0);
    vec2 texDim = vec2(texSize);

    // Convert the pixel-based offsets into normalized texture space,
    // then scale by global intensity.
    vec2 normRed   = (u_RedOffset   / texDim) * u_ChromaIntensity;
    vec2 normGreen = (u_GreenOffset / texDim) * u_ChromaIntensity;
    vec2 normBlue  = (u_BlueOffset  / texDim) * u_ChromaIntensity;

    // Compute the distance from the fragment to each edge.
    float distX = min(tex_coord.x, 1.0 - tex_coord.x);
    float distY = min(tex_coord.y, 1.0 - tex_coord.y);

    // Compute an edge factor for horizontal and vertical directions.
    float factorX = smoothstep(u_EdgeRadius.x, u_EdgeRadius.x + u_EdgeSoftness.x, distX);
    float factorY = smoothstep(u_EdgeRadius.y, u_EdgeRadius.y + u_EdgeSoftness.y, distY);

    // Invert the factors so effect is strongest at the edges.
    factorX = 1.0 - factorX;
    factorY = 1.0 - factorY;

    // Combine edge factors.
    float edgeFactor = max(factorX, factorY);

    // Scale each channel's offset by the edge factor.
    normRed   *= edgeFactor;
    normGreen *= edgeFactor;
    normBlue  *= edgeFactor;

    // Sample the input texture for each color channel using its offset.
    vec4 originalColor = texture(u_InputTex, tex_coord);
    float red   = texture(u_InputTex, tex_coord + normRed).r;
    float green = texture(u_InputTex, tex_coord + normGreen).g;
    float blue  = texture(u_InputTex, tex_coord + normBlue).b;
    float alpha = originalColor.a;  // Preserve the original alpha channel

    // Output final color with transparency
    FragColor = vec4(red, green, blue, alpha);
}
