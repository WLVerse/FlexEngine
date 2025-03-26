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
        Window::FrameBufferManager.AddFrameBuffer("Pass 1", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Pass 2", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Pass 3", window_size);
        Window::FrameBufferManager.AddFrameBuffer("Pass 4", window_size);
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
        Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 3");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 4");
        OpenGLRenderer::ClearFrameBuffer();
        #pragma endregion

        #pragma region Update Settings
        // Finalizing Post Processing
        Vector2 windowSize = Vector2((float)FlexEngine::Application::GetCurrentWindow()->GetWidth(), (float)FlexEngine::Application::GetCurrentWindow()->GetHeight());
        for (auto& element : FlexECS::Scene::GetActiveScene()->CachedQuery<PostProcessingMarker, Transform>())
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
            m_globalsettings.enableGaussianBlur = PPComponent->enableGaussianBlur;
            m_globalsettings.enableChromaticAberration = PPComponent->enableChromaticAberration;
            m_globalsettings.enableBloom = PPComponent->enableBloom;
            m_globalsettings.enableVignette = PPComponent->enableVignette;
            m_globalsettings.enableColorGrading = PPComponent->enableColorGrading;
            m_globalsettings.enableFilmGrain = PPComponent->enableFilmGrain;
            m_globalsettings.enablePixelate = PPComponent->enablePixelate;
            m_globalsettings.globalIntensity = PPComponent->globalIntensity;

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
                m_globalsettings.chromaRedOffset = chroma->redOffset;
                m_globalsettings.chromaGreenOffset = chroma->greenOffset;
                m_globalsettings.chromaBlueOffset = chroma->blueOffset;
                m_globalsettings.chromaEdgeRadius = chroma->edgeRadius;
                m_globalsettings.chromaEdgeSoftness = chroma->edgeSoftness;
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
        ProcessLocalPostProcessing();
        DrawGlobalPostProcessing();
        #pragma endregion

        OpenGLFrameBuffer::Unbind();
    }

    void PostProcessing::ProcessLocalPostProcessing()
    {
        Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");

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
                // If the entity has any post processing components, use the local post processing draw call.
                if (element.HasComponent<PostProcessingGaussianBlur>() ||
                    element.HasComponent<PostProcessingChromaticAbberation>() ||
                    element.HasComponent<PostProcessingBloom>() ||
                    element.HasComponent<PostProcessingVignette>() ||
                    element.HasComponent<PostProcessingColorGrading>() ||
                    element.HasComponent<PostProcessingPixelate>())
                {
                    prePostProcessingQueue.Insert({ [&element]() {
                        PostProcessing::DrawLocalPostProcessing(element);
                    }, "", entityZIndex });
                    continue;
                }

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
                // If the entity has any post processing components, use the local post processing draw call.
                if (element.HasComponent<PostProcessingGaussianBlur>() ||
                    element.HasComponent<PostProcessingChromaticAbberation>() ||
                    element.HasComponent<PostProcessingBloom>() ||
                    element.HasComponent<PostProcessingVignette>() ||
                    element.HasComponent<PostProcessingColorGrading>() ||
                    element.HasComponent<PostProcessingPixelate>())
                {
                    prePostProcessingQueue.Insert({ [&element]() {
                        PostProcessing::DrawLocalPostProcessing(element);
                    }, "", entityZIndex });
                    continue;
                }

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

    void PostProcessing::DrawLocalPostProcessing(FlexECS::Entity& entity)
    {
        if (!CameraManager::has_main_camera) return;

        Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
        GLuint localtexture = Window::FrameBufferManager.GetFrameBuffer("Local Post Processing")->GetColorAttachment();
        GLuint globaltexture = Window::FrameBufferManager.GetFrameBuffer("Global Post Processing")->GetColorAttachment();
        GLuint pass1texture = Window::FrameBufferManager.GetFrameBuffer("Pass 1")->GetColorAttachment();
        GLuint pass2texture = Window::FrameBufferManager.GetFrameBuffer("Pass 2")->GetColorAttachment();
        GLuint pass3texture = Window::FrameBufferManager.GetFrameBuffer("Pass 3")->GetColorAttachment();
        GLuint pass4texture = Window::FrameBufferManager.GetFrameBuffer("Pass 4")->GetColorAttachment();
        // Draw Entity in local framebuffer
        if (entity.HasComponent<Sprite>())
        {
            Sprite& sprite = *entity.GetComponent<Sprite>();
            Renderer2DProps props;

            // Check if the sprite has an animator component.
            if (entity.HasComponent<Animator>() && FLX_STRING_GET(entity.GetComponent<Animator>()->spritesheet_handle) != "")
            {
                Animator& animator = *entity.GetComponent<Animator>();
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
            props.world_transform = entity.GetComponent<Transform>()->transform;

            OpenGLRenderer::DrawTexture2D(*CameraManager::GetMainGameCamera(), props);
        }
        else if (entity.HasComponent<Text>())
        {
            const auto textComponent = entity.GetComponent<Text>();
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
                entity.GetComponent<Scale>()->scale.x, 0.00, 0.00, 0.00,
                0.00, entity.GetComponent<Scale>()->scale.y, 0.00, 0.00,
                0.00, 0.00, entity.GetComponent<Scale>()->scale.z, 0.00,
                entity.GetComponent<Position>()->position.x,
                entity.GetComponent<Position>()->position.y,
                entity.GetComponent<Position>()->position.z, 1.00
            );
            textProps.m_alignment = std::pair{
                static_cast<Renderer2DText::AlignmentX>(textComponent->alignment.first),
                static_cast<Renderer2DText::AlignmentY>(textComponent->alignment.second)
            };
            textProps.m_textboxDimensions = textComponent->textboxDimensions;
            textProps.m_linespacing = 12.0f;

            OpenGLRenderer::DrawTexture2D(*CameraManager::GetMainGameCamera(), textProps);
        }
        else
        {
            return;
        }

        // ---------- Bloom Pipeline ---------- (GLOW)
        if (entity.HasComponent<PostProcessingBloom>())
        {
            // Step 1: Brightness Extraction - isolate bright areas based on the bloom threshold.
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyBrightnessPass(localtexture, entity.GetComponent<PostProcessingBloom>()->threshold);

            // Step 2: Gaussian Blur as part of the bloom chain.
            GLuint inputTex = pass1texture;
            bool horizontal = true; // Start with a horizontal blur.
            for (int i = 0; i < m_globalsettings.blurPasses; ++i) // Uses generic values for this
            {
                if (horizontal)
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
                else
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 3");
                OpenGLRenderer::ApplyGaussianBlur(inputTex, m_globalsettings.blurDistance, m_globalsettings.blurIntensity, horizontal); // Uses generic values for this
                inputTex = horizontal ? pass2texture : pass3texture;
                horizontal = !horizontal;
            }

            // Step 3: Final Bloom Composition - combine the blurred highlights back with the original scene.
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyBloomFinalComposition(localtexture, pass2texture, pass3texture, entity.GetComponent<PostProcessingBloom>()->intensity, entity.GetComponent<PostProcessingBloom>()->radius);

            // Step 4: Update Local FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            // Step 6: Reset
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 3");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Apply a full-screen Gaussian blur on the scene if enabled. (Avoid using, looks weird)
        if (entity.HasComponent<PostProcessingGaussianBlur>())
        {
            GLuint inputTex = localtexture;
            bool horizontal = true; // Start with a horizontal blur.
            for (int i = 0; i < entity.GetComponent<PostProcessingGaussianBlur>()->blurPasses; ++i)
            {
                if (horizontal)
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
                else
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
                OpenGLRenderer::ApplyGaussianBlur(inputTex, entity.GetComponent<PostProcessingGaussianBlur>()->distance,
                    entity.GetComponent<PostProcessingGaussianBlur>()->intensity, horizontal);
                inputTex = horizontal ? pass1texture : pass2texture;
                horizontal = !horizontal;
            }

            // Merge results
            Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
            OpenGLRenderer::ApplyBlurFinalComposition(pass1texture, pass2texture);

            // Reset frame buffers
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Chromatic Aberration: apply color separation if enabled. (Glitching effect)
        if (entity.HasComponent<PostProcessingChromaticAbberation>())
        {
            GLuint inputTex = localtexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyChromaticAberration(
                inputTex,
                entity.GetComponent<PostProcessingChromaticAbberation>()->intensity,
                entity.GetComponent<PostProcessingChromaticAbberation>()->redOffset,
                entity.GetComponent<PostProcessingChromaticAbberation>()->greenOffset,
                entity.GetComponent<PostProcessingChromaticAbberation>()->blueOffset,
                entity.GetComponent<PostProcessingChromaticAbberation>()->edgeRadius,
                entity.GetComponent<PostProcessingChromaticAbberation>()->edgeSoftness
            );

            // Update Local FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Color Grading / Tone Mapping: adjust brightness, contrast, and saturation. (Not ideal, supposed to use color add to sprite, use vignette instead for brightness)
        if (entity.HasComponent<PostProcessingColorGrading>())
        {
            GLuint inputTex = localtexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyColorGrading(
                inputTex,
                entity.GetComponent<PostProcessingColorGrading>()->brightness,
                entity.GetComponent<PostProcessingColorGrading>()->contrast,
                entity.GetComponent<PostProcessingColorGrading>()->saturation
            );

            // Update Local FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Vignette: darken edges to draw attention to the center. (Make brighter or darker)
        if (entity.HasComponent<PostProcessingVignette>())
        {
            GLuint inputTex = localtexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyVignette(
                inputTex,
                entity.GetComponent<PostProcessingVignette>()->intensity,
                entity.GetComponent<PostProcessingVignette>()->radius,
                entity.GetComponent<PostProcessingVignette>()->softness
            );

            // Update Local FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Pixelate: optionally apply a pixelation effect as a final overlay. (Pixelate)
        if (entity.HasComponent<PostProcessingPixelate>())
        {
            GLuint inputTex = localtexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyPixelate(
                inputTex,
                (float)entity.GetComponent<PostProcessingPixelate>()->pixelWidth,
                (float)entity.GetComponent<PostProcessingPixelate>()->pixelHeight
            );

            // Update Local FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Film Grain is not needed here (Not going to implement)

        // Merge results from local frame buffer to global frame buffer
        Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 4");
        OpenGLRenderer::ApplyOverlay(globaltexture, localtexture);
        Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
        ReplicateFrameBufferAttachment(pass4texture);

        // Clear local frame buffer
        Window::FrameBufferManager.SetCurrentFrameBuffer("Local Post Processing");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 4");
        OpenGLRenderer::ClearFrameBuffer();
        Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
    }

    void PostProcessing::DrawGlobalPostProcessing()
    {
        if (!CameraManager::has_main_camera) return;

        Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
        GLuint globaltexture = Window::FrameBufferManager.GetFrameBuffer("Global Post Processing")->GetColorAttachment();
        GLuint pass1texture = Window::FrameBufferManager.GetFrameBuffer("Pass 1")->GetColorAttachment();
        GLuint pass2texture = Window::FrameBufferManager.GetFrameBuffer("Pass 2")->GetColorAttachment();
        GLuint pass3texture = Window::FrameBufferManager.GetFrameBuffer("Pass 3")->GetColorAttachment();
        // ---------- Bloom Pipeline ----------
        if (m_globalsettings.enableBloom)
        {
            // Step 1: Brightness Extraction - isolate bright areas based on the bloom threshold.
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyBrightnessPass(globaltexture, m_globalsettings.bloomThreshold);

            // Step 2: Gaussian Blur as part of the bloom chain.
            GLuint inputTex = pass1texture;
            bool horizontal = true; // Start with a horizontal blur.
            for (int i = 0; i < m_globalsettings.blurPasses; ++i)
            {
                if (horizontal)
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
                else
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 3");
                OpenGLRenderer::ApplyGaussianBlur(inputTex, m_globalsettings.blurDistance, m_globalsettings.blurIntensity, horizontal);
                inputTex = horizontal ? pass2texture : pass3texture;
                horizontal = !horizontal;
            }

            // Step 3: Final Bloom Composition - combine the blurred highlights back with the original scene.
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyBloomFinalComposition(globaltexture, pass2texture, pass3texture, m_globalsettings.bloomIntensity, m_globalsettings.bloomRadius);

            // Step 4: Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            // Step 6: Reset
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 3");
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
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
                else
                    Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
                OpenGLRenderer::ApplyGaussianBlur(inputTex, m_globalsettings.blurDistance, m_globalsettings.blurIntensity, horizontal);
                inputTex = horizontal ? pass1texture : pass2texture;
                horizontal = !horizontal;
            }

            // Merge results
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            OpenGLRenderer::ApplyBlurFinalComposition(pass1texture, pass2texture);

            // Reset frame buffers
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 2");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Chromatic Aberration: apply color separation if enabled.
        if (m_globalsettings.enableChromaticAberration)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyChromaticAberration(
                inputTex,
                m_globalsettings.chromaIntensity,
                m_globalsettings.chromaRedOffset,
                m_globalsettings.chromaGreenOffset,
                m_globalsettings.chromaBlueOffset,
                m_globalsettings.chromaEdgeRadius,
                m_globalsettings.chromaEdgeSoftness
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Color Grading / Tone Mapping: adjust brightness, contrast, and saturation.
        if (m_globalsettings.enableColorGrading)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyColorGrading(
                inputTex,
                m_globalsettings.colorBrightness,
                m_globalsettings.colorContrast,
                m_globalsettings.colorSaturation
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Vignette: darken edges to draw attention to the center.
        if (m_globalsettings.enableVignette)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyVignette(
                inputTex,
                m_globalsettings.vignetteIntensity,
                m_globalsettings.vignetteRadius,
                m_globalsettings.vignetteSoftness
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Film Grain: overlay a subtle noise effect.
        if (m_globalsettings.enableFilmGrain)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyFilmGrain(
                inputTex,
                m_globalsettings.filmGrainIntensity,
                m_globalsettings.filmGrainSize,
                m_globalsettings.filmGrainAnimate
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Pixelate: optionally apply a pixelation effect as a final overlay.
        if (m_globalsettings.enablePixelate)
        {
            GLuint inputTex = globaltexture;
            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ApplyPixelate(
                inputTex,
                (float)m_globalsettings.pixelWidth,
                (float)m_globalsettings.pixelHeight
            );

            // Update Global FrameBuffer
            Window::FrameBufferManager.SetCurrentFrameBuffer("Global Post Processing");
            ReplicateFrameBufferAttachment(pass1texture);

            Window::FrameBufferManager.SetCurrentFrameBuffer("Pass 1");
            OpenGLRenderer::ClearFrameBuffer();
        }

        // Draw to final post processing buffer
        Window::FrameBufferManager.SetCurrentFrameBuffer("Final Post Processing");
        ReplicateFrameBufferAttachment(globaltexture);
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
