#version 330 core

in vec2 vTexCoord;
out vec4 FragColor;

uniform sampler2D u_InputTex;    // The input texture
uniform float u_Brightness;      // Additive brightness offset
uniform float u_Contrast;        // Contrast multiplier (e.g. 1.0 means no change)
uniform float u_Saturation;      // Saturation multiplier (1.0 = unchanged, 0 = grayscale)

void main()
{
    // Sample the base color.
    vec3 color = texture(u_InputTex, vTexCoord).rgb;

    // Adjust brightness (add offset).
    color += u_Brightness;

    // Adjust contrast: scale around 0.5.
    color = (color - 0.5) * u_Contrast + 0.5;

    // Convert color to grayscale (luminance) using standard coefficients.
    float luminance = dot(color, vec3(0.2126, 0.7152, 0.0722));
    // Interpolate between grayscale and the original color based on saturation.
    color = mix(vec3(luminance), color, u_Saturation);

    FragColor = vec4(color, 1.0);
}
