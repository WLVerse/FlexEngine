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
        FT_Library Font::s_library{}; /*!< Global FreeType library instance */
        GLuint Font::s_facesCount = 0; /*!< Tracks number of loaded faces */

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

        /***************************************************************************//**
        * \brief
        * Constructs a Font object and loads a specified font face.
        *
        * \param key Font file key within assets directory.
        *******************************************************************************/
        Font::Font(std::string const& key)
        {
            // Set up FreeType Lib
            SetupLib();

            // Load the font in the font database
            if (FT_New_Face(s_library, Path::current("assets" + key).string().c_str(), 0, &s_face))
            {
                Log::Fatal("Could not load font, check font directory");
            }
            ++s_facesCount;

            // Set pixel sizes and load glyphs (builds both individual textures and the atlas)
            FT_Set_Pixel_Sizes(s_face, 0, m_fontSize);
            LoadGlyphs();
        }

        #pragma region Font Management Functions

        /***************************************************************************//**
        * \brief
        * Unloads the font, freeing resources and textures.
        *******************************************************************************/
        void Font::Unload()
        {
            // Free the FreeType face object
            if (s_face)
            {
                Log::Debug("Unloaded font: " + std::string(s_face->family_name) + " " + std::string(s_face->style_name));
                FT_Done_Face(s_face);
                s_face = nullptr;
                --s_facesCount;
            }

            // Free the FreeType library object if no faces remain
            if (s_facesCount <= 0 && s_library)
            {
                FT_Done_FreeType(s_library);
                s_library = nullptr;
                Log::Info("All fonts unloaded.");
            }

            // Delete all individual glyph textures
            for (auto& pair : m_glyphs)
                glDeleteTextures(1, &pair.second.textureID);
            m_glyphs.clear();

            // Delete the atlas texture if it exists
            if (m_atlasTexture)
            {
                glDeleteTextures(1, &m_atlasTexture);
                m_atlasTexture = 0;
            }
        }

        /***************************************************************************//**
        * \brief
        * Initializes the FreeType library if not already initialized.
        *******************************************************************************/
        void Font::SetupLib()
        {
            if (s_library == nullptr)
                if (FT_Init_FreeType(&s_library))
                    Log::Fatal("Unable to initialize FreeType Library, check dll files.");
        }

        /***************************************************************************//**
        * \brief
        * Loads all ASCII glyphs into individual OpenGL textures and packs them
        * into a single atlas texture for batched rendering.
        *******************************************************************************/
        void Font::LoadGlyphs()
        {
            // Free any previously loaded glyphs and atlas texture
            for (auto& pair : m_glyphs)
            {
                glDeleteTextures(1, &pair.second.textureID);
            }
            m_glyphs.clear();
            if (m_atlasTexture)
            {
                glDeleteTextures(1, &m_atlasTexture);
                m_atlasTexture = 0;
            }

            glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

            // Temporary storage for glyph bitmaps for atlas packing.
            std::map<char, TempGlyph> tempGlyphs;

            // We'll pack ASCII characters 0-127 into an atlas.
            // Choose a fixed atlas width (for example, 512 pixels).
            const int atlasWidth = 512;
            int atlasHeight = 0;
            int rowWidth = 0;
            int rowHeight = 0;

            // Define padding in pixels to add around each glyph.
            const int padding = 1;

            // Load each glyph, create an individual texture, and store temporary bitmap data.
            for (unsigned char c = 0; c < 128; ++c)
            {
                int loadFlags = FT_LOAD_RENDER;
                if (!m_hintingEnabled)
                    loadFlags |= FT_LOAD_NO_HINTING;

                if (FT_Load_Char(s_face, c, loadFlags))
                {
                    Log::Warning("Failed to load glyph for character: " + std::string(1, c));
                    continue;
                }

                FT_GlyphSlot g = s_face->glyph;

                // Create individual texture for this glyph.
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_2D, texture);
                glTexImage2D(
                    GL_TEXTURE_2D,
                    0,
                    GL_RED,
                    g->bitmap.width,
                    g->bitmap.rows,
                    0,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    g->bitmap.buffer
                );
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                // Store the glyph with its individual texture.
                Glyph glyph = {
                    texture,
                    Vector2(static_cast<float>(g->bitmap.width), static_cast<float>(g->bitmap.rows)),
                    Vector2(static_cast<float>(g->bitmap_left), static_cast<float>(g->bitmap_top)),
                    static_cast<GLuint>(g->advance.x >> 6) // Advance in pixels (1/64th factor)
                };
                m_glyphs[c] = glyph;

                // Copy glyph bitmap into temporary storage for atlas packing.
                TempGlyph tg;
                tg.width = g->bitmap.width;
                tg.height = g->bitmap.rows;
                tg.bearingX = g->bitmap_left;
                tg.bearingY = g->bitmap_top;
                tg.advance = static_cast<int>(g->advance.x >> 6);
                tg.buffer.resize(tg.width * tg.height);
                std::memcpy(tg.buffer.data(), g->bitmap.buffer, tg.width * tg.height);
                tempGlyphs[c] = tg;

                // Compute padded dimensions.
                int paddedWidth = tg.width + 2 * padding;
                int paddedHeight = tg.height + 2 * padding;

                // Update atlas packing measurements.
                if (rowWidth + paddedWidth > atlasWidth)
                {
                    atlasHeight += rowHeight;
                    rowWidth = 0;
                    rowHeight = 0;
                }
                rowWidth += paddedWidth;
                rowHeight = std::max(rowHeight, paddedHeight);
            }
            // Add the final row height.
            atlasHeight += rowHeight;

            m_atlasWidth = atlasWidth;
            m_atlasHeight = atlasHeight;

            // Create an atlas buffer and initialize it to zero.
            std::vector<unsigned char> atlasBuffer(atlasWidth * atlasHeight, 0);

            // Pack each glyph's bitmap into the atlas buffer.
            int x = 0, y = 0;
            rowHeight = 0;
            for (unsigned char c = 0; c < 128; ++c)
            {
                if (tempGlyphs.find(c) == tempGlyphs.end())
                    continue;

                TempGlyph& tg = tempGlyphs[c];
                int paddedWidth = tg.width + 2 * padding;
                int paddedHeight = tg.height + 2 * padding;
                if (x + paddedWidth > atlasWidth)
                {
                    y += rowHeight;
                    x = 0;
                    rowHeight = 0;
                }
                // Copy bitmap data for the glyph into the atlas buffer,
                // leaving a padding border around it.
                for (int row = 0; row < tg.height; ++row)
                {
                    for (int col = 0; col < tg.width; ++col)
                    {
                        // Destination offset includes padding.
                        int destX = x + col + padding;
                        int destY = y + row + padding;
                        atlasBuffer[destY * atlasWidth + destX] = tg.buffer[row * tg.width + col];
                    }
                }
                // Compute normalized UV coordinates for this glyph.
                // The glyph's UV region excludes the padding.
                Vector2 uvOffset(static_cast<float>(x + padding) / atlasWidth,
                                 static_cast<float>(y + padding) / atlasHeight);
                Vector2 uvSize(static_cast<float>(tg.width) / atlasWidth,
                               static_cast<float>(tg.height) / atlasHeight);
                m_glyphs[c].uvOffset = uvOffset;
                m_glyphs[c].uvSize = uvSize;

                x += paddedWidth;
                rowHeight = std::max(rowHeight, paddedHeight);
            }

            // Generate the atlas texture.
            glGenTextures(1, &m_atlasTexture);
            glBindTexture(GL_TEXTURE_2D, m_atlasTexture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                atlasWidth,
                atlasHeight,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                atlasBuffer.data()
            );
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        #pragma endregion

        /***************************************************************************//**
        * \brief
        * Sets the font size and reloads glyphs accordingly.
        *
        * \param size The new font size (must be positive).
        *******************************************************************************/
        void Font::SetFontSize(int size)
        {
            if (size <= 0)
            {
                Log::Warning("Font size must be positive.");
                return;
            }
            m_fontSize = size;
            FT_Set_Pixel_Sizes(s_face, 0, m_fontSize);
            LoadGlyphs(); // Reload glyphs at the new size
        }

        /***************************************************************************//**
        * \brief
        * Enables or disables hinting, then reloads glyphs.
        *
        * \param enabled True to enable hinting; false to disable.
        *******************************************************************************/
        void Font::SetHinting(bool enabled)
        {
            m_hintingEnabled = enabled;
            LoadGlyphs(); // Reload glyphs with updated hinting settings
        }

        /***************************************************************************//**
        * \brief
        * Enables or disables kerning.
        *
        * \param enabled True to enable kerning; false to disable.
        *******************************************************************************/
        void Font::SetKerning(bool enabled)
        {
            m_kerningEnabled = enabled;
        }

        #pragma region Getter functions

        /***************************************************************************//**
        * \brief
        * Retrieves the glyph for a given character.
        *
        * \param c Character to retrieve the glyph for.
        * \return Reference to the corresponding glyph, or default if not found.
        *******************************************************************************/
        const Glyph& Font::GetGlyph(char c) const
        {
            static const Glyph defaultGlyph = {};
            auto it = m_glyphs.find(c);
            return it != m_glyphs.end() ? it->second : defaultGlyph;
        }

        #pragma endregion

    }
}
