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
        // Process local post-processing (per-object effects)
        static void ProcessLocalPostProcessing();

        // Process global post-processing (applied to the full scene)
        static void ProcessGlobalPostProcessing();

        static Renderer2D_GlobalPPSettings m_globalsettings;
        static int postProcessZIndex;

        // Framebuffers for intermediate rendering
        // static std::shared_ptr<FrameBuffer> m_LocalFramebuffer;
        // static std::shared_ptr<FrameBuffer> m_GlobalFramebuffer;
    };

} // namespace Game
