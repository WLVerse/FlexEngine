#include "PostProcessing.h"

namespace Game 
{
    Renderer2D_GlobalPPSettings PostProcessing::m_globalsettings = Renderer2D_GlobalPPSettings();
    int PostProcessing::postProcessZIndex = INT_MAX;

    void PostProcessing::Init()
    {
        // Initialize framebuffers for local and global post-processing.
        // These framebuffers should be set up with the desired resolution and attachments.
        // The below part should be handled by renderer if framebuffermanager is under it
        Vector2 window_size = Vector2(static_cast<float>(Application::GetCurrentWindow()->GetWidth()), static_cast<float>(Application::GetCurrentWindow()->GetHeight()));
        Window::FrameBufferManager.AddFrameBuffer("Local Post Processing", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Global Post Processing", window_size);
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

        #pragma region Clearing Framebuffers
        Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Final Post Processing");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Bloom");
        OpenGLRenderer::ClearFrameBuffer();
        #pragma endregion

        #pragma region Update Settings
        // Finalizing Post Processing
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker>())
        {
            if (!element.GetComponent<Transform>()->is_active)
                continue;

            // Retrieve the marker that holds the global toggles.
            const auto PPComponent = element.GetComponent<PostProcessingMarker>();

            // Create and fill out the global post-processing settings.
            Renderer2D_GlobalPPSettings settings;

            // Global toggles from the marker.
            settings.enableGaussianBlur = PPComponent->enableGaussianBlur;
            settings.enableChromaticAberration = PPComponent->enableChromaticAberration;
            settings.enableBloom = PPComponent->enableBloom;
            settings.enableVignette = PPComponent->enableVignette;
            settings.enableColorGrading = PPComponent->enableColorGrading;
            settings.enableFilmGrain = PPComponent->enableFilmGrain;
            settings.enablePixelate = PPComponent->enablePixelate;
            settings.globalIntensity = PPComponent->globalIntensity;

            // Override global default with effect component settings if present.
            if (element.HasComponent<PostProcessingGaussianBlur>())
            {
                auto blur = element.GetComponent<PostProcessingGaussianBlur>();
                settings.blurKernelSize = blur->kernelSize;
                settings.blurSigma = blur->sigma;
                settings.blurPasses = blur->blurPasses;
            }

            if (element.HasComponent<PostProcessingChromaticAbberation>())
            {
                auto chroma = element.GetComponent<PostProcessingChromaticAbberation>();
                settings.chromaIntensity = chroma->intensity;
                settings.chromaMaxOffset = chroma->maxOffset;
                settings.chromaRedOffset = chroma->redOffset;
                settings.chromaGreenOffset = chroma->greenOffset;
                settings.chromaBlueOffset = chroma->blueOffset;
            }

            if (element.HasComponent<PostProcessingBloom>())
            {
                auto bloom = element.GetComponent<PostProcessingBloom>();
                settings.bloomThreshold = bloom->threshold;
                settings.bloomIntensity = bloom->intensity;
                settings.bloomRadius = bloom->radius;
            }

            if (element.HasComponent<PostProcessingVignette>())
            {
                auto vignette = element.GetComponent<PostProcessingVignette>();
                settings.vignetteIntensity = vignette->intensity;
                settings.vignetteRadius = vignette->radius;
                settings.vignetteSoftness = vignette->softness;
            }

            if (element.HasComponent<PostProcessingColorGrading>())
            {
                auto colorGrade = element.GetComponent<PostProcessingColorGrading>();
                settings.colorBrightness = colorGrade->brightness;
                settings.colorContrast = colorGrade->contrast;
                settings.colorSaturation = colorGrade->saturation;
                //settings.lutTexturePath = colorGrade->lutTexturePath;
            }

            if (element.HasComponent<PostProcessingPixelate>())
            {
                auto pixelate = element.GetComponent<PostProcessingPixelate>();
                settings.pixelWidth = pixelate->pixelWidth;
                settings.pixelHeight = pixelate->pixelHeight;
            }

            if (element.HasComponent<PostProcessingFilmGrain>())
            {
                auto filmGrain = element.GetComponent<PostProcessingFilmGrain>();
                settings.filmGrainIntensity = filmGrain->grainIntensity;
                settings.filmGrainSize = filmGrain->grainSize;
                settings.filmGrainAnimate = filmGrain->animateGrain;
            }

            // Retrieve z-index if available.
            if (element.HasComponent<ZIndex>())
                postProcessZIndex = element.GetComponent<ZIndex>()->z;
        }
        #pragma endregion
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
        Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
        
