#include <glad/glad.h>
#include "Utilities/file.h"
#include "openglfont.h"
#include <vector>
#include <algorithm>
#include <cstring>
#include <map>

namespace FlexEngine
{
    namespace Asset
    {
        // Initialize static members.
        FT_Library Font::s_library{};
        GLuint Font::s_facesCount = 0;

        // Temporary structure to hold glyph bitmap data for atlas packing.
        struct TempGlyph 
        {
            std::vector<unsigned char> buffer;
            int width;
            int height;
            int bearingX;
            int bearingY;
            int advance;
        };

        Font::Font(const std::string& key)
            : m_key(key), m_currentFontSize(50)
        {
            SetupLib();

            // Load the font face. Path::current builds the asset path.
            std::string fontPath = Path::current("assets" + key).string();
            if (FT_New_Face(s_library, fontPath.c_str(), 0, &s_face)) 
            {
                Log::Fatal("Could not load font: " + fontPath);
            }
            ++s_facesCount;

            // Load and cache glyphs for the default font size.
            LoadGlyphsForSize(m_currentFontSize);
        }

        void Font::SetupLib() 
        {
            if (s_library == nullptr) 
            {
                if (FT_Init_FreeType(&s_library)) 
                {
                    Log::Fatal("Unable to initialize FreeType Library, check dll files.");
                }
            }
        }

        void Font::LoadGlyphsForSize(int size) 
        {
            // Primary Check: If data for this size exists, free its resources first.
            auto it = m_sizeData.find(size);
            if (it != m_sizeData.end()) 
            {
                FontSizeData& oldData = it->second;
                for (auto& glyphPair : oldData.glyphs) 
                {
                    glDeleteTextures(1, &glyphPair.second.textureID);
                }
                if (oldData.atlasTexture) 
                {
                    glDeleteTextures(1, &oldData.atlasTexture);
                }
                m_sizeData.erase(it);
            }

            FontSizeData data;

            // Set the desired pixel size.
            FT_Set_Pixel_Sizes(s_face, 0, size);

            // Ensure proper unpack alignment.
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            std::map<char, TempGlyph> tempGlyphs;
            const int atlasWidth = 512;
            int atlasHeight = 0, rowWidth = 0, rowHeight = 0;
            const int padding = 1;

            // Load ASCII glyphs (0-127).
            for (unsigned char c = 0; c < 128; ++c) {
                int loadFlags = FT_LOAD_RENDER;
                if (!m_hintingEnabled)
                    loadFlags |= FT_LOAD_NO_HINTING;

                if (FT_Load_Char(s_face, c, loadFlags)) {
                    Log::Warning("Failed to load glyph for character: " + std::string(1, c));
                    continue;
                }
                FT_GlyphSlot g = s_face->glyph;

                // Create individual texture for the glyph.
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                             g->bitmap.width, g->bitmap.rows,
                             0, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // Store glyph information.
                Glyph glyph;
                glyph.textureID = texture;
                glyph.size = Vector2(static_cast<float>(g->bitmap.width),
                                     static_cast<float>(g->bitmap.rows));
                glyph.bearing = Vector2(static_cast<float>(g->bitmap_left),
                                        static_cast<float>(g->bitmap_top));
                glyph.advance = static_cast<unsigned int>(g->advance.x >> 6);
                data.glyphs[c] = glyph;

                // Store temporary glyph data for atlas packing.
                TempGlyph tg;
                tg.width = g->bitmap.width;
                tg.height = g->bitmap.rows;
                tg.bearingX = g->bitmap_left;
                tg.bearingY = g->bitmap_top;
                tg.advance = static_cast<int>(g->advance.x >> 6);
                tg.buffer.resize(tg.width * tg.height);
                if (tg.width > 0 && tg.height > 0) {
                    std::memcpy(tg.buffer.data(), g->bitmap.buffer, tg.width * tg.height);
                }
                tempGlyphs[c] = tg;

                int paddedWidth = tg.width + 2 * padding;
                int paddedHeight = tg.height + 2 * padding;
                if (rowWidth + paddedWidth > atlasWidth) {
                    atlasHeight += rowHeight;
                    rowWidth = 0;
                    rowHeight = 0;
                }
                rowWidth += paddedWidth;
                rowHeight = std::max(rowHeight, paddedHeight);
            }
            atlasHeight += rowHeight;
            data.atlasWidth = atlasWidth;
            data.atlasHeight = atlasHeight;

            // Create an atlas buffer.
            std::vector<unsigned char> atlasBuffer(atlasWidth * atlasHeight, 0);
            int x = 0, y = 0;
            rowHeight = 0;
            for (unsigned char c = 0; c < 128; ++c) {
                if (tempGlyphs.find(c) == tempGlyphs.end())
                    continue;
                TempGlyph& tg = tempGlyphs[c];
                int paddedWidth = tg.width + 2 * padding;
                int paddedHeight = tg.height + 2 * padding;
                if (x + paddedWidth > atlasWidth) {
                    y += rowHeight;
                    x = 0;
                    rowHeight = 0;
                }
                // Copy glyph bitmap into the atlas buffer (with padding).
                for (int row = 0; row < tg.height; ++row) {
                    for (int col = 0; col < tg.width; ++col) {
                        int destX = x + col + padding;
                        int destY = y + row + padding;
                        atlasBuffer[destY * atlasWidth + destX] = tg.buffer[row * tg.width + col];
                    }
                }
                // Compute normalized UV coordinates (note the corrected division by atlasHeight).
                Vector2 uvOffset(static_cast<float>(x + padding) / atlasWidth,
                                 static_cast<float>(y + padding) / atlasHeight);
                Vector2 uvSize(static_cast<float>(tg.width) / atlasWidth,
                               static_cast<float>(tg.height) / atlasHeight);
                data.glyphs[c].uvOffset = uvOffset;
                data.glyphs[c].uvSize = uvSize;

                x += paddedWidth;
                rowHeight = std::max(rowHeight, paddedHeight);
            }

            // Generate the atlas texture.
            glGenTextures(1, &data.atlasTexture);
            glBindTexture(GL_TEXTURE_2D, data.atlasTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                         atlasWidth, atlasHeight,
                         0, GL_RED, GL_UNSIGNED_BYTE, atlasBuffer.data());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);

