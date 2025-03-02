#version 460 core

layout (location = 0) in vec2 aPosTex;

uniform mat4 u_model;
uniform mat4 u_projection;
uniform float u_letterSpacing;

// This uniform holds the starting offset (x, y) for the current line (computed on the CPU).
uniform vec2 u_lineOffset;

uniform int u_text[256];
uniform int u_textLength;

struct Glyph {
    float advance;
    vec2 size;
    vec2 bearing;
    vec2 uvOffset;
    vec2 uvSize;
};
uniform Glyph u_glyphs[128];

out vec2 TexCoord;

void main() {
    int idx = gl_InstanceID;
    if (idx >= u_textLength) {
        gl_Position = vec4(0.0);
        TexCoord = vec2(0.0);
        return;
    }
    
    // Do not render newline characters.
    if (u_text[idx] == int('\n')) {
        gl_Position = vec4(0.0);
        TexCoord = vec2(0.0);
        return;
    }
    
    // Compute the horizontal position for this glyph by accumulating advances
    float xPos = 0.0;
    for (int i = 0; i < idx; i++) {
        int code = u_text[i];
        if (code == int('\n'))
            break;
        xPos += u_glyphs[code].advance + u_letterSpacing;
    }
    
    int charCode = u_text[idx];
    Glyph glyph = u_glyphs[charCode];
    
    // Apply the CPU-computed line offset.
    float glyphX = xPos + glyph.bearing.x + u_lineOffset.x;
    float glyphY = u_lineOffset.y + (glyph.bearing.y - glyph.size.y);
    
    vec2 vertexPos = vec2(glyphX, glyphY) + aPosTex * glyph.size;
    gl_Position = u_projection * u_model * vec4(vertexPos, 0.0, 1.0);
    
    // Compute texture coordinate with vertical flip if needed.
    TexCoord = glyph.uvOffset + vec2(aPosTex.x, 1.0 - aPosTex.y) * glyph.uvSize;
}