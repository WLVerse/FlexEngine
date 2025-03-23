#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game {

    // A dedicated layer to handle both local and global post-processing
    class PostProcessingLayer : public FlexEngine::Layer 
    {
    public:
        PostProcessingLayer();
        virtual ~PostProcessingLayer() = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;
        virtual void Update() override;

    private:
        // Process local post-processing (per-object effects)
        void ProcessLocalPostProcessing();

        // Process global post-processing (applied to the full scene)
        void ProcessGlobalPostProcessing();

        // Framebuffers for intermediate rendering
        //std::shared_ptr<FrameBuffer> m_LocalFramebuffer;
        //std::shared_ptr<FrameBuffer> m_GlobalFramebuffer;
    };

} // namespace Game
