/*!***************************************************************************
* WLVERSE [https://wlverse.web.app]
* \file shared.vert
* \brief
* Vertex shader used in the post-processing pipeline to process vertex
* attributes for the screen quad. Outputs texture coordinates for fragment
* shaders that handle bloom effects and final composition.
*
* Key functionalities:
* - Passes position directly to the OpenGL pipeline for rendering.
* - Outputs texture coordinates to be interpolated for fragment shaders.
*
* Inputs (location):
* - vec3 aPos: Vertex position of the screen quad.
* - vec2 aTexCoords: Texture coordinates for sampling.
*
* Outputs:
* - vec2 tex_coord: Interpolated texture coordinates for use in fragment shaders.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 tex_coord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    tex_coord = aTexCoords;
}
