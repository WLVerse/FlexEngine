#include "PostProcessing.h"

namespace Game 
{
    void PostProcessing::Init()
    {
        // Initialize framebuffers for local and global post-processing.
        // These framebuffers should be set up with the desired resolution and attachments.
        // The below part should be handled by renderer if framebuffermanager is under it
        Vector2 window_size = Vector2(static_cast<float>(Application::GetCurrentWindow()->GetWidth()), static_cast<float>(Application::GetCurrentWindow()->GetHeight()));
        Window::FrameBufferManager.AddFrameBuffer("Final Post Processing", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Gaussian Blur", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Bloom", window_size);
    }

    void PostProcessing::Exit()
    {
        // Release or reset resources.
    }

    void PostProcessing::Update() 
    {
        OpenGLFrameBuffer::Unbind();

        if (!CameraManager::has_main_camera) return;

        // === Step 1: Process Local Post-Processing ===
        // Bind local framebuffer and render objects that have post-processing components.
        //m_LocalFramebuffer->Bind();
        // Clear local framebuffer as needed.
        // Here, you would iterate through all objects with a PostProcessingComponent,
        // then apply their stack of post-processing effects.
        ProcessLocalPostProcessing();
        //m_LocalFramebuffer->Unbind();

        // === Step 2: Render Scene to Global Framebuffer ===
        // At this stage, your regular RenderingLayer might have rendered the scene.
        // You may now composite the local post-processed textures with the main scene.
        // For example, you can render the local processed texture onto m_GlobalFramebuffer.

        // === Step 3: Process Global Post-Processing ===
        // Bind global framebuffer if not already bound and apply global effects.
        ProcessGlobalPostProcessing();

        OpenGLFrameBuffer::Unbind();
    }

    void PostProcessing::ProcessLocalPostProcessing() 
    {
        // Example: For each object with a PostProcessingComponent, do:
        //   1. Retrieve its stackable post-processing effects.
        //   2. For each effect in order:
        //         - Bind the corresponding shader.
        //         - Render a fullscreen quad using the object's local texture.
        //         - Swap between intermediate textures as needed.
        //
        // Pseudocode:
        // for (auto& obj : Scene::GetActiveScene()->QueryPostProcessingObjects()) {
        //     auto& ppComponent = obj.GetComponent<PostProcessingComponent>();
        //     Texture currentTexture = obj.GetRenderTexture();
        //     for (auto& effect : ppComponent.GetEffectsStack()) {
        //         currentTexture = effect.Apply(currentTexture);
        //     }
        //     // Store the final local texture back into the component or use it for composition.
        // }
    }

    void PostProcessing::ProcessGlobalPostProcessing() 
    {
        // Apply global post-processing effects such as bloom, tone mapping, or color grading.
        // Typically, you render a fullscreen quad with a shader chain that takes the global framebuffer's texture.
        //
        // Example pseudocode:
        // Texture sceneTexture = m_GlobalFramebuffer->GetColorAttachment();
        // for (auto& globalEffect : GlobalPostProcessingManager::GetEffects()) {
        //     sceneTexture = globalEffect.Apply(sceneTexture);
        // }
        // Finally, render the processed sceneTexture to the screen.
    }

} // namespace Game
