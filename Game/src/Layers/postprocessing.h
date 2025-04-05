// WLVERSE [https://wlverse.web.app]
// postprocessing.h
//
// Declares the PostProcessing class which manages both local and global
// post-processing effects within the editor, including initialization,
// per-frame updates, and rendering pipelines.
//
// [100%] Soh Wei Jie (weijie.soh\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2025 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game
{
    // Handles both local and global post-processing
    class PostProcessing
    {
    public:
        // Function: PostProcessing::Init
        // Description: Initialize shaders, framebuffers, and settings for
        //              the post-processing pipeline.
        static void Init();

        // Function: PostProcessing::Exit
        // Description: Release post-processing resources and clean up state.
        static void Exit();

        // Function: PostProcessing::Update
        // Description: Execute per-frame post-processing steps:
        //              process local effects and apply global effects.
        static void Update();

        // Function: PostProcessing::GetGlobalSettings
        // Description: Retrieve the current global post-processing settings.
        static Renderer2D_GlobalPPSettings GetGlobalSettings();

        // Function: PostProcessing::GetPostProcessZIndex
        // Description: Get the Z-index threshold at which post-processing
        //              rendering occurs.
        static int GetPostProcessZIndex();

    private:
        // Function: PostProcessing::ProcessLocalPostProcessing
        // Description: Iterate over entities marked for local post-processing
        //              and prepare them for individual effects.
        static void ProcessLocalPostProcessing();

        // Function: PostProcessing::DrawLocalPostProcessing
        // Description: Apply local post-processing effect for a single entity.
        static void DrawLocalPostProcessing(FlexECS::Entity& entity);

        // Function: PostProcessing::DrawGlobalPostProcessing
        // Description: Render the full scene with global post-processing shaders.
        static void DrawGlobalPostProcessing();

        // Function: PostProcessing::ReplicateFrameBufferAttachment
        // Description: Copy the given texture into the current framebuffer
        //              for subsequent post-processing passes.
        static void ReplicateFrameBufferAttachment(GLuint texture);

        static Renderer2D_GlobalPPSettings m_globalsettings;
        static int postProcessZIndex;
    };

} // namespace Game
