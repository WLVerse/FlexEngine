#version 330 core
out vec4 FragColor;

in vec2 tex_coord;

uniform sampler2D scene;
uniform float u_Threshold; // Brightness threshold

void main()
{
    vec3 color = texture(scene, tex_coord).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722)); // Perceived brightness
    if (brightness > u_Threshold)
    {
        FragColor = vec4(1.0,1.0,1.0,1.0);
    }
    else
    {
        FragColor = vec4(0.0,0.0,0.0,1.0); // Discard non-bright areas
    }
}
