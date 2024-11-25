#pragma once
#include <vector>
#include "FlexMath/Vector4.h"
#include <glad/glad.h>
namespace FlexEngine 
{
    #pragma region old
    //class OpenGLFrameBuffer 
    //{
    //public:
    //    static GLuint m_editorFBO;         //framebuffer to handle editor cam
    //    static GLuint m_gameFBO;           //framebuffer to handle in-game cam
    //    static GLuint m_postProcessingFBO; //framebuffer to handle post-processing
    //    static GLuint m_bloomFBO;          //framebuffer to handle bloom exclusively
    //    
    //    static GLuint m_pingpongTex[2];
    //    static GLuint m_postProcessingTex;
    //    static GLuint m_editorTex;
    //    static GLuint m_gameRenderTex;
    //    static GLuint m_finalRenderTex;

    //    enum __FLX_API CreatedTextureID
    //    {
    //        CID_editor,
    //        CID_gameRender,
    //        CID_finalRender,
    //        CID_brightnessPass,
    //        CID_blur,
    //    };

    //    static void Init(const Vector2& windowSize);
    //    /*!***************************************************************************
    //    * \brief
    //    * Sets the editor framebuffer for rendering.
    //    *****************************************************************************/
    //    static void SetEditorFrameBuffer();
    //    /*!***************************************************************************
    //    * \brief
    //    * Sets the game framebuffer for rendering.
    //    *****************************************************************************/
    //    static void SetGameFrameBuffer();
    //    /*!***************************************************************************
    //    * \brief
    //    * Sets the default framebuffer for rendering.
    //    *****************************************************************************/
    //    static void SetDefaultFrameBuffer();
    //    /*!***************************************************************************
    //    * \brief
    //    * Enables post-processing effects for rendering.
    //    *****************************************************************************/
    //    static void SetPostProcessingFrameBuffer();
    //    /*!***************************************************************************
    //    * \brief
    //    * Sets the framebuffer specifically for bloom post-processing effects.
    //    *****************************************************************************/
    //    static void SetBloomFrameBuffer();
    //    /*!***************************************************************************
    //    * \brief
    //    * Clears the current framebuffer.
    //    *****************************************************************************/
    //    static void ClearFrameBuffer();
    //    static GLuint GetCreatedTexture(GLuint textureID);
    //    /*!***************************************************************************
    //    * \brief Retrieves the ID of the currently bound framebuffer.
    //    * \return The ID of the currently bound framebuffer.
    //    *****************************************************************************/
    //    static GLuint GetCurrFrameBuffer();
    //    /*!**************************************************************************
    //    * \brief Checks if the currently bound framebuffer matches the given framebuffer ID.
    //    * \param framebufferID The ID of the framebuffer to compare against.
    //    * \return True if the currently bound framebuffer matches the given ID, false otherwise.
    //    ***************************************************************************/
    //    static bool CheckSameFrameBuffer(const GLuint framebufferID);
    //};
    #pragma endregion

    #pragma region new
    class OpenGLFrameBuffer
    {
    public:
        struct FBOData
        {
            GLuint fboID;
            std::vector<GLuint> textures; // Attached textures
        };

        static void Init(const Vector2& windowSize);
        static void CreateFramebuffer(const std::string& name, int numTextures, const Vector2& windowSize);
        static void BindFramebuffer(const std::string& name);
        static void UnbindFramebuffer();
        static GLuint GetFramebufferTexture(const std::string& name, int textureIndex);
        static void DeleteFramebuffer(const std::string& name);
        static void ClearFrameBuffer();

        /*!***************************************************************************
        * \brief
        * Regenerates all framebuffer textures for the given window size.
        *****************************************************************************/
        static void RegenerateTextures(const Vector2& newSize);

    private:
        static std::unordered_map<std::string, FBOData> m_framebuffers;
    };
    #pragma endregion
}