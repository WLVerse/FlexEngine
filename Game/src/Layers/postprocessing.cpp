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
        Window::FrameBufferManager.AddFrameBuffer("Gaussian Blur Horizontal", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Gaussian Blur Vertical", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Bloom", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Chromatic Aberration", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Color Grading", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Vignette", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Film Grain", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Pixelate", window_size);
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
        Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Horizontal");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Vertical");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Bloom");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Chromatic Aberration");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Color Grading");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Vignette");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Film Grain");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Pixelate");
        OpenGLRenderer::ClearFrameBuffer();
        #pragma endregion

        #pragma region Update Settings
        // Finalizing Post Processing
        Vector2 windowSize = Vector2((float)FlexEngine::Application::GetCurrentWindow()->GetWidth(), (float)FlexEngine::Application::GetCurrentWindow()->GetHeight());
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker>())
        {
            if (!element.GetComponent<Transform>()->is_active)
                continue;

            auto positionComponent = element.GetComponent<Position>();
            positionComponent->position = Vector3(0, 0, 0);
            auto rotationComponent = element.GetComponent<Rotation>();
            auto scaleComponent = element.GetComponent<Scale>();
            scaleComponent->scale = Vector3(windowSize, 0.0f);

            Matrix4x4 translation_matrix = Matrix4x4::Translate(Matrix4x4::Identity, positionComponent->position);
            Matrix4x4 rotation_matrix = Quaternion::FromEulerAnglesDeg(rotationComponent->rotation).ToRotationMatrix();
            Matrix4x4 scale_matrix = Matrix4x4::Scale(Matrix4x4::Identity, scaleComponent->scale);

            // Update the transform
            element.GetComponent<Transform>()->transform = translation_matrix * rotation_matrix * scale_matrix;

            // Retrieve the marker that holds the global toggles.
            const auto PPComponent = element.GetComponent<PostProcessingMarker>();

            // Global toggles from the marker.
            //m_globalsettings.enableGaussianBlur = PPComponent->enableGaussianBlur;
            //m_globalsettings.enableChromaticAberration = PPComponent->enableChromaticAberration;
            //m_globalsettings.enableBloom = PPComponent->enableBloom;
            //m_globalsettings.enableVignette = PPComponent->enableVignette;
            //m_globalsettings.enableColorGrading = PPComponent->enableColorGrading;
            //m_globalsettings.enableFilmGrain = PPComponent->enableFilmGrain;
            //m_globalsettings.enablePixelate = PPComponent->enablePixelate;
            //m_globalsettings.globalIntensity = PPComponent->globalIntensity;

            // Override global default with effect component settings if present.
            if (element.HasComponent<PostProcessingGaussianBlur>())
            {
                auto blur = element.GetComponent<PostProcessingGaussianBlur>();
                m_globalsettings.blurIntensity = blur->intensity;
                m_globalsettings.blurDistance = blur->distance;
                m_globalsettings.blurPasses = blur->blurPasses;
            }

            if (element.HasComponent<PostProcessingChromaticAbberation>())
            {
                auto chroma = element.GetComponent<PostProcessingChromaticAbberation>();
                m_globalsettings.chromaIntensity = chroma->intensity;
                m_globalsettings.chromaMaxOffset = chroma->maxOffset;
                m_globalsettings.chromaRedOffset = chroma->redOffset;
                m_globalsettings.chromaGreenOffset = chroma->greenOffset;
                m_globalsettings.chromaBlueOffset = chroma->blueOffset;
            }

            if (element.HasComponent<PostProcessingBloom>())
            {
                auto bloom = element.GetComponent<PostProcessingBloom>();
                m_globalsettings.bloomThreshold = bloom->threshold;
                m_globalsettings.bloomIntensity = bloom->intensity;
                m_globalsettings.bloomRadius = bloom->radius;
            }

            if (element.HasComponent<PostProcessingVignette>())
            {
                auto vignette = element.GetComponent<PostProcessingVignette>();
                m_globalsettings.vignetteIntensity = vignette->intensity;
                m_globalsettings.vignetteRadius = vignette->radius;
                m_globalsettings.vignetteSoftness = vignette->softness;
            }

            if (element.HasComponent<PostProcessingColorGrading>())
            {
                auto colorGrade = element.GetComponent<PostProcessingColorGrading>();
                m_globalsettings.colorBrightness = colorGrade->brightness;
                m_globalsettings.colorContrast = colorGrade->contrast;
                m_globalsettings.colorSaturation = colorGrade->saturation;
            }

            if (element.HasComponent<PostProcessingPixelate>())
            {
                auto pixelate = element.GetComponent<PostProcessingPixelate>();
                m_globalsettings.pixelWidth = pixelate->pixelWidth;
                m_globalsettings.pixelHeight = pixelate->pixelHeight;
            }

            if (element.HasComponent<PostProcessingFilmGrain>())
            {
                auto filmGrain = element.GetComponent<PostProcessingFilmGrain>();
                m_globalsettings.filmGrainIntensity = filmGrain->grainIntensity;
                m_globalsettings.filmGrainSize = filmGrain->grainSize;
                m_globalsettings.filmGrainAnimate = filmGrain->animateGrain;
            }

            // Retrieve z-index if available.
            if (element.HasComponent<ZIndex>())
                postProcessZIndex = element.GetComponent<ZIndex>()->z;
        }
        #pragma endregion

        #pragma region Run Postprocessing Effects
        // === Step 1: Process Local Post-Processing ===
        // Bind local framebuffer and render objects that have post-processing components.
        //m_LocalFramebuffer->Bind();
        // Clear local framebuffer as needed.
        // Here, you would iterate through all objects with a PostProcessingComponent,
        // then apply their stack of post-processing effects.
        //ProcessLocalPostProcessing();
        //DrawLocalPostProcessing();
        //m_LocalFramebuffer->Unbind();

        // === Step 2: Render Scene to Global Framebuffer ===
        // At this stage, your regular RenderingLayer might have rendered the scene.
        // You may now composite the local post-processed textures with the main scene.
        // For example, you can render the local processed texture onto m_GlobalFramebuffer.
        // Toggle settings using bottom keys:
        if (Input::GetKeyDown(GLFW_KEY_Z)) // Toggle bloom
            m_globalsettings.enableBloom = !m_globalsettings.enableBloom;
        if (Input::GetKeyDown(GLFW_KEY_X)) // Toggle Gaussian Blur
            m_globalsettings.enableGaussianBlur = !m_globalsettings.enableGaussianBlur;
        if (Input::GetKeyDown(GLFW_KEY_C)) // Toggle Chromatic Aberration
            m_globalsettings.enableChromaticAberration = !m_globalsettings.enableChromaticAberration;
        if (Input::GetKeyDown(GLFW_KEY_V)) // Toggle Vignette
            m_globalsettings.enableVignette = !m_globalsettings.enableVignette;
        if (Input::GetKeyDown(GLFW_KEY_B)) // Toggle Color Grading
            m_globalsettings.enableColorGrading = !m_globalsettings.enableColorGrading;
        if (Input::GetKeyDown(GLFW_KEY_N)) // Toggle Film Grain
            m_globalsettings.enableFilmGrain = !m_globalsettings.enableFilmGrain;
        if (Input::GetKeyDown(GLFW_KEY_M)) // Toggle Pixelate
            m_globalsettings.enablePixelate = !m_globalsettings.enablePixelate;

        if (Input::GetKeyDown(GLFW_KEY_T)) m_globalsettings.bloomThreshold += 0.05f;
        if (Input::GetKeyDown(GLFW_KEY_G)) m_globalsettings.bloomThreshold -= 0.05f;

        // Bloom Intensity
        if (Input::GetKeyDown(GLFW_KEY_Y)) m_globalsettings.bloomIntensity += 0.05f;
        if (Input::GetKeyDown(GLFW_KEY_H)) m_globalsettings.bloomIntensity -= 0.05f;

        // Bloom Radius
        if (Input::GetKeyDown(GLFW_KEY_U)) m_globalsettings.bloomRadius += 1.0f;
        if (Input::GetKeyDown(GLFW_KEY_J)) m_globalsettings.bloomRadius -= 1.0f;

        // Blur Kernel Size
        if (Input::GetKeyDown(GLFW_KEY_I)) m_globalsettings.blurIntensity = std::min(m_globalsettings.blurIntensity + 1, 64);
        if (Input::GetKeyDown(GLFW_KEY_K)) m_globalsettings.blurIntensity = std::max(m_globalsettings.blurIntensity - 1, 1);

        // Blur Sigma
        if (Input::GetKeyDown(GLFW_KEY_O)) m_globalsettings.blurDistance += 0.5f;
        if (Input::GetKeyDown(GLFW_KEY_L)) m_globalsettings.blurDistance -= 0.5f;

        // Blur Passes
        if (Input::GetKeyDown(GLFW_KEY_P)) m_globalsettings.blurPasses = std::min(m_globalsettings.blurPasses + 1, 10);
        if (Input::GetKeyDown(GLFW_KEY_SEMICOLON)) m_globalsettings.blurPasses = std::max(m_globalsettings.blurPasses - 1, 1);

        // === Step 3: Process Global Post-Processing ===
        // Bind global framebuffer if not already bound and apply global effects.
        ProcessGlobalPostProcessing();
        DrawGlobalPostProcessing();
        #pragma endregion

        #pragma region Merge Results
        Window::FrameBufferManager.SetCurrentFrameBuffer("Final Post Processing");

        GLuint globaltexture = Window::FrameBufferManager.GetFrameBuffer("Global Post Processing")->GetColorAttachment();
        //GLuint localtexture = Window::FrameBufferManager.GetFrameBuffer("Local Post Processing")->GetColorAttachment();
        ReplicateFrameBufferAttachment(globaltexture);

        #pragma endregion

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
    }

    void PostProcessing::DrawGlobalPostProcessing()
    {
        Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
        GLuint globaltexture = Window::FrameBufferManager.GetFrameBuffer("Global Post Processing")->GetColorAttachment();
        GLuint bloomtexture = Window::FrameBufferManager.GetFrameBuffer("Bloom")->GetColorAttachment();
        GLuint gaussianblurHorizontaltexture = Window::FrameBufferManager.GetFrameBuffer("Gaussian Blur Horizontal")->GetColorAttachment();
        GLuint gaussianblurVerticaltexture = Window::FrameBufferManager.GetFrameBuffer("Gaussian Blur Vertical")->GetColorAttachment();
        GLuint chromaticaberration_texture = Window::FrameBufferManager.GetFrameBuffer("Chromatic Aberration")->GetColorAttachment();
        GLuint colorgradingtexture = Window::FrameBufferManager.GetFrameBuffer("Color Grading")->GetColorAttachment();
        GLuint vignettetexture = Window::FrameBufferManager.GetFrameBuffer("Vignette")->GetColorAttachment();
        GLuint filmgraintexture = Window::FrameBufferManager.GetFrameBuffer("Film Grain")->GetColorAttachment();
        GLuint pixelatetexture = Window::FrameBufferManager.GetFrameBuffer("Pixelate")->GetColorAttachment();

        // ---------- Bloom Pipeline ----------
        if (m_globalsettings.enableBloom)
        {
            // Step 1: Brightness Extraction - isolate bright areas based on the bloom threshold.
            Window::FrameBufferManager.SetCurrentFrameBuffer("Bloom");
            OpenGLRenderer::ApplyBrightnessPass(globaltexture, m_globalsettings.bloomThreshold);

            // Step 2: Gaussian Blur as part of the bloom chain.
            GLuint inputTex = bloomtexture;
            bool horizontal = true; // Start with a horizontal blur.
            for (int i = 0; i < m_globalsettings.blurPasses; ++i)
            {
                if (horizontal)
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Horizontal");
                else
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Vertical");
                OpenGLRenderer::ApplyGaussianBlur(inputTex, m_globalsettings.blurDistance, m_globalsettings.blurIntensity, horizontal);
                inputTex = horizontal ? gaussianblurHorizontaltexture : gaussianblurVerticaltexture;
                horizontal = !horizontal;
            }

            // Step 3: Final Bloom Composition - combine the blurred highlights back with the original scene.
            Window::FrameBufferManager.SetCurrentFrameBuffer("Bloom");
            OpenGLRenderer::ApplyBloomFinalComposition(globaltexture, gaussianblurHorizontaltexture, gaussianblurVerticaltexture, m_globalsettings.bloomIntensity);

            // Step 4: Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(bloomtexture);

            // Step 6: Reset
            Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Horizontal");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Vertical");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Bloom");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Apply a full-screen Gaussian blur on the scene if enabled.
        if (m_globalsettings.enableGaussianBlur)
        {
            GLuint inputTex = globaltexture;
            bool horizontal = true; // Start with a horizontal blur.
            for (int i = 0; i < m_globalsettings.blurPasses; ++i)
            {
                if (horizontal)
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Horizontal");
                else
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Vertical");
                OpenGLRenderer::ApplyGaussianBlur(inputTex, m_globalsettings.blurDistance, m_globalsettings.blurIntensity, horizontal);
                inputTex = horizontal ? gaussianblurHorizontaltexture : gaussianblurVerticaltexture;
                horizontal = !horizontal;
            }

            // Merge results
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            OpenGLRenderer::ApplyBlurFinalComposition(gaussianblurHorizontaltexture, gaussianblurVerticaltexture);

            // Reset frame buffers
            Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Horizontal");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Gaussian Blur Vertical");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Chromatic Aberration: apply color separation if enabled.
        if (m_globalsettings.enableChromaticAberration)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Chromatic Aberration");
            OpenGLRenderer::ApplyChromaticAberration(
                inputTex,
                m_globalsettings.chromaIntensity,
                m_globalsettings.chromaMaxOffset,
                m_globalsettings.chromaRedOffset,
                m_globalsettings.chromaGreenOffset,
                m_globalsettings.chromaBlueOffset
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(chromaticaberration_texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Chromatic Aberration");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Color Grading / Tone Mapping: adjust brightness, contrast, and saturation.
        if (m_globalsettings.enableColorGrading)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Color Grading");
            OpenGLRenderer::ApplyColorGrading(
                inputTex,
                m_globalsettings.colorBrightness,
                m_globalsettings.colorContrast,
                m_globalsettings.colorSaturation
                //, m_globalsettings.lutTexturePath if required
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(colorgradingtexture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Color Grading");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Vignette: darken edges to draw attention to the center.
        if (m_globalsettings.enableVignette)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Vignette");
            OpenGLRenderer::ApplyVignette(
                inputTex,
                m_globalsettings.vignetteIntensity,
                m_globalsettings.vignetteRadius,
                m_globalsettings.vignetteSoftness
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(vignettetexture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Vignette");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Film Grain: overlay a subtle noise effect.
        if (m_globalsettings.enableFilmGrain)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Film Grain");
            OpenGLRenderer::ApplyFilmGrain(
                inputTex,
                m_globalsettings.filmGrainIntensity,
                m_globalsettings.filmGrainSize,
                m_globalsettings.filmGrainAnimate
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(filmgraintexture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Film Grain");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Pixelate: optionally apply a pixelation effect as a final overlay.
        if (m_globalsettings.enablePixelate)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pixelate");
            OpenGLRenderer::ApplyPixelate(
                inputTex,
                (float)m_globalsettings.pixelWidth,
                (float)m_globalsettings.pixelHeight
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(pixelatetexture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pixelate");
            OpenGLRenderer::ClearFrameBuffer();
        }
    }

    void PostProcessing::ReplicateFrameBufferAttachment(GLuint texture)
    {
        Vector2 windowSize = Vector2((float)FlexEngine::Application::GetCurrentWindow()->GetWidth(), (float)FlexEngine::Application::GetCurrentWindow()->GetHeight());
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker>())
        {
            if (!element.GetComponent<Transform>()->is_active)
                continue;
            Matrix4x4 transform = element.GetComponent<Transform>()->transform;

            OpenGLRenderer::DrawTexture2D(texture, transform, windowSize);
        }
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
