#version 460 core
// This vertex shader uses instanced rendering so that each instance corresponds
// to one character from the provided text array.

// Base quad vertex (in normalized [0,1] space)
layout (location = 0) in vec2 aPosTex;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform float u_letterSpacing;
uniform float u_lineSpacing;
uniform float u_textboxWidth;
uniform float u_textboxHeight;
uniform float u_alignX; // 0.0 = left, 0.5 = center, 1.0 = right
uniform float u_alignY; // 0.0 = top, 0.5 = middle, 1.0 = bottom

// The complete text as an array of ASCII codes.
uniform int u_text[256];
uniform int u_textLength;

// Glyph metrics for ASCII characters.
struct Glyph {
    float advance;
    vec2 size;
    vec2 bearing;
    vec2 uvOffset;
    vec2 uvSize;
};
uniform Glyph u_glyphs[128];

out vec2 TexCoord;

void main()
{
    // Use the built-in instance ID to determine which character to process.
    int idx = gl_InstanceID;
    if(idx >= u_textLength) {
        gl_Position = vec4(0.0);
        TexCoord = vec2(0.0);
        return;
    }
    
    // --- Compute the base offset for this character by looping over all previous characters ---
    float xOffset = 0.0;
    float yOffset = 0.0;
    int lineStart = 0;
    for (int i = 0; i < idx; i++)
    {
        int code = u_text[i];
        if (code == int('\n')) {
            // Newline: reset horizontal offset and move down vertically.
            xOffset = 0.0;
            yOffset -= (u_glyphs[int('A')].size.y + u_lineSpacing);
            lineStart = i + 1;
        }
        else
        {
            // Advance by the glyph’s advance and letter spacing.
            xOffset += (u_glyphs[code].advance + u_letterSpacing);
        }
    }
    
    // --- Compute current line width for horizontal alignment ---
    float lineWidth = 0.0;
    for (int i = lineStart; i < u_textLength; i++)
    {
        int code = u_text[i];
        if (code == int('\n'))
            break;
        lineWidth += (u_glyphs[code].advance + u_letterSpacing);
    }
    // Alignment: shift the current line’s x-offset based on u_alignX.
    float alignOffsetX = 0.0;
    if(u_alignX == 0.5)       // center
        alignOffsetX = -lineWidth / 2.0;
    else if(u_alignX == 1.0)  // right
        alignOffsetX = -lineWidth;
    
    // (For vertical alignment you could similarly compute total text height.)
    float alignOffsetY = 0.0;
    
    // --- Get the glyph for the current character ---
    int charCode = u_text[idx];
    // If the character is a newline, output a degenerate vertex.
    if(charCode == int('\n')) {
        gl_Position = vec4(0.0);
        TexCoord = vec2(0.0);
        return;
    }
    Glyph glyph = u_glyphs[charCode];
    
    // --- Compute the final quad position for the character ---
    // The base position for the glyph quad: add the glyph’s bearing.
    float glyphX = xOffset + glyph.bearing.x + alignOffsetX;
    float glyphY = yOffset + (glyph.bearing.y - glyph.size.y) + alignOffsetY;
    // Scale the unit quad (aPosTex in [0,1]) to the glyph’s size.
    vec2 vertexPos = vec2(glyphX, glyphY) + aPosTex * glyph.size;
    
    gl_Position = u_projection * u_model * vec4(vertexPos, 0.0, 1.0);
    
    // --- Compute texture coordinates based on the atlas UV data ---
    TexCoord = glyph.uvOffset + vec2(aPosTex.x, 1.0 - aPosTex.y) * glyph.uvSize;
}
