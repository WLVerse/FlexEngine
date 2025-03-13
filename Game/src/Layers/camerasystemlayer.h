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
        struct ShakeEffect
        {
            float duration;   // Total duration of the effect.
            float elapsed;    // Time elapsed so far.
            float intensity;  // Base shake intensity.
            bool lerp;        // If true, intensity ramps up then down.
        };

        // Updated ZoomEffect:
        // - For persistent zoom (non auto-return), 'duration' is used.
        // - For auto-return zoom, we add lerpDuration (ramp up/down),
        //   holdDuration (time at target zoom), and totalDuration = 2*lerpDuration + holdDuration.
        struct ZoomEffect
        {
            // Persistent zoom effect parameters.
            float duration;         // Total duration for persistent zoom effect.

            // Common parameters:
            float elapsed;          // Time elapsed so far.
            float targetOrthoWidth; // Desired target orthographic width.
            float initialOrthoWidth;// Starting orthographic width.
            bool autoReturn;        // If true, zoom will ramp in then return.

            // Auto-return effect parameters (used when autoReturn is true):
            float lerpDuration;     // Lerp duration for ramping up and down.
            float holdDuration;     // Duration to hold at target zoom.
            float totalDuration;    // Total duration (2*lerpDuration + holdDuration).
        };

        std::vector<ShakeEffect> m_shakeEffects;
        ZoomEffect m_zoomEffect;

        // Original camera position (base for applying shake).
        Vector3 m_originalCameraPos;
        float m_zoomBase = 1920.0f; // Original ortho width of camera
        bool m_zoomActive = false;
        const float m_baseAspectRatio = 16.0f / 9.0f;
        const float m_minOrthoWidth = 640.0f; // Smallest ortho width allowed

        // Helper functions.
        Vector3 GenerateShakeOffset(float intensity);
    };

} // namespace Game
