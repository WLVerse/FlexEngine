#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
    // Handles both local and global post-processing
    class PostProcessing
    {
    public:
        PostProcessing() = default;
        ~PostProcessing() = default;

        static void Init();
        static void Exit();
        static void Update();

        static Renderer2D_GlobalPPSettings GetGlobalSettings();
        static int GetPostProcessZIndex();
    private:
        // Process entities for local post-processing (per-object effects)
        static void ProcessLocalPostProcessing();

        // Starts applying local post-processing (applied to individual entity)
        static void DrawLocalPostProcessing(FlexECS::Entity& entity);

        // Starts applying global post-processing (applied to the full scene)
        static void DrawGlobalPostProcessing();

        // Redraws a texture to current framebuffer
        static void ReplicateFrameBufferAttachment(GLuint texture);

        static Renderer2D_GlobalPPSettings m_globalsettings;
        static int postProcessZIndex;
    };

} // namespace Game