            m_sizeData[size] = data;
        }

        void Font::SetFontSize(int size) 
        {
            if (size <= 0) 
            {
                Log::Warning("Font size must be positive.");
                return;
            }
            if (m_currentFontSize == size)
                return;

            // Generate glyphs for this size if not already cached.
            if (m_sizeData.find(size) == m_sizeData.end())
                LoadGlyphsForSize(size);

            m_currentFontSize = size;
        }

        void Font::SetHinting(bool enabled) 
        {
            m_hintingEnabled = enabled;
            // Regenerate glyphs for the current size with the updated hinting.
            LoadGlyphsForSize(m_currentFontSize);
        }

        void Font::SetKerning(bool enabled) 
        {
            m_kerningEnabled = enabled;
            // Kerning is typically applied during text layout rather than during glyph generation.
        }

        const Glyph& Font::GetGlyph(char c) const 
        {
            auto it = m_sizeData.find(m_currentFontSize);
            if (it != m_sizeData.end()) 
            {
                auto glyphIt = it->second.glyphs.find(c);
                if (glyphIt != it->second.glyphs.end())
                    return glyphIt->second;
            }
            static const Glyph defaultGlyph = {};
            return defaultGlyph;
        }

        unsigned int Font::GetAtlasTexture() const 
        {
            auto it = m_sizeData.find(m_currentFontSize);
            return (it != m_sizeData.end()) ? it->second.atlasTexture : 0;
        }

        void Font::Unload() 
        {
            // Free all cached font size data.
            for (auto& pair : m_sizeData) 
            {
                FontSizeData& data = pair.second;
                for (auto& glyphPair : data.glyphs) 
                {
                    glDeleteTextures(1, &glyphPair.second.textureID);
                }
                data.glyphs.clear();

                if (data.atlasTexture) 
                {
                    glDeleteTextures(1, &data.atlasTexture);
                    data.atlasTexture = 0;
                }
            }
            m_sizeData.clear();

            // Free the FreeType face.
            if (s_face) 
            {
                Log::Debug("Unloaded font: " + std::string(s_face->family_name) + " " +
                           std::string(s_face->style_name));
                FT_Done_Face(s_face);
                s_face = nullptr;
                --s_facesCount;
            }

            // Free the FreeType library if no faces remain.
            if (s_facesCount <= 0 && s_library) 
            {
                FT_Done_FreeType(s_library);
                s_library = nullptr;
                Log::Info("All fonts unloaded.");
            }
        }

    }
}
