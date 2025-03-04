#pragma once

#include "flx_api.h"
#include <ft2build.h>
#include FT_FREETYPE_H

#include "FlexMath/matrix4x4.h"

#include <string>

namespace FlexEngine
{

    namespace Asset
    {
        /*!************************************************************************
        * \struct Glyph
        * \brief
        * Represents a single glyph with texture and positioning data.
        *************************************************************************/
        struct __FLX_API Glyph 
        {
            unsigned int textureID = 0;   // Individual glyph texture ID.
            Vector2 size;                 // Size of the glyph in pixels.
            Vector2 bearing;              // Offset from baseline to left/top of glyph.
            unsigned int advance = 0;     // Horizontal offset to advance to next glyph.
            Vector2 uvOffset;             // Bottom-left corner of glyph region in atlas (normalized).
            Vector2 uvSize;               // Size of glyph region in atlas (normalized).
        };


        //Helper struct to pass glyph information to shader
        struct GlyphMetric
        {
            float advance;
            float size[2];
            float bearing[2];
            float uvOffset[2];
            float uvSize[2];
        };

        /*!************************************************************************
        * \class Font
        * \brief
        * Manages font loading, glyph generation, and rendering properties.
        *************************************************************************/
        class __FLX_API Font
        {
            // Helper structure to store glyph data and atlas information for a specific font size.
            struct FontSizeData 
            {
                std::map<char, Glyph> glyphs;
                unsigned int atlasTexture = 0;
                int atlasWidth = 0;
                int atlasHeight = 0;
            };

            // Static FreeType library and active face count.
            static FT_Library s_library;
            static unsigned int s_facesCount;

            // FreeType face for this font.
            FT_Face s_face;

            // Caches of font data keyed by font size.
            std::map<int, FontSizeData> m_sizeData;

            // Currently active font size.
            int m_currentFontSize = 50;

            // Font settings.
            bool m_hintingEnabled = true;
            bool m_kerningEnabled = true;

            // Font asset key.
            std::string m_key;

            // Loads glyphs for a given font size and caches the results.
            void LoadGlyphsForSize(int size);
        public:
            #pragma region Constructors
            // Delete default constructor.
            Font() = delete;

            /*!************************************************************************
             * \brief Constructs a Font instance and loads font data.
             * \param key Font file key within the assets directory.
             *************************************************************************/
            explicit Font(const std::string& key);

            #pragma endregion

            #pragma region Font Management Functions
            
            /*!************************************************************************
            * \brief
            * Frees all faces and library resources.
            *************************************************************************/
            void Unload();

            /*!************************************************************************
            * \brief
            * Initializes the FreeType library if not already set up.
            *************************************************************************/
            void SetupLib();

            #pragma endregion

            #pragma region Set Functions

            /*!************************************************************************
            * \brief
            * Sets the current font size. Caches the font data for each size.
            * \param size New font size (must be positive).
            *************************************************************************/
            void SetFontSize(int size);

            /*!************************************************************************
            * \brief
            * Enables or disables hinting and regenerates glyphs for the current size.
            * \param enabled True to enable hinting; false to disable.
            *************************************************************************/
            void SetHinting(bool enabled);

            /*!************************************************************************
            * \brief
            * Enables or disables kerning.
            * \param enabled True to enable kerning; false to disable.
            *************************************************************************/
            void SetKerning(bool enabled);
            #pragma endregion

            #pragma region Get Functions

            /*!************************************************************************
            * \brief
            * Retrieves a glyph for a specific character.
            * \param c Character to retrieve glyph for.
            * \return Reference to the glyph object.
            *************************************************************************/
            Glyph const& GetGlyph(char c) const;

            /*!************************************************************************
            * \brief
            * Gets the current font size.
            * \return Current font size.
            *************************************************************************/
            int GetFontSize() const { return m_currentFontSize; }

            /*!************************************************************************
            * \brief
            * Checks if hinting is enabled.
            * \return True if hinting is enabled, false otherwise.
            *************************************************************************/
            bool IsHintingEnabled() const { return m_hintingEnabled; }

            /*!************************************************************************
            * \brief
            * Checks if kerning is enabled.
            * \return True if kerning is enabled, false otherwise.
            *************************************************************************/
            bool IsKerningEnabled() const { return m_kerningEnabled; }

            /*!************************************************************************
            * \brief 
            * Retrieves the atlas texture for the current font size.
            * \return OpenGL texture ID for the atlas.
            *************************************************************************/
            unsigned int GetAtlasTexture() const;
            #pragma endregion

        };

    }
}
