/*!************************************************************************
 * WLVERSE [https://wlverse.web.app]
 * bloom_bright_extraction.frag
 *
 * This fragment shader is used to apply a brightness threshold filter to 
 * a given texture. It evaluates the brightness of each pixel and selectively
 * retains only those pixels that exceed the specified threshold. The rest 
 * are rendered as black, effectively isolating bright areas in the texture.
 *
 * Key functionalities include:
 * - Computing perceived brightness using a weighted RGB dot product.
 * - Filtering out pixels below the brightness threshold.
 *
 * This shader is commonly utilized in post-processing pipelines, such as 
 * in the extraction of bright regions for effects like bloom or glare.
 *
 * AUTHORS
 * [100%] Soh Wei Jie (weijie.soh@digipen.edu)
 *   - Main Author
 *   - Developed the thresholding logic and integrated brightness computation.
 *
 * Copyright (c) 2024 DigiPen, All rights reserved.
 **************************************************************************/

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
        FragColor = vec4(color,1.0);
    }
    else
    {
        FragColor = vec4(0.0,0.0,0.0,1.0); // Discard non-bright areas
    }
}
