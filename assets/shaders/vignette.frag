#version 330 core

in vec2 tex_coord;
out vec4 FragColor;

uniform sampler2D u_InputTex;       // Input texture (scene)
uniform float u_VignetteIntensity;    // How strong the vignette darkening is (0 = none, 1 = full effect)
uniform vec2 u_VignetteRadius;          // Distance from each edge (normalized, 0 to 0.5) below which no effect is applied.
uniform vec2 u_VignetteSoftness;        // Transition range (normalized) over which the effect ramps up.

void main()
{
    // Sample the base color.
    vec3 color = texture(u_InputTex, tex_coord).rgb;

    // Compute the distance from the fragment to each edge.
    // For texture coordinates in [0,1]:
    //   left edge: tex_coord.x, right edge: 1.0 - tex_coord.x
    //   bottom edge: tex_coord.y, top edge: 1.0 - tex_coord.y
    float distX = min(tex_coord.x, 1.0 - tex_coord.x);
    float distY = min(tex_coord.y, 1.0 - tex_coord.y);

    // Compute horizontal and vertical factors using smoothstep:
    // Within u_VignetteRadius, no effect (smoothstep returns 0)
    // Beyond u_VignetteRadius+u_VignetteSoftness, full effect (smoothstep returns 1).
    float factorX = smoothstep(u_VignetteRadius.x, u_VignetteRadius.x + u_VignetteSoftness.x, distX);
    float factorY = smoothstep(u_VignetteRadius.y, u_VignetteRadius.y + u_VignetteSoftness.y, distY);

    // Invert the factors so that 0 is at the center and 1 at the edge.
    factorX = 1.0 - factorX;
    factorY = 1.0 - factorY;

    // Combine the horizontal and vertical contributions.
    // Using max ensures that if the fragment is close to either edge, full effect is applied.
    float edgeFactor = max(factorX, factorY);

    // Apply the vignette by darkening the color at the edges.
    color *= (1.0 - u_VignetteIntensity * edgeFactor);

    FragColor = vec4(color, 1.0);
}
