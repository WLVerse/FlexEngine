#pragma once
/*!************************************************************************
* WLVERSE [https://wlverse.web.app]
* openglfont.h
*
* This file implements the Font class, responsible for font rendering using
* FreeType and OpenGL. It loads font files, generates glyph textures,
* manages font settings (size, hinting, kerning), and provides glyph data
* for rendering.
*
* Key functionalities include:
* - Font file loading and unloading via FreeType library.
* - Dynamic font size adjustment and glyph reloading.
* - Per-glyph OpenGL texture management for efficient rendering.
*
* AUTHORS
* [100%] Soh Wei Jie (weijie.soh@digipen.edu)
*   - Main Author
*   - Developed font handling and texture generation with OpenGL.
*
* Copyright (c) 2024 DigiPen, All rights reserved.
**************************************************************************/
#include "flx_api.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Wrapper/file.h"

#include "FlexMath/matrix4x4.h"
#include <glad/glad.h>
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
            GLuint textureID = 0;   // Texture ID
            Vector2 size;    // Size of glyph
            Vector2 bearing; // Offset from baseline to left/top of glyph
            GLuint advance = 0;      // Horizontal offset to advance to next glyph
        };

        /*!************************************************************************
        * \class Font
        * \brief
        * Manages font loading, glyph generation, and rendering properties.
        *************************************************************************/
        class __FLX_API Font
        {
            // Static FreeType library and face for all Font instances
            static FT_Library s_library; /*!< FreeType library instance */
            static GLuint s_facesCount;  /*!< Count of active font faces */
            FT_Face s_face;              /*!< FreeType face object */
            std::map<char, Glyph> m_glyphs; /*!< Stores glyphs for ASCII characters */

            // Font properties
            int m_fontSize = 50;          // Default font size
            bool m_hintingEnabled = true; // Hinting flag
            bool m_kerningEnabled = true; // Kerning flag

        public:
            // Delete default constructor
            Font() = delete;

            /*!************************************************************************
            * \brief
            * Constructs a Font instance and loads font data.
            * \param key Font file key within assets directory.
            *************************************************************************/
            explicit Font(std::string const& key);

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

            /*!************************************************************************
            * \brief
            * Loads all ASCII glyphs for rendering.
            *************************************************************************/
            void LoadGlyphs();
            #pragma endregion

            #pragma region Set Functions

            /*!************************************************************************
            * \brief
            * Sets font size and reloads glyphs.
            * \param size New font size to set.
            *************************************************************************/
            void SetFontSize(int size);

            /*!************************************************************************
            * \brief
            * Enables or disables hinting.
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
            int GetFontSize() const { return m_fontSize; }

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
            #pragma endregion
        };

    }
}