        // Apply global post-processing effects such as bloom, tone mapping, or color grading.
        auto scene = FlexECS::Scene::GetActiveScene();

        FunctionQueue prePostProcessingQueue;

        // Render all sprite objects with a z-index lower than the post-process marker's.
        for (auto& element : scene->CachedQuery<Transform, Sprite, Position, Rotation, Scale>())
        {
            if (!element.GetComponent<Transform>()->is_active)
                continue;

            int entityZIndex = element.HasComponent<ZIndex>() ? element.GetComponent<ZIndex>()->z : 0;

            if (entityZIndex < postProcessZIndex)
            {
                Sprite& sprite = *element.GetComponent<Sprite>();
                Renderer2DProps props;

                // Check if the sprite has an animator component.
                if (element.HasComponent<Animator>() && FLX_STRING_GET(element.GetComponent<Animator>()->spritesheet_handle) != "")
                {
                    Animator& animator = *element.GetComponent<Animator>();
                    props.asset = FLX_STRING_GET(animator.spritesheet_handle);
                    props.texture_index = animator.current_frame;
                    props.alpha = 1.0f;
                }
                else
                {
                    props.asset = FLX_STRING_GET(sprite.sprite_handle);
                    props.texture_index = -1;
                    props.alpha = sprite.opacity;
                }

                props.window_size = Vector2(CameraManager::GetMainGameCamera()->GetOrthoWidth(),
                                             CameraManager::GetMainGameCamera()->GetOrthoHeight());
                props.alignment = Renderer2DProps::Alignment_TopLeft;
                props.world_transform = element.GetComponent<Transform>()->transform;

                prePostProcessingQueue.Insert({ [props]() {
                    OpenGLRenderer::DrawTexture2D(*CameraManager::GetMainGameCamera(), props);
                }, "", entityZIndex });
            }
        }

        // Render all text objects with a z-index lower than the post-process marker's.
        for (auto& element : scene->CachedQuery<Transform, Text>())
        {
            if (!element.GetComponent<Transform>()->is_active)
                continue;

            int entityZIndex = element.HasComponent<ZIndex>() ? element.GetComponent<ZIndex>()->z : 0;

            if (entityZIndex < postProcessZIndex)
            {
                const auto textComponent = element.GetComponent<Text>();
                Renderer2DText textProps;
                textProps.m_window_size = Vector2(
                    static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetWidth()),
                    static_cast<float>(FlexEngine::Application::GetCurrentWindow()->GetHeight())
                );
                textProps.m_words = FLX_STRING_GET(textComponent->text);
                textProps.m_color = textComponent->color;
                textProps.m_fonttype = FLX_STRING_GET(textComponent->fonttype);

                // Construct a basic transform matrix using the Scale and Position components.
                textProps.m_transform = Matrix4x4(
                    element.GetComponent<Scale>()->scale.x, 0.00, 0.00, 0.00,
                    0.00, element.GetComponent<Scale>()->scale.y, 0.00, 0.00,
                    0.00, 0.00, element.GetComponent<Scale>()->scale.z, 0.00,
                    element.GetComponent<Position>()->position.x,
                    element.GetComponent<Position>()->position.y,
                    element.GetComponent<Position>()->position.z, 1.00
                );
                textProps.m_alignment = std::pair{
                    static_cast<Renderer2DText::AlignmentX>(textComponent->alignment.first),
                    static_cast<Renderer2DText::AlignmentY>(textComponent->alignment.second)
                };
                textProps.m_textboxDimensions = textComponent->textboxDimensions;
                textProps.m_linespacing = 12.0f;

                prePostProcessingQueue.Insert({ [textProps]() {
                    OpenGLRenderer::DrawTexture2D(*CameraManager::GetMainGameCamera(), textProps);
                }, "", entityZIndex });
            }
        }

        // Render all objects (sprites and text) that are below the post-process marker.
        prePostProcessingQueue.Flush();

        // Call OpenGLRenderer to start process
        //OpenGLRenderer::DrawPostProcessing();
    }
    
    Renderer2D_GlobalPPSettings PostProcessing::GetGlobalSettings()
    {
        return m_globalsettings;
    }

    int PostProcessing::GetPostProcessZIndex()
    {
        return postProcessZIndex;
    }

} // namespace Game
