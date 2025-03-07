// WLVERSE [https://wlverse.web.app]
// camerasystemlayer.h / cameraservicelayer.h
// 
// Layer that handles the updates to validating cameras to update in CamManager
//
// AUTHORS
// [100%] Soh Wei Jie (weijie_soh.c\@digipen.edu)
//   - Main Author
// 
// Copyright (c) 2024 DigiPen, All rights reserved.

#pragma once

#include <FlexEngine.h>
using namespace FlexEngine;

namespace Game {

    // The CameraSystemLayer handles camera shake and zoom effects.
    class CameraSystemLayer : public FlexEngine::Layer {
    public:
        CameraSystemLayer();
        ~CameraSystemLayer() override = default;

        void OnAttach() override;
        void OnDetach() override;
        void Update() override;

    private:
        // Main camera entity and base parameters.
        FlexECS::Entity m_mainCameraEntity;
        float m_zoomBase = 0.0f; //Original Ortho width of camera
        const float m_baseAspectRatio = 16.0f / 9.0f;
        const float m_minOrthoWidth = 1000.0f;

        struct ShakeEffect 
        {
            float duration;   // Total duration of the effect.
            float elapsed;    // Time elapsed so far.
            float intensity;  // Base shake intensity.
            bool lerp;        // If true, intensity ramps up then down.
        };
        struct ZoomEffect 
        {
            float duration;         // Total duration of the effect.
            float elapsed;          // Time elapsed so far.
            float targetOrthoWidth; // Desired target orthographic width.
            float initialOrthoWidth;// Starting orthographic width.
            bool autoReturn;        // If true, zoom will ramp in then return.
        };

        std::vector<ShakeEffect> m_shakeEffects;
        ZoomEffect m_zoomEffect;

        // Original camera position (base for applying shake).
        Vector3 m_originalCameraPos;

        // Helper functions.
        void EnsureMainCamera();
        Vector3 GenerateShakeOffset(float intensity);
    };

} // namespace Game